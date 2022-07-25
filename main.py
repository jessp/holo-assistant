import time, logging
from datetime import datetime
import threading, collections, queue, os, os.path
import deepspeech
import numpy as np
import sounddevice as sd
import webrtcvad
from halo import Halo
from scipy import signal
import socket
import time
import configparser
import json
import requests
import urllib.parse
import geograpy
import re


logging.basicConfig(level=20)

class Audio(object):
    """Streams raw audio from microphone. Data is received in a separate thread, and stored in a buffer, to be read from."""

    DTYPE = "int16"
    # Network/VAD rate-space
    RATE_PROCESS = 16000
    CHANNELS = 1
    BLOCKS_PER_SECOND = 50

    def __init__(self, callback=None, input_rate=RATE_PROCESS):
        def proxy_callback(in_data, frame_count, time_info, status):
            #pylint: disable=unused-argument
            callback(in_data.copy())
            return (None, 0)
        if callback is None: 
            callback = lambda in_data: self.buffer_queue.put(in_data)
        self.buffer_queue = queue.Queue()
        self.input_rate = input_rate
        self.sample_rate = self.RATE_PROCESS
        self.block_size = int(self.RATE_PROCESS / float(self.BLOCKS_PER_SECOND))
        self.block_size_input = int(self.input_rate / float(self.BLOCKS_PER_SECOND))
        self.pa = sd
        kwargs = {
            'dtype': self.DTYPE,
            'channels': self.CHANNELS,
            'samplerate': self.input_rate,
            'callback': proxy_callback,
            'blocksize': self.block_size_input
        }

        self.stream = self.pa.InputStream(**kwargs)
        self.stream.start()

    def resample(self, data, input_rate):
        """
        Microphone may not support our native processing sampling rate, so
        resample from input_rate to RATE_PROCESS here for webrtcvad and
        deepspeech

        Args:
            data (binary): Input audio stream
            input_rate (int): Input audio rate to resample from
        """
        data16 = np.frombuffer(data, dtype=np.int16)
        resample_size = int(len(data16) / self.input_rate * self.RATE_PROCESS)
        resample = signal.resample(data16, resample_size)
        resample16 = np.array(resample, dtype=np.int16)
        return resample16.tobytes()

    def read_resampled(self):
        """Return a block of audio data resampled to 16000hz, blocking if necessary."""
        return self.resample(data=self.buffer_queue.get(),
                             input_rate=self.input_rate)

    def read(self):
        """Return a block of audio data, blocking if necessary."""
        return self.buffer_queue.get()

    def destroy(self):
        self.stream.stop_stream()
        self.stream.close()
        self.pa.terminate()

    frame_duration_ms = property(lambda self: 1000 * self.block_size // self.sample_rate)


class VADAudio(Audio):
    """Filter & segment audio with voice activity detection."""

    def __init__(self, aggressiveness=3, input_rate=None):
        super().__init__(input_rate=input_rate)
        self.vad = webrtcvad.Vad(aggressiveness)

    def frame_generator(self):
        """Generator that yields all audio frames from microphone."""
        if self.input_rate == self.RATE_PROCESS:
            while True:
                yield self.read()
        else:
            while True:
                yield self.read_resampled()

    def vad_collector(self, padding_ms=450, ratio=0.65, frames=None):
        """Generator that yields series of consecutive audio frames comprising each utterence, separated by yielding a single None.
            Determines voice activity by ratio of frames in padding_ms. Uses a buffer to include padding_ms prior to being triggered.
            Example: (frame, ..., frame, None, frame, ..., frame, None, ...)
                      |---utterence---|        |---utterence---|
        """
        if frames is None: frames = self.frame_generator()
        num_padding_frames = padding_ms // self.frame_duration_ms
        ring_buffer = collections.deque(maxlen=num_padding_frames)
        triggered = False

        frame_idx = 0
        for frame in frames:
            if len(frame) < 640:
                return
            is_speech = self.vad.is_speech(frame, self.sample_rate)
            
            if not triggered:
                ring_buffer.append((frame, is_speech))
                num_voiced = len([f for f, speech in ring_buffer if speech])
                if num_voiced > ratio * ring_buffer.maxlen:
                    triggered = True
                    for f, s in ring_buffer:
                        yield f
                    ring_buffer.clear()

            else:
                yield frame
                frame_idx += 1
                ring_buffer.append((frame, is_speech))
                num_unvoiced = len([f for f, speech in ring_buffer if not speech])
                #track frame index and stop listening if more than 100 frames
                #to prevent overlong periods before reporting back
                if num_unvoiced > ratio * ring_buffer.maxlen or frame_idx > 100:
                    triggered = False
                    frame_idx = 0
                    yield None
                    ring_buffer.clear()



class infinite_timer():
    """
    A Thread that executes infinitely, used to reset to idle position
    """
    def __init__(self, t, hFunction, conn):
        self.t = t
        self.hFunction = hFunction
        self.conn = conn
        self.thread = threading.Timer(self.t, self.handle_function)
        
    def handle_function(self):
        self.hFunction(self.conn)
        self.thread.cancel()
        
    def start(self):
        self.thread = threading.Timer(self.t, self.handle_function)
        self.thread.start()
        
    def cancel(self):
        self.thread.cancel()

def get_extreme(json_string, isHigh):
    get_all_vals = [temp["temp_c"] for temp in json_string["hour"]]
    if isHigh:
        return max(get_all_vals)
    else:
        return min(get_all_vals)

def get_condition(json_string, condition):
    for hour in json_string["hour"]:
        if condition in hour["condition"]["text"].lower():
            return hour["time"][-5:]
        return False



def get_weather(key, text, default_location):
    weather_base_url = "http://api.weatherapi.com/v1/"
    weather_string = ""
    where_var = "auto:ip"
    loc_attempt = text.find(" in ")
    if loc_attempt > -1:
        loc_string = text[loc_attempt + 4):]
        places = geograpy.get_geoPlace_context(text=loc_string.title())
        if len(places.cities) > 0:
            where_var = places.cities[0]
            weather_string += "In %s, "%where_var
    #either we look for the word tomorrow, or we focus on the rest of today
    when_string = "the rest of today"
    when_var = 1
    if "tomorrow" in text:
        when_string = "tomorrow"
        when_var = 2
    
    weather_current_response = requests.get((weather_base_url + "forecast.json?key=%s&days=%s&q=%s")%(key, when_var, where_var))
    if weather_current_response.status_code == 200:
        json_resp = weather_current_response.json()
        the_high = get_extreme(json_resp["forecast"]["forecastday"][when_var - 1], True)
        the_low = get_extreme(json_resp["forecast"]["forecastday"][when_var - 1], False)
        weather_string += "The high for %s will be %s degrees, and the low will be %s degrees."%(when_string,the_high,the_low)
        rain_time = get_condition(json_resp["forecast"]["forecastday"][when_var - 1], "rain")
        if (rain_time != False):
            weather_string += " There may be rain starting at %s."%(rain_time)
        snow_time = get_condition(json_resp["forecast"]["forecastday"][when_var - 1], "snow")
        if (snow_time != False):
            weather_string += " There may be snow starting at %s."%(snow_time)
        print(weather_string)
    else:
        print(weather_current_response.status_code)


