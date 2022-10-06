import time
from text_to_num import alpha2digit
from infinitetimer import InfiniteTimer
from charactercontroller import CharacterController


class TimerSkill(CharacterController):
	def __init__(self, connection, global_timer, google_key, callback, terms):
		super().__init__(connection, global_timer, google_key, callback, terms)
		self.timer_func = InfiniteTimer(0, 0, self.connection)

	def listen(self, heard):
		if "stop" in heard or "cancel" in heard:
			self.pause_timer()
		else:
			if self.timer_func.is_timer_on():
				#don't interrupt timer from going off to say you can't have multiple timers at once
				if (self.timer_func.remaining() > 3.0):
					self.talk("one_timer.wav")
			else:
				timer_resp = self.parse_timer_phrase(heard)
				self.talk(timer_resp[1])
				if timer_resp[0] != -1:
					self.timer_func.start(timer_resp[2], self.play_timer_sound)
					self.send_command('clock')


	def play_timer_sound(self, conn):
		self.send_command('exit clock')
		time.sleep(0.01)
		self.talk("timer_up.wav")

	def pause_timer(self):
		if self.timer_func.is_timer_on():
			self.timer_func.cancel()
			self.talk("stop_timer.wav")
			self.send_command('exit clock')
		else:
			self.talk("no_timer.wav")

	def iter_time_text(self, split_text, denomination):
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

	def parse_timer_phrase(self, text):
		too_long = ["year", "month", "week", "day"]
		accepted = ["hour", "minute", "second"]
		if any(duration in text for duration in too_long):
			return (-1, "sorry_timer_format.wav")
		elif any(duration in text for duration in accepted):
			decoded_text = alpha2digit(text, "en")
			split_text = decoded_text.split(" ")
			final_time = 0
			hour_time = self.iter_time_text(split_text, {"name": "hour", "value": 3600})
			minute_time = self.iter_time_text(split_text, {"name": "minute", "value": 60})
			second_time = self.iter_time_text(split_text, {"name": "second", "value": 1})
			final_time = (0 if hour_time == -1 else hour_time[0]) + \
			(0 if minute_time == -1 else minute_time[0]) + \
			(0 if second_time == -1 else second_time[0])

			if final_time > 21600:
				return (-1, "sorry_timer_not_long.wav")
			elif hour_time == -1 and minute_time == -1 and second_time == -1:
				return (-1, "sorry_not_catch.wav")
			else:
				print(final_time)
				def concat_statement(in_time):
					print(in_time)
					if in_time != -1:
						return str(in_time[1]) + in_time[2]
					else:
						return ""
				return_statement = "OK! Timer started for "
				return_statement += concat_statement(hour_time)
				return_statement += concat_statement(minute_time)
				return_statement += concat_statement(second_time)
				return_statement += "."
				self.synthesize_text(return_statement)
				return (1, "latest_output.wav", final_time)

		else:
			return (-1, "sorry_not_catch.wav")