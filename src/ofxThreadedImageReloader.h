//By Rick Companje
//March 14, 2016

#pragma once
#include "ofxThreadedImageLoader.h"

class ofxThreadedImageReloader {
public:

  bool loading;
  ofxThreadedImageLoader loader;
  ofImage img;
  string nextFilename;
  float loadTime;

  ofxThreadedImageReloader() {
    loading = false;
    nextFilename = "";
  }

  void update() {
    static float startTime = 0;
    
    if (!loading && nextFilename!="") {
      loader.setup(nextFilename);
      startTime = ofGetElapsedTimef();
      loading = true;
      loader.startThread();
      nextFilename = "";
    }

    if (loading==true && !loader.isThreadRunning()){
      img.getPixels() = loader.image;
      img.update();
      loading = false;
      loadTime = ofGetElapsedTimef() - startTime; //in seconds
    }
  }

  void load(string filename) {
    nextFilename = filename;
  }

  void draw() {
    if (img.isAllocated()) {
      img.draw(0, 0);
    }
  }

};