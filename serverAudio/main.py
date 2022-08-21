import time, logging
from datetime import datetime
import time
import threading, queue, os, os.path
import sys
import vosk
import sounddevice as sd
from pydub import AudioSegment
from pydub.playback import play
import socket
import configparser
import json
import requests
import urllib.parse
import geograpy


logging.basicConfig(level=20)


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

def int_or_str(text):
    """Helper function for argument parsing."""
    try:
        return int(text)
    except ValueError:
        return text


def synthesize_text(text, file_loc):
    """Synthesizes speech from the input string of text."""
    import os
    from google.cloud import texttospeech
    os.environ['GOOGLE_APPLICATION_CREDENTIALS'] = file_loc

    client = texttospeech.TextToSpeechClient()

    input_text = texttospeech.SynthesisInput(text=text)

    # Note: the voice can also be specified by name.
    # Names of voices can be retrieved with client.list_voices().
    voice = texttospeech.VoiceSelectionParams(
        language_code="en-US",
        name="en-US-Standard-F",
        ssml_gender=texttospeech.SsmlVoiceGender.FEMALE,
    )

    audio_config = texttospeech.AudioConfig(
        audio_encoding=texttospeech.AudioEncoding.LINEAR16,
        pitch=2.5
    )

    response = client.synthesize_speech(
        request={"input": input_text, "voice": voice, "audio_config": audio_config}
    )

    # The response's audio_content is binary.
    with open("./resources/sound_clips/latest_output.wav", "wb") as out:
        out.write(response.audio_content)

def get_condition(json_string, condition):
    for hour in json_string["hour"]:
        if condition in hour["condition"]["text"].lower():
            return hour["time"][-5:]
        return False

def get_weather(key, text):
    weather_base_url = "http://api.weatherapi.com/v1/"
    weather_string = ""
    where_var = "auto:ip"
    loc_attempt = text.find(" in ")
    if loc_attempt > -1:
        loc_string = text[(loc_attempt + 4):]
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
        return weather_string
    else:
        print(weather_current_response.status_code)
        return -1


def main():
    HOST = "127.0.0.1"  # The server's hostname or IP address
    PORT = 65432  # The port used by the server
    start_time = time.time()
    config = configparser.ConfigParser()
    config.sections()
    config.read('./settings.config')
    weather_key = config['DEFAULT']['weatherApiKey']
    speaking = False #is maria currently speaking?
    command_mode = False
    q = queue.Queue()
    wake_word = "maria"

    def callback(indata, frames, time, status):
        """This is called (from a separate thread) for each audio block."""
        if status:
            print(status, file=sys.stderr)
        q.put(bytes(indata))

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.bind((HOST, PORT))
            s.listen()
            conn, addr = s.accept()

            device_info = sd.query_devices(None, 'input')
            sample_rate = int(device_info['default_samplerate'])
            # Load vosk model
            model = vosk.Model(lang="en-us")

            def exit_listen(conn):
                global command_mode
                command_mode = False
                conn.sendall(b'exit listen\n')
                print("heard exit command")

            t = infinite_timer(15, exit_listen, conn)

            with sd.RawInputStream(samplerate=sample_rate, blocksize = 12000, 
                device=0, dtype='int16', channels=1, latency=0.3, callback=callback):
                print('#' * 80)
                print('Press Ctrl+C to stop the recording')
                print('#' * 80)

                rec = vosk.KaldiRecognizer(model, sample_rate)

                while True:
                    data = q.get()
                    if rec.AcceptWaveform(data):
                        heard = json.loads(rec.Result())["text"]
                        if command_mode:
                            if "whether" in heard or "weather" in heard:
                                speaking = True
                                t.cancel()
                                conn.sendall(b'exit listen\n')
                                weather_resp = get_weather(weather_key, heard)
                                synthesize_text(weather_resp, config['DEFAULT']['googleFileLocation'])
                                conn.sendall(b'talk\n')
                                if weather_resp == -1:
                                    sound = AudioSegment.from_file("./resources/sound_clips/sorry_no_info.wav", format="wav")
                                    play(sound)
                                else:
                                    sound = AudioSegment.from_file("./resources/sound_clips/latest_output.wav", format="wav")
                                    play(sound)
                                conn.sendall(b'exit talk\n')
                                command_mode = False
                                speaking = False
                        else:
                            continue
                        print(heard)
                    else:
                        heard = json.loads(rec.PartialResult())["partial"]
                        if command_mode == False and wake_word in heard:
                            conn.sendall(b'listen\n')
                            command_mode = True
                            start_time = time.time()
                            t.cancel()
                            t.start()


    except KeyboardInterrupt:
        print("Exiting server...")
    except Exception as e:
        print(str(e))


if __name__ == '__main__':
    main()