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
I suggest running this file on your Rasbperry Pi on its little terminal with a keyboard \nand mouse attached. \
These are helpful for authenticating with Spotipy--the Python library \nfor Spotify's API--which requires interaction in the broswer.\n")

time.sleep(1)

print("If you don't encounter terminal errors, but get kicked out due to Python installing\n\
a prerequisite, just keep rerunning the file until you see the message saying \"Done!\"\n\n")

time.sleep(1)


import geograpy
import spotipy
from spotipy.oauth2 import SpotifyOAuth
import configparser


print("Checking geograpy prerequisites...\n\n")

geograpy.get_geoPlace_context(text="Paris is a city")

config = configparser.ConfigParser()
config.sections()
config.read('./settings.config')
client_id = config['DEFAULT']['spotifyClientId']
secret = config['DEFAULT']['spotifyClientSecret']
redirect = config['DEFAULT']['spotifyRedirectURL']

print("Checking Spotipy prerequisites. You may need to log into your Spotify account...\n\n")

scope = "user-read-playback-state,user-modify-playback-state"
sp = spotipy.Spotify(auth_manager=SpotifyOAuth(client_id=client_id, client_secret=secret, redirect_uri=redirect, scope=scope))
sp.devices()

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
