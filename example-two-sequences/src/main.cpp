//By Rick Companje
//March 14, 2016

#include "ofMain.h"
#include "ofxThreadedImageSequencePlayer.h"

class ofApp : public ofBaseApp {
public:

  vector<ofxThreadedImageSequencePlayer*> sequence; //we need to store pointers here because due to the use of ofThread our class's copy constructor is deleted
  ofxThreadedImageSequencePlayer *cur;
  ofQuaternion qTo;
  
  void setup() {
    ofBackground(0);
    ofDisableArbTex();
    ofEnableDepthTest();
    
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
  }

  void update() {
    cur->update(); //only the current sequence is being updated
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
    if (!cur->isInitialized()) {
      ofDrawBitmapString("No image loaded...", 5, 15);
      return;
    }

    ofSetColor(255);
    ofFill();
    ofEnableDepthTest();
    ofSetupScreenOrtho(ofGetWidth(),ofGetHeight(),-1500,1500);
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    
    cur->getTexture().bind();
    applyRotation();
    ofDrawSphere(ofGetHeight()/2);
    ofDisableDepthTest();
    ofSetupScreen();
    
    //titles
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
    
    //thumbs
    for (int i=0; i<sequence.size(); i++) {
      ofPushMatrix();
      ofSetColor(255);
      ofNoFill();
      ofTranslate(i*(128+18)+10, ofGetHeight()-64-10);
      if (sequence[i]->isInitialized()) sequence[i]->draw(0,0, 128, 64);
      ofSetLineWidth(sequence[i]==cur ? 4 : 1);
      ofDrawRectangle(0,0, 128, 64);
      ofPopMatrix();
    }
  }
  
  void keyPressed(int key) {
    if (key=='-') cur->setSpeed(cur->getSpeed()/1.1);
    if (key=='+' || key=='=') cur->setSpeed(cur->getSpeed()*1.1);
    if (key=='p' || key=='P') cur->setLoopState((cur->getLoopState()==OF_LOOP_NORMAL) ? OF_LOOP_PALINDROME : OF_LOOP_NORMAL);
    if (key=='r' || key=='R') cur->reverse();
    if (key==' ') cur->setPaused(!cur->isPaused());
    if (key>='1' && key<=sequence.size()+'0') cur = sequence[key-'1'];
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

};

//========================================================================
int main() {
  ofSetupOpenGL(1024,768,OF_WINDOW);
  ofRunApp(new ofApp());
}
