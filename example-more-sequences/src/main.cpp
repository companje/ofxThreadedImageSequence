//By Rick Companje
//March 14, 2016

#include "ofMain.h"
#include "ofxThreadedImageSequencePlayer.h"

class ofApp : public ofBaseApp {
public:

  vector<ofxThreadedImageSequencePlayer*> sequence; //we need to store pointers here because due to the use of ofThread our class's copy constructor is deleted
  ofxThreadedImageSequencePlayer *cur; //, *prev;
  ofQuaternion qTo;
  ofFbo bg,fg;
  ofShader shader;
  float fadeTimer;
  ofImage normalmap, bordermap;
  bool shaderEnabled;
  bool showHelp, showThumbs;
  
  float bumpDetail,bumpAlpha,borderAlpha,gridAlpha;
  ofPoint lightPos;
  
  void setup() {
    ofBackground(0);
    ofDisableArbTex();
    ofEnableDepthTest();
    ofSetSphereResolution(128);
    
    string folder = "/Users/rick/Documents/Globe4D/image-sequences/";
    vector<string> maps = ofSplitString("history2k,airtraffic,tsunami,juliaset,waterworld,watermars,clouds-201509",",");
    
    for (int i=0; i<maps.size(); i++) {
      ofxThreadedImageSequencePlayer *seq = new ofxThreadedImageSequencePlayer();
      cout << "preloading " << maps[i] << "... ";
      seq->load(folder + maps[i]);
      cout << seq->files.size() << " files" << endl;
      seq->play();
      seq->update();
      sequence.push_back(seq);
    }
    
    cur = sequence[0];
    fadeTimer = 1;
    
    lightPos.set(ofGetHeight()/2,ofGetHeight()/2,ofGetHeight()); //?
    bumpDetail = .99;
    bumpAlpha = .2;
    borderAlpha = .2;
    gridAlpha = .2;
    shaderEnabled = false;
    showThumbs = false;
    showHelp = false;
    
    shader.load("globe");
    normalmap.load("normalmap.png");
    bordermap.load("borders.gif");
    
    fg.allocate(ofGetHeight(), ofGetHeight());
    bg.allocate(ofGetHeight(), ofGetHeight());
    bg.begin();
    ofClear(0);
    bg.end();
  }

  void update() {
    cur->update(); //only the current sequence is being updated
    if (fadeTimer<1) fadeTimer+=1*1./60;
    if (fadeTimer>1) fadeTimer=1; //clamp
  }
  
  void applyRotation() {
    static ofVec3f fromAxis,toAxis;
    static float fromAngle=0,toAngle;
    qTo.getRotate(toAngle,toAxis);
    fromAxis += (toAxis-fromAxis) * .1;
    fromAngle += (toAngle-fromAngle) * .1;
    ofRotate(fromAngle, fromAxis.x, fromAxis.y, fromAxis.z);
  }

  void draw() {
    ofBackgroundGradient(ofColor(100),ofColor(0));

    //draw sphere into foreground fbo
    fg.begin();
    ofClear(0);
    ofSetColor(255);
    ofFill();
    ofEnableDepthTest();
    ofSetupScreenOrtho(fg.getHeight(),fg.getHeight(),-fg.getHeight()/2,0);
    ofTranslate(fg.getWidth()/2,fg.getHeight()/2);
    applyRotation();
    cur->getTexture().bind();
    if (shaderEnabled) {
      shader.begin();
      shader.setUniformTexture("colormap", *cur, 0);
      shader.setUniformTexture("bordermap", bordermap.getTexture(), 1);
      shader.setUniformTexture("normalmap", normalmap.getTexture(), 2);
      shader.setUniform3fv("lightPos", lightPos.getPtr());
      shader.setUniform1f("bumpDetail", bumpDetail);
      shader.setUniform1f("bumpAlpha", bumpAlpha);
      shader.setUniform1f("borderAlpha", borderAlpha);
      shader.setUniform1f("gridAlpha", gridAlpha);
      ofDrawSphere(fg.getHeight()/2);
      shader.end();
    } else {
      ofDrawSphere(fg.getHeight()/2);
    }
    ofDisableDepthTest();
    fg.end();
    
    //draw fbo(s) on screen and crossfade if needed
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2-fg.getWidth()/2,0); //center fbo on screen
    if (fadeTimer<1) {
      ofEnableAlphaBlending();
      ofSetColor(255,255-fadeTimer*255);
      bg.draw(0,0);
    }
    ofSetColor(255,fadeTimer*255);
    fg.draw(0,0);
    ofPopMatrix();
    
