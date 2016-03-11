#include "ofMain.h"
#include "ofxThreadedImageSequence.h"

class ofApp : public ofBaseApp {
public:

  ofxThreadedImageSequence sequence;
  float pos, speed;
  
  void setup() {
    ofBackground(0);
    ofDisableArbTex();
    ofEnableDepthTest();
    sequence.setup("/Users/rick/frames/frame-%d.bmp",1,372);
    pos = 0;
    speed = .01;
  }

  void update() {
    pos += speed;
    if (pos>1) { pos=1; speed=-speed; }
    if (pos<0) { pos=0; speed=-speed; }
    
    sequence.setPosition(pos);
    sequence.update();
  }

  void draw() {
    if (!sequence.loader.img.isAllocated()) {
      ofDrawBitmapString("No image loaded...", 5, 15);
      return;
    }

    ofSetColor(255);
    ofSetupScreenOrtho(ofGetWidth(),ofGetHeight(),-1500,1500);
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    ofRotateY(mouseX);
    ofRotateX(mouseY);
    sequence.loader.img.bind();
    ofDrawSphere(ofGetHeight()/2);
    ofSetupScreen();
    ofSetColor(255,255,0);
    ofDrawBitmapString("Use +/- to change play speed", 5, 15);
    ofDrawBitmapString("Use mouse to rotate sphere", 5, 30);
    ofDrawBitmapString("app framerate: " + ofToString(ofGetFrameRate()), 5, 45);
  }
  
  void keyPressed(int key) {
    if (key=='+' || key=='=') speed*=2;
    if (key=='-') speed/=2;
  }

};

//========================================================================
int main() {
  ofSetupOpenGL(1024,768,OF_WINDOW);
  ofRunApp(new ofApp());
}
