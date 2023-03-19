import json
import requests
import urllib.parse
import time    
from charactercontroller import CharacterController



class DictionarySkill(CharacterController):
	def __init__(self, connection, global_timer, google_key, callback, terms, dictionary_key):
		super().__init__(connection, global_timer, google_key, callback, terms)
		self.dictionary_key = dictionary_key


	def listen(self, heard):
		flattened_terms = [item for sublist in self.terms for item in sublist]
		dictionary_resp = self.get_definition(self.dictionary_key, heard, flattened_terms)
		if dictionary_resp[0] == -1:
			self.talk("sorry_no_info.wav")
		elif dictionary_resp[1] == 0:
			self.talk("not_a_word.wav")
		else:
			self.talk("found_definition.wav")
			self.synthesize_text(dictionary_resp[1])
			self.talk("latest_output.wav")

	def get_definition(self, key, text, terms):
		definition_base_url = "https://api.api-ninjas.com/v1/dictionary?word="
		found_word = ""
		for term in terms:
			attempt = text.partition(term)[2]
			if attempt != '':
				found_word = attempt.strip().split(" ")[0]
				continue
		if found_word == "":
			return (-1)
		else:
			definition_response = requests.get((definition_base_url + found_word), headers={'X-Api-Key': self.dictionary_key})
			if definition_response.status_code == requests.codes.ok:
				json_resp = definition_response.json()
				if json_resp["valid"] == True:
					return (1, json_resp["definition"])
				else:
					return (0)
			else:
				return (-1)
		