import time, logging
from datetime import datetime
import time
import queue
import sys
import vosk
import sounddevice as sd
import socket
import configparser
import json
import urllib.parse
from charactermodule import Charactermodule
from infinitetimer import Infinitetimer
from weathermodule import Weathermodule
from timermodule import Timermodule


logging.basicConfig(level=20)



def int_or_str(text):
	"""Helper function for argument parsing."""
	try:
		return int(text)
	except ValueError:
		return text



def iter_time_text(split_text, denomination):
	final_time = 0
	idx = 0
	for word in split_text:
		if denomination["name"] in word:
			try:
				the_time = int(split_text[idx-1])
				final_time = the_time * denomination["value"]
				return (final_time, the_time, denomination["name"] + "s" if the_time > 1 else denomination["name"])
			except:
				return -1
		idx = idx + 1
	return -1


def main():
	HOST = "127.0.0.1"  # The server's hostname or IP address
	PORT = 65432  # The port used by the server
	start_time = time.time()
	config = configparser.ConfigParser()
	config.sections()
	config.read('./settings.config')
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


			t = InfiniteTimer(15, exit_listen, conn)
			weather_module = WeatherModule(conn, t, config['DEFAULT']['googleFileLocation'], config['DEFAULT']['weatherApiKey'])
			timer_module = TimerModule(conn, t, config['DEFAULT']['googleFileLocation'])

			with sd.RawInputStream(samplerate=sample_rate, blocksize = 12500, 
				device=0, dtype='int16', channels=1, latency=0.35, callback=callback):
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
								weather_module.hear_value()
								weather_module.listen(heard)
								command_mode = False
							elif "timer" in heard:
								timer_module.hear_value()
								timer_module.listen(heard)
								command_mode = False
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