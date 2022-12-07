# Holo-assistant
This repo houses code for Maria: an anime pseudo-holographic virtual assistant than runs on a Raspberry Pi 4B. This README contains some basic instructions for running Maria, but more thorough documentation is on [this website](http://maria.awardwinninghuman.com/).




![maria_loop](https://user-images.githubusercontent.com/3848662/200412149-21e1398f-4635-4482-bc0f-957f53ed6800.gif)





## About the project
Currently, I have the following:
- A Python server that listens for keywords using deepspeech and webrtvt (based on this [example](https://github.com/mozilla/DeepSpeech-examples/tree/r0.9/mic_vad_streaming)) 
- A C program with a 3D character that uses Google's text-to-speech library to respond to queries. Currently this behaviour includes the following skill:
    - Gives the current and the next day's weather using [weatherApi](https://www.weatherapi.com/) 
    - Runs a simple timer program
    - Plays music using the Spotify API on a local device (not Sonos though :pensive:)
- A simple shader to perspective-correct the character based on Roxanne Luo's [Pepper's Cone](https://github.com/roxanneluo/Pepper-s-Cone-Unity) (based on the Pepper's ghost illusion)

## Platforms
This project is developed on an M1 Mac running MacOs 12.3.

This project is used on a Raspberry Pi 4B running Raspberry Pi OS release 04-04-2022.

## Pre-requisites
Requirements for serverAudio are recorded in serverAudio/requirements.txt. You may need to futz a bit with library installations on your Raspberry Pi. For example, you may need to run `sudo apt-get install libatlas-base-dev` to use some numpy-based dependencies, and you may need to run `sudo apt install python3-pyaudio` for gaps related to PortAudio/PyAudio (as used by sounddevice).

For clientCharacter, make sure you install [Raylib](https://github.com/raysan5/raylib).

I used a homebrew installation of Raylib on Mac, and for Raspberry Pi, I installed used `sudo make install PLATFORM=PLATFORM_DESKTOP GRAPHICS=GRAPHICS_API_OPENGL_21`.


## Compiling and Running

Run the server/language processing program with
```
cd serverAudio && ./../holovenv/bin/python main.py
```
Though replace `holovenv` with your own virtual env path. :wink:



On a Mac, you can compile and run the client/3D character with
```
cd clientCharacter && cc holoAssistant.c character.c stopWatch.c -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib` -o HoloAssistant && ./HoloAssistant
```

On a Raspberry Pi 4, you can compile and run the client/3D character with
```
cd clientCharacter && cc holoAssistant.c character.c stopWatch.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o HoloAssistant && ./HoloAssistant 
```
If you are ssh-ing in, you may need to run `export DISPLAY=:0.0` before using the call above.
