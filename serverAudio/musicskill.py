import spotipy
from spotipy.oauth2 import SpotifyOAuth
import json
from charactercontroller import CharacterController


class MusicSkill(CharacterController):
	def __init__(self, connection, global_timer, google_key, client_id, secret, redirect):
		super().__init__(connection, global_timer, google_key)
		scope = "user-read-playback-state,user-modify-playback-state"
		self.sp = spotipy.Spotify(auth_manager=SpotifyOAuth(client_id=client_id, client_secret=secret, redirect_uri=redirect, scope=scope))

	def listen(self, heard):
		device = self.find_device()
		if device == -1:
			self.talk("no_device.wav")
		else:
			#Get next text after play
			the_song = self.find_song(heard.split("play", 1)[1].strip())
			if the_song[0] == -1:
				if the_song[1] != "":
					self.talk(the_song[1])
			else:
				self.synthesize_text(the_song[1])
				self.talk("latest_output.wav")
				try:
					self.send_command('dab')
					self.sp.start_playback(uris=[the_song[2]["uri"]])
				except:
					# generic message on playback errors
					self.talk("backup_song.wav")

	def pause_playback(self):
		device = self.find_device()
		if device == -1:
			print("error with device")
			self.talk("connect_device.wav")
		else:
			try:
				self.talk("stop_song.wav")
				self.sp.pause_playback()
			except:
				print("error attempting to pause")
				self.talk("connect_device.wav")


	def find_song(self, heard):
		# User can specify an artist by using the phrase "as by"
		loc_attempt = heard.find(" as by ")
		search_query = heard
		if loc_attempt > -1:
			song_info = heard.split(" as by ")
			search_query = song_info[0] + " artist:" + song_info[1]
		if search_query != "":
			song = self.sp.search(search_query, market="GB", limit="1")
			try:
				song_res = song["tracks"]["items"][0]
				if song_res["is_playable"] != True:
					return (-1, "no_play_song.wav")
				else:
					result = {"artist": "", "song": "", "uri": ""}
					artist = ""
					if len(song_res["artists"]) > 1:
						artist_list = ([x["name"] for x in song_res["artists"]])
						artist = " and "
						artist = artist.join(artist_list)
					else:
						artist = song_res["artists"][0]["name"]
					result["uri"] = song_res["uri"]
					result["song"] = song_res["name"]
					result["artist"] = artist
					return (1, ("Okay! I'll play %s by %s!"%(result["song"], result["artist"])), result)
			except:
				return (-1, "no_find_song.wav")
		else:
			return (-1, "")


	def find_device(self):
		res = self.sp.devices()
		if (len(res["devices"]) > 0):
			return res
		else:
			return -1



