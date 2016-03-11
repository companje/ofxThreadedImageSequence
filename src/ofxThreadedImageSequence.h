#pragma once
#include "ofxThreadedImageReloader.h"

class ofxThreadedImageSequence {
public:
  ofxThreadedImageReloader loader;
  int currentFrame,numFrames;
  int sequenceStartIndex; //sequenceStartIndex is the number of the first file on the disk.
  string format; //for example: frames/frame-%04d.png
  
  ofxThreadedImageSequence() {
    sequenceStartIndex = 0;
    numFrames = 0;
    currentFrame = 0;
  }
  
  void setup(string format, int sequenceStartIndex, int numFrames) {
    this->format = format;
    this->sequenceStartIndex = sequenceStartIndex;
    this->numFrames = numFrames;
    this->currentFrame =  0;
  }
  
  void update() {
    loader.update();
  }
  
  string formatString(string format, int number) {
    char buffer[256];
    sprintf(buffer, format.c_str(), number);
    return (string)buffer;
  }
  
  void setFrame(int frame) { // 0..numFrames
    currentFrame = ofClamp(frame,0,numFrames-1);
    int index = ofMap(frame,0,numFrames-1, sequenceStartIndex, sequenceStartIndex+numFrames-1);
    loader.load(formatString(format,index));
  }
  
  void setPosition(float pos) { //0..1
    setFrame(pos * numFrames);
  }
  
};