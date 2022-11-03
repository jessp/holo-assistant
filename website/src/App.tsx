import React from 'react';
import Header from './components/Header';
import Step from './components/Step';
import instructionsTitle from './assets/instructions.svg';
import nameFrame from './assets/name-border.svg';
import './App.css';



function App() {
    return (
      <div className="contentWrapper">
        <div className="content">
          <Header/>
          <nav>
              <h3 className="sr-only">Instructions</h3>
              <img src={instructionsTitle} className="fullWidth" alt="" />
              <p className="navList">
                <span><a href="#step-1">Clone my Github repo</a></span>
                <span><a href="#step-2">Get your materials ready</a></span>
                <span><a href="#step-3">Sign up for APIs</a></span>
                <span><a href="#step-4">Install Python Dependencies</a></span>
                <span><a href="#step-5">Compile and install Raylib</a></span>
                <span><a href="#step-6">Prep your Raspberry Pi for use</a></span>
                <span><a href="#step-7">Build Maria's home</a></span>
                <span><a href="#step-8">Run Maria</a></span>
                <span><a href="#step-9">Enjoy asking Maria about cool stuff</a></span>
              </p>
            </nav>
          <main>
            <Step title={"Clone my Github repo"} index={1}>
              <p>Power up your Raspberry Pi. <a href="https://www.howtogeek.com/768053/how-to-ssh-into-your-raspberry-pi/" target="blank">SSH into your Pi from a computer on the same network.</a></p>
              <p>In your terminal, navigate to whichever directory you want to run Maria from. It doesn't really matter. Clone my Github repository into your folder.</p>
              <p>That might look something like <code>cd Documents && git clone https://github.com/jessp/holo-assistant.git</code>.</p>
            </Step>
            <Step title={"Get your materials ready"} index={2}>
              <p>I wanted Maria to be as easy and affordable to assemble as possible. That said, you need some things to get started. 
              I've linked to some of the specific items I used (no affiliate links), but please consider alternatives that might be easier 
              to source for you.</p>
              <p className={"bold"}>Materials</p>
              <ul>
                <li>Raspberry Pi 4B running a recent Raspberry Pi OS</li>
                <li>Micro SD card and charger, for your Raspberry Pi</li>
                <li>The cheapest keyboard you can find, for set-up</li>
                <li>The cheapest mouse you can find, for set-up</li>
                <li><a href="https://thepihut.com/products/3-2-ips-hdmi-lcd-display-for-raspberry-pi-480x800" target="blank">A 3.2", 800x400 screen, ideally one that connects over HDMI</a></li>
                <li><a href="https://thepihut.com/products/mini-external-usb-stereo-speaker" target="blank">Small USB speaker</a></li>
                <li><a href="https://thepihut.com/products/mini-usb-microphone" target="blank">Miniature USB microphone</a></li>
                <li>Acetate sheets</li>
                <li>3D-printed case (optional), including:</li>
                <ul>
                  <li>Box for Raspberry Pi and peripherals, printed in PLA FDM</li>
                  <li>Lid for Raspberry Pi and peripherals, printed in PLA FDM</li>
                  <li>Cage for surrounding cone, printed in PA12 Nylon MJF</li>
                  <li>Cap for cage, printed in PLA FDM</li>
                </ul>
              </ul>

              <p>I note that the case is optional because, honestly, it's a bit pricy to 3D print, and my modelling skills aren't great.
              You can absolutely use those files, but I also encourage you to be creative with whatever materials you have around.</p>
            </Step>
            <Step title={"Sign up for APIs"} index={3}>
              <p>Maria is powered by a few different APIs. You'll need to sign up for accounts in order to interact with her.</p>
              <p className={"bold"}>APIs</p>
              <ul>
                <li>Signing up for <a href="https://www.weatherapi.com/" target="blank">WeatherAPI.com</a> is pretty straight-forward.</li>
                <li>Sign up for a <a href="https://developer.spotify.com/" target="blank">Spotify developer account</a>. Create an app. Under the app's settings, enter http://localhost:9876 under Redirect URIs.</li>
                <li>Sign up for <a href="https://console.cloud.google.com/" target="blank">Google Cloud's</a> Text-to-Speech service. </li>
                <ul>
                  <li>I find that Google Cloud's services a bit tricky to navigate, so I recommend following the steps <a href="https://acloudguru.com/hands-on-labs/setting-up-google-cloud-text-to-speech" target="blank">here</a> under the headings "Enable the Cloud Text-to-Speech API" and "Set Up Service Account."</li>
                  <li>Place the json file you download in holo-assistant/serverAudio/googleCredentials.</li>
                </ul>
              </ul>
              <p>Once you have all your keys, create a new file called settings.config in holo-assistant/serverAudio. Copy the structure of the file from settings.config.example, but fill in the keys you acquired by signing up for the API services.</p>
            </Step>
            <Step title={"Install Python dependencies"} index={4}>
              <p>Still SSH-ing into your Pi, <a href="https://docs.python.org/3/library/venv.html" target="blank">create a virtual environment</a> at the top-level of your holo-assistant directory named "holovenv."" To do this, run the command <code>python3 -m venv holovenv</code>.</p>
              <p>Activate the environment by running <code>source holovenv/bin/activate</code>. While in holovenv, do the following:</p>
              <ul>
                <li>Install Python libraries by running <code>pip install -r serverAudio/requirements.txt</code></li>
                <li>Run <code>sudo apt-get install libatlas-base-dev</code> and <code>sudo apt install python3-pyaudio</code> to prevent some Raspberry Pi-specific issues.</li>
              </ul>
              <p>Exit your virtual environment.</p>
            </Step>
            <Step title={"Compile and install Raylib"} index={5}>
              <p>Via SSH, navigate outside of your holo-assistant directory to where you want to download <a href="https://www.raylib.com/" target="blank">Raylib</a> to. Raylib is the 3D library used to animate Maria.</p>
              <p>Download Raylib using <code>git clone https://github.com/raysan5/raylib</code>. Navigate into your newly download Raylib folder.</p>
              <p>Raylib has a <a href="https://github.com/raysan5/raylib/wiki/Working-on-Raspberry-Pi" target="blank">pretty great Wiki</a> on installing Raylib on Raspberry Pi.</p>
              <p>At the risk of mostly reiterating the steps described in the Wiki entry, I did the following:</p>
              <ul>
                <li>Run <code>sudo apt-get install --no-install-recommends raspberrypi-ui-mods lxterminal gvfs</code></li>
                <li>Run <code>sudo apt-get install libx11-dev libxcursor-dev libxinerama-dev libxrandr-dev libxi-dev libasound2-dev mesa-common-dev libgl1-mesa-dev</code></li>
                <li>Navigate into raylib/src and then run <code>sudo make install PLATFORM=PLATFORM_DESKTOP GRAPHICS=GRAPHICS_API_OPENGL_21</code></li>
              </ul>
              <p>If you end up faffing around with nothing working, try enabling x11 forwarding on your Pi per <a href="https://techsphinx.com/raspberry-pi/enable-x11-forwarding-on-raspberry-pi/" target="blank">this article</a>. I never quite confirmed if it was necessary for Raylib.</p>
            </Step>
            <Step title={"Prep your Raspberry Pi for use"} index={6}>
              <p>Plug your crappy keyboard and mouse into your Raspberry Pi. If you haven't already, connect your screen as well.</p>
              <p>Use Raspberry Pi's settings to rotate your screen orientation and select your speakers as your audio output. I promise, it's so much easier using the GUI than trying to do this over SSH.</p>
              <p>Open the terminal on your Raspberry Pi. Navigate into holo-assistant. Run <code>cd serverAudio && ./../holovenv/bin/python runmefirst.py</code>.</p>
              <p>The main thing runmefirst.py does is trigger the Spotify authentication process, which must be done within the browser of the device you are using it on. It also includes some calls to attempt to trigger a geography library we use, <a href="https://geograpy3.readthedocs.io/en/latest/" target="blank">geograpy3</a>, to download some essential place-name data we need before we can use it. But that call is pretty hit and miss, and doesn't impact our ability to run Maria.</p>
              <p>Unplug your keyboard and mouse.</p>
            </Step>
            <Step title={"Build Maria's home"} index={7}>
              <p>Maria needs a place to live. All you need is cone made out of an acetate sheet of a paper, but you can also 3D print a special case if you so choose. The video below will walk you through how to assemble Maria's home, and the steps are written out after.</p>
              <iframe className="video"
              src="https://www.youtube.com/embed/ekxollSk66c?modestbranding=1"
              title="YouTube video player"
              frameBorder="0"
              allow="accelerometer; autoplay; encrypted-media; gyroscope;"
              allowFullScreen
              ></iframe>
              <p className={"bold"}>Instructions</p>
              <ol type="i">
                <li>Draw two circles on a sheet of acetate with the same centre point. The smaller circle has a radius of 1.5cm, while the larger circle has a radius of 9.7cm.</li>
                <li>Draw a 90° angle extending from the centre of the circles.</li>
                <li>Draw jagged rectangular "teeth" extending from one line making up the angle.</li>
                <li>Cut out your shape. This will form your cone.</li>
                <li>Tape your cone together so that your jagged teeth overlao with the other edge.</li>
                <li>If you haven't already, plug in your screen to your Raspberry Pi.</li>
                <li>Plug in your microphone to your Raspberry Pi.</li>
                <li>Plug in your speaker to your Raspberry Pi. Place the speaker in the four corners that extend up from the base of the Pi case.</li>
                <li>Position your Raspberry Pi on the raised platform. The bottom of the screen should face towards the speaker.</li>
                <li>Plug your power cord into the Raspberry Pi, then put the lid of the base on. The screen should line up with the rectangular cut-away of the lid.</li>
                <li>Position your cone on the lid of the case. The bottom third of the narrow end of the cone should overlap with the screen.</li>
                <li>Place the cage over your cone. The gap in the cage should face the side of the case the power cable extends from.</li>
                <li>Finally, place the cap over the cage.</li>
              </ol>
            </Step>
            <Step title={"Run Maria"} index={8}>
              <p>Open two terminal tabs that both are SSH-ing into your Raspberry Pi.</p><p>In both tabs, navigate into holo-assistant. In one, run <code>cd serverAudio && ./../holovenv/bin/python main.py</code>. In the other, run <code>cd clientCharacter && cc holoAssistant.c character.c stopWatch.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o HoloAssistant && ./HoloAssistant</code>.</p>
            </Step>
            <Step title={"Ask Maria to do stuff"} index={9}>
              <p>TODO</p>
            </Step>
          </main>
        </div>
      </div>
    );
  }

export default App;
