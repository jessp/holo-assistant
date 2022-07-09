# # https://medium.com/slanglabs/how-to-build-python-transcriber-using-mozilla-deepspeech-5485b8d234cf
# # https://medium.com/slanglabs/automatic-speech-recognition-in-python-programs-a64851ad29b3

# import deepspeech
# import numpy as np
# import os
# import sounddevice as sd
# import time
# import sys
# import queue

# blocksize = 1024

# # DeepSpeech parameters
# model = deepspeech.Model('deepspeech-0.9.3-models.pbmm')
# model.enableExternalScorer('deepspeech-0.9.3-models.scorer')

# # Create a Streaming session
# context = model.createStream()

# device_info = sd.query_devices(0, 'input')
# # soundfile expects an int, sounddevice provides a float:
# samplerate = int(device_info['default_samplerate'])
# # Encapsulate DeepSpeech audio feeding into a callback for PyAudio
# text_so_far = ''

# def process_audio(in_data, frames, time, status):
#     try:
#         assert frames == blocksize
#         if status.input_underflow:
#             print('Input underflow: increase blocksize?', file=sys.stderr)
#             raise sd.CallbackAbort
#         global text_so_far
#         data16 = np.frombuffer(in_data, dtype=np.int16)
#         context.feedAudioContent(data16)
#         text = context.intermediateDecode()
#         if text != text_so_far:
#             print('Interim text = {}'.format(text))
#             text_so_far = text
#     except:
#         print("Ended callback")


# stream = sd.InputStream(samplerate=samplerate, device=0, channels=1, blocksize=blocksize, callback=process_audio)

# print('Please start speaking, when done press Ctrl-C ...')
# stream.start()

# try:
#     while (stream.active):
#         time.sleep(0.1)
# except KeyboardInterrupt:
#     print('Finished recording.')
#     # DeepSpeech
#     stream.stop()
#     text = context.finishStream()
#     print('Final text = {}'.format(text))


import deepspeech as ds
import sounddevice as sd
import os

# https://deepspeech.readthedocs.io/en/v0.9.3/Python-Examples.html
#print("Loading model...")
model = ds.Model('deepspeech-0.9.3-models.pbmm')
#print("Loading scorer...")
model.enableExternalScorer('deepspeech-0.9.3-models.scorer')

def stt(audio = None, temp = None):
    try:
        fs = 16000
        # https://python-sounddevice.readthedocs.io/en/latest/usage.html#recording

        duration = 10 # seconds
        #print("Recording now...")
        audioData = sd.rec(int(duration * fs), samplerate=fs, channels=1, dtype="int16")
        #audioData = sd.rec(samplerate=fs, channels=1, dtype="int16")
        sd.wait()
        #print("Done recording!")

        audioData = list(map(lambda channels : channels[0], audioData))

        #print("Processing...")
        text = model.stt(audioData)
        print(text)
        return text
    except KeyboardInterrupt:
        print("\nDetermining SpeechToText engine...")
    except Exception as e:
        print(e)

if __name__ == '__main__':
    wakeword = "test"
    text = str(stt())
    print(text)
    print("Wakeword: " + wakeword)
    if text.strip().lower().find(wakeword.replace("\n", "").strip().lower()) != -1:
        print("Wake-word was found in spoken text")
    print("Determining audio listener...")