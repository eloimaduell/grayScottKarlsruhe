#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

public:
    struct Parameters
    {
        float    diffU;
        float    diffV;
        float    feed;
        float    kill;
        float    brushSize;
        float    timeMultiplier;
        ofVec2f  brush;
        ofFloatColor color1;
        ofFloatColor color2;
        ofFloatColor color3;
        ofFloatColor color4;
        ofFloatColor color5;
        
        Parameters()
          : diffU( 0.0f )
          , diffV( 0.0f )
          , feed( 0.0f )
          , kill( 0.0f )
          , brushSize( 0.0f )
          , brush( 0.0f )
          , color1( 0.0f )
          , color2( 0.0f )
          , color3( 0.0f )
          , color4( 0.0f )
          , color5( 0.0f )
          , timeMultiplier( 0.0f )
        {}
    };
    
    ofApp();
    
 	void setup();
	void update();
	void draw();
    
    void createFullScreenQuad();
    void setDefaultParameters();
    
    void onDiffUValueChanged( float& _value );
    void onDiffVValueChanged( float& _value );
    void onFeedValueChanged( float& _value );
    void onKillValueChanged( float& _value );
    void onBrushSizeValueChanged( float& _value );
    void onTimeValueChanged( float& _value );
    
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
    
    ofShader    m_grayscottShader;
    ofShader    m_screenShader;
    
    ofVboMesh   m_fsQuadVbo;
    
    ofFbo       m_fbos[2]; // ping pong fbos
    
    Parameters  m_parameters;
    
   	ofxFloatSlider    m_diffUSlider;
   	ofxFloatSlider    m_diffVSlider;
   	ofxFloatSlider    m_feedSlider;
   	ofxFloatSlider    m_killSlider;
   	ofxFloatSlider    m_brushSizeSlider;
   	ofxFloatSlider    m_timeSlider;
    
	ofxFloatColorSlider    m_color1Slider;
	ofxFloatColorSlider    m_color2Slider;
	ofxFloatColorSlider    m_color3Slider;
	ofxFloatColorSlider    m_color4Slider;
	ofxFloatColorSlider    m_color5Slider;
    
    ofParameter<float>     m_fps;
    
	ofxPanel          m_gui;
    
    float             m_lastTime;
    
    ofImage           m_starterImage;
    ofImage           m_obstacleImage;
};
