#include "ofApp.h"

ofApp::ofApp()
  : m_lastTime( 0.0f )
{
}

//--------------------------------------------------------------
void ofApp::setup()
{
    // Use GL_TEXTURE_2D Textures (normalized texture coordinates 0..1)
    ofDisableArbTex();
    
    m_grayscottShader.load( "shaders/passthrough.vert", "shaders/grayscott.frag" );
    m_screenShader.load( "shaders/passthrough.vert", "shaders/screen.frag" );
    
    setDefaultParameters();
    createFullScreenQuad();
    
    // change the 4 to 2 or 1 (or 8) to adjust the scale
    int width = ofGetWindowWidth();
    int height = ofGetWindowHeight();
    
    m_grayscottShader.begin();
    m_grayscottShader.setUniform1f( "screenWidth", width );
    m_grayscottShader.setUniform1f( "screenHeight", height );
    m_grayscottShader.end();
    
    m_screenShader.begin();
    m_screenShader.setUniform1f( "screenWidth", width );
    m_screenShader.setUniform1f( "screenHeight", height );
    m_screenShader.end();
  
    
    ofFbo::Settings fboSettings;
    fboSettings.width = width;
    fboSettings.height = height;
    fboSettings.internalformat = GL_RGB32F;
    fboSettings.numSamples = 4;
    fboSettings.useDepth = false;
    fboSettings.useStencil = false;
    fboSettings.textureTarget = GL_TEXTURE_2D;
    fboSettings.minFilter = GL_LINEAR;
    fboSettings.maxFilter = GL_LINEAR;
    fboSettings.wrapModeHorizontal = GL_REPEAT;
    fboSettings.wrapModeVertical = GL_REPEAT;

    // create our FBOs
    m_fbos[ 0 ].allocate( fboSettings );
    m_fbos[ 1 ].allocate( fboSettings );
    
    
    // init UI
    m_gui.setup();
    m_gui.add( m_fps.set("FPS",0.0,0.0,120.0));
    m_gui.add( m_diffUSlider.setup( "Diffusion U", m_parameters.diffU, 0.0f, 1.0f ) );
    m_gui.add( m_diffVSlider.setup( "Diffustion V", m_parameters.diffV, 0.0f, 1.0f ) );
    m_gui.add( m_feedSlider.setup( "Feed Rate", m_parameters.feed, 0.0f, 0.1f ) );
	m_gui.add( m_killSlider.setup( "Death Rate", m_parameters.kill, 0.0f, 0.073f ) );
    m_gui.add( m_brushSizeSlider.setup( "Brush Size", m_parameters.brushSize, 1.0f, 20.0f ) );
    m_gui.add( m_timeSlider.setup( "Time Multiplier", m_parameters.timeMultiplier, 0.0f, 500.0f ) );
    
    m_gui.add( m_color1Slider.setup( "Color 1", m_parameters.color1, ofFloatColor( 0.0f, 0.0f, 0.0f, 0.0f ), ofFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ) ) );
    m_gui.add( m_color2Slider.setup( "Color 2", m_parameters.color2, ofFloatColor( 0.0f, 0.0f, 0.0f, 0.0f ), ofFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ) ) );
    m_gui.add( m_color3Slider.setup( "Color 3", m_parameters.color3, ofFloatColor( 0.0f, 0.0f, 0.0f, 0.0f ), ofFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ) ) );
    m_gui.add( m_color4Slider.setup( "Color 4", m_parameters.color4, ofFloatColor( 0.0f, 0.0f, 0.0f, 0.0f ), ofFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ) ) );
    m_gui.add( m_color5Slider.setup( "Color 5", m_parameters.color5, ofFloatColor( 0.0f, 0.0f, 0.0f, 0.0f ), ofFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ) ) );
    
    m_diffUSlider.addListener( this, &ofApp::onDiffUValueChanged );
    m_diffVSlider.addListener( this, &ofApp::onDiffVValueChanged );
    m_feedSlider.addListener( this, &ofApp::onFeedValueChanged );
    m_killSlider.addListener( this, &ofApp::onKillValueChanged );
    m_brushSizeSlider.addListener( this, &ofApp::onBrushSizeValueChanged );
    m_timeSlider.addListener( this, &ofApp::onTimeValueChanged );
    
    m_obstacleImage.load( "starter.jpg" );
    m_starterImage.load( "testSimple_16bits.png" );
    
    
    m_fbos[ 0 ].begin();
        ofClear( 255, 0, 0, 255 );
    m_fbos[ 0 ].end();
    
    m_fbos[ 1 ].begin();
        ofClear( 255, 0, 0, 255 );
    m_fbos[ 1 ].end();
}

