# Holo-assistant
This repo houses code for what will hopefully one day be a voice-activated virtual assistant with a simple holographic display. Currently, though, it's just a bunch of bits and pieces.

## About the project
Currently, I have the following:
- A Python server that listens for a wakeword ("Maria") using deepspeech and webrtvt (based on this [example](https://github.com/mozilla/DeepSpeech-examples/tree/r0.9/mic_vad_streaming)) 
- A C program with a 3D character and a client that plays a cute animation when the wakeword is uttered
- A simple shader to perspective-correct the character based on Roxanne Luo's [Pepper's Cone](https://github.com/roxanneluo/Pepper-s-Cone-Unity)

## Roadmap
Eventually, this is meant to be run on a Raspberry Pi with all the hardware and stuff to make it a holographic virtual assistant. Will it get there? Probably not.

## Pre-requisites
Relies on deepspeech, webrtcvad, and Raylib, among other things. Will write more later.
TO WRITE

## Running
TO WRITE