    //titles
    if (showHelp) {
      ofSetColor(255,255,0);
      int y=5;
      ofDrawBitmapString("Use +/- to change play speed", 5, y+=10);
      ofDrawBitmapString("Use 'P' to toggle palindrome looping ("+ofToString(cur->getLoopState()==OF_LOOP_PALINDROME)+")", 5, y+=10);
      ofDrawBitmapString("Use 'R' to reverse", 5, y+=10);
      ofDrawBitmapString("Use mouse to rotate sphere", 5, y+=10);
      ofDrawBitmapString("app framerate: " + ofToString(ofGetFrameRate()), 5, y+=10);
      ofDrawBitmapString("sequence fps: " + ofToString(cur->getFrameRate()), 5, y+=10);
      ofDrawBitmapString("speed factor: " + ofToString(cur->getSpeed()), 5, y+=10);
      ofDrawBitmapString("speed * fps: " + ofToString(cur->getSpeed() * cur->getFrameRate()), 5, y+=10);
      ofDrawBitmapString("current sequence frame: " + ofToString(cur->getCurrentFrame()), 5, y+=10);
      ofDrawBitmapString("current app frame: " + ofToString(ofGetFrameNum()), 5, y+=10);
      ofDrawBitmapString("image load fps: " + ofToString(1/cur->loader.loadTime), 5, y+=10);
    }
    
    //thumbs
    if (showThumbs) {
      for (int i=0; i<sequence.size(); i++) {
        ofPushMatrix();
        ofPushStyle();
        ofSetColor(255);
        ofNoFill();
        ofTranslate(i*(128)+10, ofGetHeight()-64-10);
        if (sequence[i]->isInitialized()) sequence[i]->draw(0,0, 128, 64);
        ofSetLineWidth(sequence[i]==cur ? 4 : 1);
        ofDrawRectangle(0,0, 128, 64);
        ofPopStyle();
        ofPopMatrix();
      }
      
      //fade
      ofPushMatrix();
      ofTranslate(10,ofGetHeight()-218);
      ofSetColor(255-fadeTimer*255);
      bg.draw(0,0,128,128);
      ofSetColor(fadeTimer*255);
      fg.draw(128,0,128,128);
      ofEnableAlphaBlending();
      ofSetColor(255,255-fadeTimer*255);
      bg.draw(256,0,128,128);
      ofSetColor(255,fadeTimer*255);
      fg.draw(256,0,128,128);
      ofSetColor(255);
      ofNoFill();
      ofDrawRectangle(0,0,128,128);
      ofDrawRectangle(128,0,128,128);
      ofDrawRectangle(256,0,128,128);
      ofPopMatrix();
    }
  }
  
  void keyPressed(int key) {
    if (key=='-') cur->setSpeed(cur->getSpeed()/1.1);
    if (key=='+' || key=='=') cur->setSpeed(cur->getSpeed()*1.1);
    if (key=='p' || key=='P') cur->setLoopState((cur->getLoopState()==OF_LOOP_NORMAL) ? OF_LOOP_PALINDROME : OF_LOOP_NORMAL);
    if (key=='r' || key=='R') cur->reverse();
    if (key==' ') cur->setPaused(!cur->isPaused());
    if (key=='f') ofToggleFullscreen();
    if (key=='s') shaderEnabled = !shaderEnabled;
    if (key=='h') showHelp = !showHelp;
    if (key=='t') showThumbs = !showThumbs;
    if (key>='1' && key<=sequence.size()+'0') {
      cur = sequence[key-'1'];
      fadeTimer = 0; //start/reset timer
      
      bg.begin();
      ofSetColor(255);
      fg.draw(0,0); //draw foreground into background
      bg.end();
    }
  }
  
  void mouseDragged(int x, int y, int button){
    ofPoint from(ofGetPreviousMouseX(), ofGetPreviousMouseY());
    ofPoint to(ofGetMouseX(), ofGetMouseY());
    
    from = toSphere(from / ofGetWidth() - 0.5f);
    to = toSphere(to / ofGetWidth() - 0.5f);
    ofPoint axis = from.getCrossed(to);
    
    qTo *= ofQuaternion(axis.x,axis.y,axis.z,from.dot(to));
  }
  
  ofPoint toSphere(ofPoint v) {  //-0.5 ... +0.5
    float mag = v.x*v.x + v.y*v.y;
    if (mag>1.0f) v.normalize();
    else v.z = sqrt(1.0f - mag);
    return v;
  }
  
  void windowResized(int w, int h) {
    cout << "resize: h=" << h << endl;
    bg.allocate(h,h);
    fg.allocate(h,h);
    lightPos.set(h/2,h/2,h);
  }

};

//========================================================================
int main() {
  ofSetupOpenGL(1024,768,OF_WINDOW);
  ofRunApp(new ofApp());
}
