#pragma once
#include "ofxThreadedImageLoader.h"

class ofxThreadedImageReloader {
public:

    bool loading;
    ofxThreadedImageLoader loader;
    ofImage img;
    string nextFilename;

    ofxThreadedImageReloader() {
        loading = false;
        nextFilename = "";
    }

    void update() {
        if (!loading && nextFilename!="") {
            loader.setup(nextFilename);
            loading = true;
            loader.startThread();
            nextFilename = "";
        }

        if (loading==true && !loader.isThreadRunning()){
            img.getPixels() = loader.image;
            img.update();
            loading = false;
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