import time, logging
from datetime import datetime
import threading, collections, queue, os, os.path
import deepspeech
import numpy as np
# import pyaudio
import sounddevice as sd
import soundfile as sf
import wave
# import webrtcvad
from halo import Halo
from scipy import signal

import tempfile
import queue
import sys
import os

import sounddevice as sd
import soundfile as sf
import numpy  # Make sure NumPy is loaded before it is used in the callback
assert numpy  # avoid "imported but unused" message (W0611)



q = queue.Queue()


def callback(indata, frames, time, status):
    """This is called (from a separate thread) for each audio block."""
    if status:
        print(status, file=sys.stderr)
    q.put(indata.copy())


device_info = sd.query_devices(0, 'input')
# soundfile expects an int, sounddevice provides a float:
samplerate = int(device_info['default_samplerate'])
filename = ("test.wav")
if os.path.isfile(filename):
    os.remove(filename)
# Make sure the file is opened before recording anything:
try:
    with sf.SoundFile(filename, mode='x', samplerate=44100,
                      channels=1) as file:
        with sd.InputStream(samplerate=44100, device=0,
                            channels=1, callback=callback):
            print('#' * 80)
            print('press Ctrl+C to stop the recording')
            print('#' * 80)
            while True:
                file.write(q.get())
except KeyboardInterrupt:
    print('\nRecording finished: ' + repr(filename))