void ofApp::onDiffUValueChanged( float& _value )
{
    m_parameters.diffU = _value;
}

void ofApp::onDiffVValueChanged( float& _value )
{
    m_parameters.diffV = _value;
}

void ofApp::onFeedValueChanged( float& _value )
{
    m_parameters.feed = _value;
}

void ofApp::onKillValueChanged( float& _value )
{
    m_parameters.kill = _value;
}

void ofApp::onBrushSizeValueChanged( float& _value )
{
    m_parameters.brushSize = _value;
}

void ofApp::onTimeValueChanged( float& _value )
{
    m_parameters.timeMultiplier = _value;
}

void ofApp::createFullScreenQuad()
{
    // -1.0 to +1.0 is the full viewport (screen) if you use these as vertices in your vertex shader
    // (without multiplying by model, view, and projection matrices)

    ofVec3f vertices[4] =
    {
        ofVec3f(  1.0f,  1.0f, 0.0f ),
        ofVec3f( -1.0f,  1.0f, 0.0f ),
        ofVec3f(  1.0f, -1.0f, 0.0f ),
        ofVec3f( -1.0f, -1.0f, 0.0f )
    };
    
    ofIndexType indices[6] =
    {
        0, 1, 2,
        2, 1, 3
    };
    
    // Texture coordinates vary from 0.0 to 1.0 when they are in normalized format
    // ofDisableArbTex() was called earlier set that we're using normalized texture coordinates
    ofVec2f texCoords[4] =
    {
        ofVec2f( 1.0f, 1.0f ),
        ofVec2f( 0.0f, 1.0f ),
        ofVec2f( 1.0f, 0.0f ),
        ofVec2f( 0.0f, 0.0f )
    };
    
    m_fsQuadVbo.addVertices( vertices, 4 );
    m_fsQuadVbo.addTexCoords( texCoords, 4 );
    m_fsQuadVbo.addIndices( indices, 6 );
}

//--------------------------------------------------------------
void ofApp::update()
{
    m_parameters.color1 = m_color1Slider;
    m_parameters.color2 = m_color2Slider;
    m_parameters.color3 = m_color3Slider;
    m_parameters.color4 = m_color4Slider;
    m_parameters.color5 = m_color5Slider;
    
    m_fps = ofGetFrameRate();
 }

