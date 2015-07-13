#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "ofxSyphon.h"
#include "ofxOsc.h"

#define LOG(args...) fprintf( stderr , ## args)

#define LB_CHECK_GL_ERROR()                                               \
{                                                                         \
  GLenum err;                                                             \
  while ((err = glGetError()) != GL_NO_ERROR) {                           \
  LOG( "OpenGL Error: %u at %s : %u - %s()", err, __FILE__, __LINE__, __FUNCTION__ ); \
  };                                                                      \
}

/*
// TO DO
 
 * avoid texture wrap/repeat vertically and horizontally
 - save / load different presets : keys 1..0 shif keys 1..0
 - osc to params
 - syphon/movie as input texture
 
 */

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
    
    ///----------------------------------////

    bool        m_bDebugMode;
    float       m_lastTime;
    
    /// Custom functions
    void createFullScreenQuad();
    void setDefaultParameters();
    
    void onDiffUValueChanged( float& _value );
    void onDiffVValueChanged( float& _value );
    void onFeedValueChanged( float& _value );
    void onKillValueChanged( float& _value );
    void onBrushSizeValueChanged( float& _value );
    void onTimeValueChanged( float& _value );

    void drawStarterIntoFbo();
    
    void runSimulation();
    
    /// Shaders and quad mesh
    ofShader    m_grayscottShader;
    ofShader    m_screenShader;
    ofVboMesh   m_fsQuadVbo;
    
    /// FBOs
    ofFbo       m_fbos[2]; // ping pong fbos
    ofFbo       m_renderFbo; // final render fbo
    ofVec2f     m_renderSize;

    /// GUI
    ofxPanel          m_gui;
    bool              m_showGUI;
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

    /// Images
    ofImage           m_starterImage;
    ofImage           m_obstacleImage;
    
    /// Syphon
    bool                m_showSyphonObstacle;
    bool                m_useSyphonAsObstacle;
    ofxSyphonClient     m_syphonClientObstacles;
    ofFbo               m_syphonFboObstacles;

    bool                m_showSyphonStarter;
    bool                m_useSyphonAsStarter;
    ofxSyphonClient     m_syphonClientStarter;
    ofFbo               m_syphonFboStarter;


    
    /// OSC
    ofxOscReceiver      m_oscReceiver;
    void                updateOSC();
    
    
};
