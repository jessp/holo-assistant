import React, { Component } from 'react';
import Vimeo from '@u-wave/react-vimeo';
import drawing from './../assets/drawing.png';
import bottomFrame from './../assets/bottom-ornamental-frame.svg';
import topFrame from './../assets/top-ornamental-frame.svg';
import nameFrame from './../assets/name-border.svg';
import logo from './../assets/maria.svg';


class Header extends React.Component {

  constructor(props) {
    super(props);
    this.state = {
      showing: true
    };

  }


  render() {
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
                <Vimeo
                  video="https://vimeo.com/767444828"
                  id="player"
                  showByline={false}
                  dnt={true}
                  className="video"
                  responsive
                  onPlaying={() => this.setState({"showing": false})}
                  onPause={() => this.setState({"showing": true})}/>
                <img src={topFrame} className="frameOverlay" alt="" />
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