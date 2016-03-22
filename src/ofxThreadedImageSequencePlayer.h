//By Rick Companje
//March 14, 2016

#pragma once
#include "ofxThreadedImageReloader.h"

class ofxThreadedImageSequencePlayer : public ofBaseVideoPlayer, public ofBaseVideoDraws {
public:
  
  ofxThreadedImageReloader loader;
  vector<ofFile> files;
  vector<ofTexture> tex; //texture planes. not really used but needed as return value in for example getTexturePlanes
  
  //------------------------------------------------------------------------------------------------------------
  static bool natural(const ofFile& a, const ofFile& b) {
    string aname = a.getBaseName(), bname = b.getBaseName();
    
    size_t adigitpos = aname.find_last_not_of("0123456789");
    size_t bdigitpos = bname.find_last_not_of("0123456789");
  
    string astringpart = aname.substr(0,adigitpos+1);
    string bstringpart = bname.substr(0,bdigitpos+1);
    
    if (astringpart == bstringpart) {
      string anumpart = aname.substr(adigitpos+1);
      string bnumpart = bname.substr(bdigitpos+1);
      
      int aint = ofToInt(anumpart);
      int bint = ofToInt(bnumpart);
      
      if (ofToString(aint) == anumpart && ofToString(bint) == bnumpart) {
        return aint < bint; //integer compare
      }
    }
    
    //in all other cases string compare
    return a < b;
  }
  
  ofxThreadedImageSequencePlayer() {
    _isPlaying = false;
    _curFrame = 0;
    _speed = 1.0; //speed to frameRate factor
    _fps = 20; //frames per second when speed is 1
    _playDir = 1;
    _loopState = OF_LOOP_NORMAL;
  }
  
  virtual bool load(string name) { //folder or filename
    ofFile file(name);
    
    if (file.isDirectory()) {
      files = ofDirectory(name).getFiles();
    } else {
      ofDirectory folder(file.getEnclosingDirectory());
      files = folder.getFiles();
    }
    
    ofSort(files, natural); //improved natural sort
    
    //ofLogNotice() << "ofxThreadedImageSequencePlayer::load: " << files.size() << " files";
    return true;
  };
  
  virtual void loadAsync(string name) {
    load(name);
  }
  
  virtual void play() {
    _isPlaying = true;
  }
  
  virtual void stop() {
    setPaused(true);
  }
  
  virtual float getWidth() const {
    return loader.img.getWidth(); //fixme: check if img.isAllocated()?
  }
  
  virtual float getHeight() const {
    return loader.img.getHeight(); //fixme: check if img.isAllocated()?
  }
  
  virtual bool isPaused() const {
    return !_isPlaying;
  }
  
  virtual bool isLoaded() const {
    return loader.img.isAllocated();
  }
  
  virtual bool isPlaying() const {
    return _isPlaying;
  }
  
  virtual bool isInitialized() const {
    return isLoaded();
  }
  
  virtual float getPosition() const {
    return _curFrame / getTotalNumFrames();
  }
  
  virtual float getSpeed() const {
    return _speed;
  }
  
  virtual float getDuration() const {
    return getTotalNumFrames() / getFrameRate();
  }
  
  virtual bool getIsMovieDone() const {
    //?
    return false;
  }
  
  virtual void setPaused(bool bPause) {
    _isPlaying = !bPause;
  }
  
  virtual void setPosition(float pct) {
    setFrame(pct * getTotalNumFrames());
  }
  
  void setProgress(float pct) {
    setPosition(pct);
  }
  
  virtual void setVolume(float volume) { // 0..1
    //not implemented;
  }
  
  virtual void setLoopState(ofLoopType state) {
    _loopState = state;
  }
  
  virtual void setSpeed(float speed) {
    if (speed < 0) {
      _playDir = -1;
      speed = -speed;
    }
    
    _speed = ofClamp(speed,0,10000);
  }
  
  virtual void setFrame(int frame) { // frame 0 = first frame...
    _curFrame = ofClamp(frame,0,files.size()-1);
  }
  
