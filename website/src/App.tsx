import React from 'react';
import Step from './components/Step';
import logo from './assets/maria.svg';
import instructionsTitle from './assets/instructions.svg';
import bottomFrame from './assets/bottom-ornamental-frame.svg';
import topFrame from './assets/top-ornamental-frame.svg';
import drawing from './assets/drawing.png';
import './App.css';


function App() {
  return (
    <div className="contentWrapper">
      <div className="content">
        <header>
          <img src={logo} className="fullWidth" alt="" />
          <h1 className="sr-only">Maria</h1>
          <h2>An anime-inspired holographic virtual assistant you can make at home!</h2>
          <div className="heroVideo">
            <div className="overlay">
              <img src={topFrame} className="frameOverlay" alt="" />
              <img src={drawing} className="drawing" alt="An anime character says in Japanese 'I am powered by Raspberry Pi.'"/>
              <img src={bottomFrame} className="frameOverlay" alt="" />
            </div>
          </div>
        </header>
        <nav>
            <h3 className="sr-only">Instructions</h3>
            <img src={instructionsTitle} className="fullWidth" alt="" />
            <p className="navList">
              <span>Assemble your materials</span>
              <span>Sign up for APIs</span>
              <span>Set up your pi with a keyboard and mouse</span>
            </p>
          </nav>
        <main>
          <Step title={"Assemble your materials"}>
            <p>Labore ipsum velit. Architecto nobis est provident est. Ut fuga temporibus nihil sed. Sed voluptates iure repellat ducimus ex. Porro mollitia voluptate in quisquam quibusdam natus aliquam.</p>
            <p>Labore ipsum velit. Architecto nobis est provident est. Ut fuga temporibus nihil sed. Sed voluptates iure repellat ducimus ex. Porro mollitia voluptate in quisquam quibusdam natus aliquam.</p>
          </Step>
          <Step title={"Sign up for APIs"}>
            <p>Boop</p>
          </Step>
          <Step title={"Set up your pi with a keyboard and mouse"}>
            <p>Boop</p>
          </Step>
        </main>
      </div>
    </div>
  );
}

export default App;
