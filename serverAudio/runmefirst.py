import time
import subprocess


print("""

                                                           
          ____                                             
        ,'  , `.                                           
     ,-+-,.' _ |                      ,--,                 
  ,-+-. ;   , ||             __  ,-.,--.'|                 
 ,--.'|'   |  ;|           ,' ,'/ /||  |,                  
|   |  ,', |  ':  ,--.--.  '  | |' |`--'_       ,--.--.    
|   | /  | |  || /       \ |  |   ,',' ,'|     /       \   
'   | :  | :  |,.--.  .-. |'  :  /  '  | |    .--.  .-. |  
;   . |  ; |--'  \__\/: . .|  | '   |  | :     \__\/: . .  
|   : |  | ,     ," .--.; |;  : |   '  : |__   ," .--.; |  
|   : '  |/     /  /  ,.  ||  , ;   |  | '.'| /  /  ,.  |  
;   | |`-'     ;  :   .'   \---'    ;  :    ;;  :   .'   \ 
|   ;/         |  ,     .-./        |  ,   / |  ,     .-./ 
'---'           `--`---'             ---`-'   `--`---'     
                                                           

""")

time.sleep(0.5)

print("This file was made to help you get started with holographic virtual assistant Maria.\n\
In a moment, the file will call actions that should trigger libraries to install various \nprerequisites you need to use Maria's services. \
It's annoying, but it's easier to run \nthese functions up front rather than have them be triggered the first time you use Maria.\n\
I suggest running this file on your Rasbperry Pi with a keyboard and mouse attached. \
These \nare helpful for authenticating with Spotipy--the Python library for Spotify's API--which \nrequires interaction in the broswer.\n")

time.sleep(1)

print("If you don't encounter terminal errors, but get kicked out due to Python installing\n\
a prerequisite, just keep rerunning the file until you see the message saying \"Done!\"\n\n")

time.sleep(1)

print("Installing numpy dependencies for Rasbperry Pi. You're going to have to enter your password...\n\n")

subprocess.run(["sudo apt-get install libatlas-base-dev"])


time.sleep(1)


import geograpy
import spotipy
from spotipy.oauth2 import SpotifyOAuth
import configparser


print("Checking geograpy prerequisites...\n\n")

config = configparser.ConfigParser()
config.sections()
config.read('./settings.config')
client_id = config['DEFAULT']['spotifyClientId']
secret = config['DEFAULT']['spotifyClientSecret']
redirect = config['DEFAULT']['spotifyRedirectURL']

print("Checking Spotipy prerequisites...\n\n")

scope = "user-read-playback-state,user-modify-playback-state"
sp = spotipy.Spotify(auth_manager=SpotifyOAuth(client_id=client_id, client_secret=secret, redirect_uri=redirect, scope=scope))

time.sleep(1)

print("You're good to get started.\n\n")

time.sleep(1)

print("""

                                                ,---,  
                                             ,`--.' |  
    ,---,                                    |   :  :  
  .'  .' `\                                  '   '  ;  
,---.'     \    ,---.        ,---,           |   |  |  
|   |  .`\  |  '   ,'\   ,-+-. /  |          '   :  ;  
:   : |  '  | /   /   | ,--.'|'   |   ,---.  |   |  '  
|   ' '  ;  :.   ; ,. :|   |  ,"' |  /     \ '   :  |  
'   | ;  .  |'   | |: :|   | /  | | /    /  |;   |  ;  
|   | :  |  ''   | .; :|   | |  | |.    ' / |`---'. |  
'   : | /  ; |   :    ||   | |  |/ '   ;   /| `--..`;  
|   | '` ,/   \   \  / |   | |--'  '   |  / |.--,_     
;   :  .'      `----'  |   |/      |   :    ||    |`.  
|   ,.'                '---'        \   \  / `-- -`, ; 
'---'                                `----'    '---`"  
                                                       


""")