  virtual int getCurrentFrame() const {
    return (int)_curFrame;
  }
  
  virtual int getTotalNumFrames() const {
    return files.size();
  }
  
  virtual ofLoopType getLoopState() const {
    return _loopState;
  }
  
  virtual void firstFrame() {
    setFrame(0);
  }
  
  virtual void nextFrame() {
    setFrame(getCurrentFrame()+1);
  }
  
  virtual void previousFrame() {
    setFrame(getCurrentFrame()-1);
  }
  
  //ofBaseVideo
  virtual bool isFrameNew() const {
    return true;
  }
  
  virtual void close() {
    if (_isPlaying) stop();
  }
  
  virtual bool setPixelFormat(ofPixelFormat pixelFormat) {
    //?
    return true;
  }
  
  virtual ofPixelFormat getPixelFormat() const {
    //?
    return OF_PIXELS_RGB; //fixme
  }
  
  //ofBaseUpdates
  virtual void update() {
    if (getTotalNumFrames()==0) return;
    
    if (_isPlaying) {
      float movieAppFrameRatio = _fps / (ofGetFrameRate() > 0 ? ofGetFrameRate() : 1);
      _curFrame += _playDir * _speed * movieAppFrameRatio;
    }
    
    if (_loopState==OF_LOOP_NONE) {
      if (_curFrame >= getTotalNumFrames()) {
        _curFrame = getTotalNumFrames()-1;
      } else if (_curFrame < 0) {
        _curFrame = 0;
      }
    } else if (_loopState==OF_LOOP_NORMAL) {
      while (_curFrame >= getTotalNumFrames()) {
        _curFrame -= getTotalNumFrames();
      }
      while (_curFrame < 0) {
        _curFrame += getTotalNumFrames();
      }
    } else if (_loopState==OF_LOOP_PALINDROME) {
      if (_curFrame >= getTotalNumFrames()) {
        _curFrame = getTotalNumFrames()-1;
        _playDir = -1;
      } else if (_curFrame < 0) {
        _curFrame = 0;
        _playDir = 1;
      }
    }
      
    //cout << "mov update: " << _curFrame << " speed: " << _speed << " fps: " << _fps << " _isPlaying: " << _isPlaying << endl;
    //cout << files[frame].getAbsolutePath() << " _curFrame:" << _curFrame << endl;
    
    loader.load(files[(int)_curFrame].getAbsolutePath());
    loader.update();
  }
  
  //ofBaseDraws
  virtual void draw(float x, float y) const {
    loader.img.draw(x,y);
  }

  virtual void draw(float x, float y, float w, float h) const {
    loader.img.draw(x,y,w,h);
  }
  
  //ofBaseHasTexturePlanes
  virtual vector<ofTexture> & getTexturePlanes() {
    //?
    return tex;
  }
  
  virtual const vector<ofTexture> & getTexturePlanes() const {
    //?
    return tex;
  }
  
  //ofBaseHasTexture {
  virtual ofTexture & getTexture() {
    return loader.img.getTexture();
  }
  
  virtual const ofTexture & getTexture() const {
    return loader.img.getTexture();
  }
  
  virtual void setUseTexture(bool bUseTex) {
    //? loader.img.setUseTexture(bUseTex);
  }
  
  virtual bool isUsingTexture() const {
    return loader.img.isUsingTexture();
  }
  
  //ofBaseHasPixels
  virtual ofPixels & getPixels() {
    return loader.img.getPixels();
  }
  
  virtual const ofPixels & getPixels() const {
    return loader.img.getPixels();
  }
  
  //non-virtual methods (not inheritted from any base class)
  
  void setFrameRate(float fps) {
    _fps = ofClamp(fps,1,10000);
  }
  
  float getFrameRate() const {
    return _fps;
  }
  
  void reverse() {
    _playDir *= -1;
  }
  
protected:
  
  bool _isPlaying;
  float _curFrame; //float to allow for speeds lower than 1
  float _fps;
  float _speed;
  int _playDir;
  ofLoopType _loopState;

};