//--------------------------------------------------------------
void ofApp::draw()
{
    ofClear( 0, 255, 0, 255 );
    
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
 
    ofDisableDepthTest();
    ofEnableAlphaBlending();

    float currTime = ofGetElapsedTimef();
    float dt = ( currTime - m_lastTime ) * m_parameters.timeMultiplier;
    
    if ( dt > 0.85f || dt <= 0.0f )
    {
        dt = 0.85f;
    }
    
    m_lastTime = currTime;
    
    m_grayscottShader.begin();
        m_grayscottShader.setUniform1f( "diffU", m_parameters.diffU );
        m_grayscottShader.setUniform1f( "diffV", m_parameters.diffV );
        m_grayscottShader.setUniform1f( "feed", m_parameters.feed );
        m_grayscottShader.setUniform1f( "kill", m_parameters.kill );
        m_grayscottShader.setUniform1f( "delta", dt );
    
        m_grayscottShader.setUniformTexture( "tInfluence", m_obstacleImage.getTexture(), 1 );
    
        int fboIndex = 0;
        for ( int i = 0; i < 8; ++ i )
        {
            fboIndex = i % 2;
            m_grayscottShader.setUniformTexture( "tSource", m_fbos[ 1 - fboIndex ].getTexture(), 0 );
            
            m_fbos[ fboIndex ].begin();
                ofClear( 255, 0, 0, 255 );
                m_fsQuadVbo.draw();
            m_fbos[ fboIndex ].end();
        }
    m_grayscottShader.end();
    
    glDisable( GL_CULL_FACE );
    
//    m_fbos[ 0 ].draw( 0, 0 );
    
    m_screenShader.begin();
        m_screenShader.setUniform4f( "color1", m_parameters.color1.r, m_parameters.color1.g, m_parameters.color1.b, m_parameters.color1.a );
        m_screenShader.setUniform4f( "color2", m_parameters.color2.r, m_parameters.color2.g, m_parameters.color2.b, m_parameters.color2.a );
        m_screenShader.setUniform4f( "color3", m_parameters.color3.r, m_parameters.color3.g, m_parameters.color3.b, m_parameters.color3.a );
        m_screenShader.setUniform4f( "color4", m_parameters.color4.r, m_parameters.color4.g, m_parameters.color4.b, m_parameters.color4.a );
        m_screenShader.setUniform4f( "color5", m_parameters.color5.r, m_parameters.color5.g, m_parameters.color5.b, m_parameters.color5.a );
    
        m_screenShader.setUniformTexture( "tSource", m_fbos[ fboIndex ].getTexture(), 0 );
    
        m_fsQuadVbo.draw();
    m_screenShader.end();
    
    m_gui.draw();
}

void ofApp::setDefaultParameters()
{
    m_parameters.diffU = 0.2097f;
    m_parameters.diffV = 0.1050f;
    m_parameters.feed = 0.037f;
    m_parameters.kill = 0.06f;
    m_parameters.timeMultiplier = 20.0f;
    m_parameters.brushSize = 5.0f;
    m_parameters.brush.set( -10.0f, -10.0f );
    m_parameters.color1.set( 0, 0, 0, 0 );
    m_parameters.color2.set( 0, 1, 0, 0.2 );
    m_parameters.color3.set( 1, 1, 0, 0.21 );
    m_parameters.color4.set( 1, 0, 0, 0.4 );
    m_parameters.color5.set( 1, 1, 1, 0.6 );
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    if ( 'c' == key )
    {
        m_fbos[ 1 ].begin();
            ofClear( 200, 0, 0, 255 );
            ofSetColor( 0, 225, 0, 255 );
        
            ofDisableBlendMode();
        m_fbos[ 1 ].end();
    }
    else if ( ' ' == key )
    {
        m_fbos[ 1 ].begin();
            ofClear( 200, 0, 0, 255 );
            ofSetColor( 0, 225, 0, 255 );
        
            ofEnableBlendMode( OF_BLENDMODE_ADD );
            m_starterImage.draw( 0, 0, 1920, 1080 );
        
            ofDisableBlendMode();
        m_fbos[ 1 ].end();
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
    m_parameters.brush.set( x, y );
    
    m_fbos[ 1 ].begin();
    
        if ( 0 == button )
        {
            ofSetColor( 0, 255, 0, 255 );
        }
        else if ( 2 == button )
        {
            ofSetColor( 255, 0, 0, 255 );
        }
    
        ofDrawCircle( m_parameters.brush.x, m_parameters.brush.y, m_parameters.brushSize );
    m_fbos[ 1 ].end();
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
    m_parameters.brush.set( x, y );
    
    m_fbos[ 1 ].begin();
    
        if ( 0 == button )
        {
            ofSetColor( 0, 255, 0, 255 );
        }
        else if ( 2 == button )
        {
            ofSetColor( 255, 0, 0, 255 );
        }
    
        ofDrawCircle( m_parameters.brush.x, m_parameters.brush.y, m_parameters.brushSize );
    m_fbos[ 1 ].end();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}
