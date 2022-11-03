import React, { Component } from 'react';
import drawing from './../assets/drawing.png';
import bottomFrame from './../assets/bottom-ornamental-frame.svg';
import topFrame from './../assets/top-ornamental-frame.svg';
import nameFrame from './../assets/name-border.svg';
import logo from './../assets/maria.svg';

import YouTube, { YouTubeProps } from 'react-youtube';


class Header extends React.Component {

  constructor(props) {
    super(props);
    this.state = {
      showing: true
    };
  }


  render() {
    const opts = {
      playerVars: {
        autoplay: 0,
        controls: 1,
        cc_load_policy: 0,
        fs: 0,
        iv_load_policy: 3,
        modestbranding: 1,
        rel: 0,
        showinfo: 0
      },
    };
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
            <h2>An anime-inspired holo&shy;graphic virtual assis&shy;tant you can make at home!</h2>
            <div className="heroVideo">
              <div className="overlay">
                <img src={topFrame} className="frameOverlay" alt="" />
                <YouTube videoId="ekxollSk66c" 
                  opts={opts} 
                  className="video" 
                  onPlay={() => this.setState({"showing": false})}
                  onPause={() => this.setState({"showing": true})}
                  onPause={() => this.setState({"showing": true})}
                  id="player" />
                <img src={drawing} 
                  className={this.state.showing ? "drawing" : "drawing hide"} 
                  alt="An anime character says in Japanese 'I am powered by Raspberry Pi.'"/>
                <img src={bottomFrame} className="frameOverlay" alt="" />
              </div>
            </div>
          </header>
    );
  }
}

export default Header;