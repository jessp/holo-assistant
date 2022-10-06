from infinitetimer import InfiniteTimer
from pydub import AudioSegment
from pydub.playback import play
import os, os.path

class CharacterController:
	def __init__(self, connection, global_timer, google_key, callback, terms):
		self.connection = connection
		self.global_timer = global_timer
		self.google_key = google_key
		self.callback = callback
		self.terms = terms

	def hear_value(self, heard):
		if (any(all(word in heard for word in term_list) for term_list in self.terms)):
			print("here")
			self.global_timer.cancel()
			self.connection.sendall(b'exit listen\n')
			self.listen(heard)
			self.callback()

	def talk(self, file):
		self.connection.sendall(b'talk\n')
		sound = AudioSegment.from_file("./resources/sound_clips/" + file, format="wav")
		play(sound)
		self.connection.sendall(b'exit talk\n')

	def send_command(self, command):
		self.connection.sendall(bytes(command + '\n', 'utf-8'))

	def synthesize_text(self, text, out_name="./resources/sound_clips/latest_output.wav"):
		"""Synthesizes speech from the input string of text."""
		import os
		from google.cloud import texttospeech
		os.environ['GOOGLE_APPLICATION_CREDENTIALS'] = self.google_key

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
		with open(out_name, "wb") as out:
			out.write(response.audio_content)