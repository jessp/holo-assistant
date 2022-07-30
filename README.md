# Holo-assistant
This repo houses code for what will hopefully one day be a voice-activated virtual assistant with a simple holographic display. Currently, though, it's just a bunch of bits and pieces.

## About the project
Currently, I have the following:
- A Python server that listens for keywords using deepspeech and webrtvt (based on this [example](https://github.com/mozilla/DeepSpeech-examples/tree/r0.9/mic_vad_streaming)) 
- A C program with a 3D character that uses Google's text-to-speech library to respond to queries. Currently this behaviour includes the following skill:
 - Gives the current and the next day's weather using [weatherApi](https://www.weatherapi.com/) 
- A simple shader to perspective-correct the character based on Roxanne Luo's [Pepper's Cone](https://github.com/roxanneluo/Pepper-s-Cone-Unity)

## Roadmap
Eventually, this is meant to be run on a Raspberry Pi with all the hardware and stuff to make it a holographic virtual assistant. Will it get there? Probably not.

## Pre-requisites
Requirements for serverAudio are recorded in serverAudio/requirements.txt.

For clientCharacter, install the following dependencies:
- [Raylib](https://github.com/raysan5/raylib)

## Compiling and Running

Run the server/language processing program with
```
cd serverAudio && /usr/local/bin/envs/pepperscone/bin/python main.py
```
Though replace the env path and name with your own. :wink:



Compile and run the client/3D character with
```
cd clientCharacter && cc holoAssistant.c character.c -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib` -o HoloAssistant && ./HoloAssistant
```
