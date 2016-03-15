//By Rick Companje
//March 14, 2016

#pragma once
#include "ofxThreadedImageLoader.h"

class ofxThreadedImageReloader {
public:

  bool loading;
  ofxThreadedImageLoader loader;
  ofImage img;
  string curFilename,nextFilename;
  float loadTime;

  ofxThreadedImageReloader() {
    loading = false;
    curFilename = "";
    nextFilename = "";
  }

  void update() {
    static float startTime = 0;
    
    if (!loading && nextFilename!=curFilename) {
      loader.setup(nextFilename);
      startTime = ofGetElapsedTimef();
      loading = true;
      loader.startThread();
      curFilename = nextFilename;
    }

    if (loading==true && !loader.isThreadRunning()){
      img.getPixels() = loader.image;
      img.update();
      loading = false;
      loadTime = ofGetElapsedTimef() - startTime; //in seconds
    }
  }

  void load(string filename) {
    if (filename!=curFilename) {
      nextFilename = filename;
    }
  }

  void draw() {
    if (img.isAllocated()) {
      img.draw(0, 0);
    }
  }

};