def main():
    HOST = "127.0.0.1"  # The server's hostname or IP address
    PORT = 65432  # The port used by the server
    start_time = time.time()
    command_mode = False
    config = configparser.ConfigParser()
    config.sections()
    config.read('settings.config')
    weather_key = config['DEFAULT']['weatherApiKey']

    
    get_weather(weather_key, "weather in tokyo", "")

    def exit_listen(conn):
        global command_mode
        command_mode = False
        conn.sendall(b'exit listen')
        print("heard exit command")

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.bind((HOST, PORT))
            s.listen()
            conn, addr = s.accept()
            device_info = sd.query_devices(None, 'input')
            sample_rate = int(device_info['default_samplerate'])
            DEFAULT_SAMPLE_RATE = sample_rate
            # Load DeepSpeech model
            print('Initializing model...')
            model = deepspeech.Model('deepspeech-0.9.3-models.pbmm')
            model.enableExternalScorer('deepspeech-0.9.3-models.scorer')
            model.addHotWord("maria", 100)

            # Start audio with VAD
            vad_audio = VADAudio(aggressiveness=2,
                                input_rate=DEFAULT_SAMPLE_RATE)
            print("Listening (ctrl-C to exit)...")
            frames = vad_audio.vad_collector()
            # Stream from microphone to DeepSpeech using VAD
            spinner = Halo(spinner='line')
            stream_context = model.createStream()

            t = infinite_timer(8, exit_listen, conn)


            for frame in frames:
                if frame is not None:
                    if spinner: spinner.start()
                    logging.debug("streaming frame")
                    try:
                        stream_context.feedAudioContent(np.frombuffer(frame, np.int16))
                    except:
                        stream_context = model.createStream()
                        print("Unended stream error...")
                else:
                    if spinner: spinner.stop()
                    logging.debug("end utterence")
                    try:
                        text = stream_context.finishStream()
                    except:
                        text = ""
                        print("Finish stream error...")
                    print("Recognized: %s" % text)
                    if "maria" in text:
                        print("heard name...")
                        conn.sendall(b'listen')
                        command_mode = True
                        start_time = time.time()
                        t.cancel()
                        t.start()
                    if command_mode:
                        if "whether" in text or "weather" in text:
                            conn.sendall(b'talk')
                            command_mode = False
                    else:
                        continue

                    stream_context = model.createStream()
    except KeyboardInterrupt:
        print("Exiting server...")


if __name__ == '__main__':
    main()