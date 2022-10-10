import React, { Component } from 'react';
import './Step.css';

class Step extends React.Component {

  constructor(props) {
    super(props);
  }

  render() {
    return (
      <div className="step">
        <h3 id={`step-${this.props.index}`}>{this.props.title}</h3>
        <div>
          {this.props.children}
        </div>
      </div>
    );
  }
}

export default Step;