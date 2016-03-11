#pragma once
#include "ofMain.h"

class ofxThreadedImageLoader: public ofThread {
public:
  
    void setup(string imagePath) {
        this->path = imagePath;
    }

    void threadedFunction(){
        ofLoadImage(image, path);
    }

    ofPixels image;
    string path;
};
