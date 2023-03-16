import React, { useRef, useState } from 'react';
import Vimeo from '@u-wave/react-vimeo';
import drawing from './../assets/drawing.png';
import bottomFrame from './../assets/bottom-ornamental-frame.svg';
import topFrame from './../assets/top-ornamental-frame.svg';
import nameFrame from './../assets/name-border.svg';
import logo from './../assets/maria.svg';
import heart from './../assets/beige-heart.svg';


const Header = () => {


  const [showing, setShowing] = useState(true);

  const playerRef = useRef()

  const onReady = player => {
    playerRef.current = player
  }

  const buttonClick = e => {
    if (playerRef.current && showing){
      playerRef.current.play();
    }
  }


  return (
      <header>

            <div className="nameBanner">
              <div className="bannerLines">
                <img src={nameFrame} alt="" />
              </div>
              <p>A Project by <a href="https://awardwinninghuman.com/" target="blank">J. Peter</a></p>
              <div className="bannerLines">
                <img src={nameFrame} alt="" />
              </div>
            </div>

            <img src={logo} className="fullWidth" alt="" />
            <h1 className="sr-only">Maria</h1>
            <h2>An anime pseudo-holog&shy;raphic virtual assis&shy;tant you can make at home!</h2>
            <div className="heroVideo">
              <div className="overlay">
                <Vimeo
                  video="https://vimeo.com/768220002"
                  id="player"
                  showByline={false}
                  dnt={true}
                  className="video"
                  responsive
                  onPlaying={() => setShowing(false)}
                  onPause={() => setShowing(true)}
                  onReady={onReady}/>
                <img src={topFrame} className="frameOverlay" alt="" />
                <button 
                  className={showing ? "" : "hide"}
                  onClick = {() => buttonClick()}>
                  <img src={heart} alt=""/>
                  <span>play video</span>
                </button>
                <img src={drawing} 
                  className={showing ? "drawing" : "drawing hide"} 
                  alt="An anime character says in Japanese 'I am powered by Raspberry Pi.'"/>
                <img src={bottomFrame} className="frameOverlay" alt="" />
              </div>
            </div>
          </header>
    );
}

export default Header;