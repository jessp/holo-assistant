import json
import geograpy
import requests
import urllib.parse
import time    
from charactercontroller import CharacterController



class WeatherSkill(CharacterController):
	def __init__(self, connection, global_timer, google_key, callback, terms, weather_key):
		super().__init__(connection, global_timer, google_key, callback, terms)
		self.weather_key = weather_key

	def listen(self, heard):
		weather_resp = self.get_weather(self.weather_key, heard)
		self.synthesize_text(weather_resp[1])
		if weather_resp[0] == -1:
			self.talk("sorry_no_info.wav")
		else:
			self.talk("latest_output.wav")
		

	def get_condition(self, json_string, condition, is_tomorrow):
		now_epoch = int(time.time())
		for hour in json_string["hour"]:
			if is_tomorrow or hour["time_epoch"] > now_epoch:
				if condition == "rain" and hour["chance_of_rain"] > 50:
					return hour["time"][-5:]
				elif condition == "snow" and hour["chance_of_snow"] > 50:
					return hour["time"][-5:]
			return False

	def get_extreme(self, json_string, isHigh):
		get_all_vals = [temp["temp_c"] for temp in json_string["hour"]]
		if isHigh:
			return max(get_all_vals)
		else:
			return min(get_all_vals)

	def get_weather(self, key, text):
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
			the_high = self.get_extreme(json_resp["forecast"]["forecastday"][when_var - 1], True)
			the_low = self.get_extreme(json_resp["forecast"]["forecastday"][when_var - 1], False)
			weather_string += "The high for %s will be %s degrees, and the low will be %s degrees."%(when_string,the_high,the_low)
			rain_time = self.get_condition(json_resp["forecast"]["forecastday"][when_var - 1], "rain", when_var - 1)
			if (rain_time != False):
				weather_string += " There may be rain starting at %s."%(rain_time)
			snow_time = self.get_condition(json_resp["forecast"]["forecastday"][when_var - 1], "snow", when_var - 1)
			if (snow_time != False):
				weather_string += " There may be snow starting at %s."%(snow_time)
			print(weather_string)
			return (1, weather_string)
		else:
			print(weather_current_response.status_code)
			return (-1)