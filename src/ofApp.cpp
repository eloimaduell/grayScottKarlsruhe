#include "ofApp.h"

ofApp::ofApp()
  : m_lastTime( 0.0f )
  , m_bDebugMode( false )
{
}

//--------------------------------------------------------------
void ofApp::setup()
{
    LB_CHECK_GL_ERROR();
    
    // Use GL_TEXTURE_2D Textures (normalized texture coordinates 0..1)
    ofDisableArbTex();
    
    /// Shaders and quad mesh
    ///-------------------------
    // change the 4 to 2 or 1 (or 8) to adjust the scale
    int width = ofGetWindowWidth();
    int height = ofGetWindowHeight();

    setDefaultParameters();
    createFullScreenQuad();

    m_grayscottShader.load( "shaders/passthrough.vert", "shaders/grayscott.frag" );
    m_screenShader.load( "shaders/passthrough.vert", "shaders/screen.frag" );

    
    m_grayscottShader.begin();
    m_grayscottShader.setUniform1f( "screenWidth", width );
    m_grayscottShader.setUniform1f( "screenHeight", height );
    m_grayscottShader.end();
    
    m_screenShader.begin();
    m_screenShader.setUniform1f( "screenWidth", width );
    m_screenShader.setUniform1f( "screenHeight", height );
    m_screenShader.end();
  
    /// Shader FBOs
    ///-------------------------
    
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
    fboSettings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
    fboSettings.wrapModeVertical = GL_CLAMP_TO_EDGE;

    // create our FBOs
    m_fbos[ 0 ].allocate( fboSettings );
    m_fbos[ 1 ].allocate( fboSettings );
    
    // setup FBOs
    m_fbos[ 0 ].begin();
    ofClear( 255, 0, 0, 255 );
    m_fbos[ 0 ].end();
    
    m_fbos[ 1 ].begin();
    ofClear( 255, 0, 0, 255 );
    m_fbos[ 1 ].end();
    
    
    /// GUI
    ///-------------------------

    m_gui.setup();
    m_gui.add( m_fps.set("FPS",0.0,0.0,120.0));
    m_gui.add( m_diffUSlider.setup( "Diffusion U", m_parameters.diffU, 0.0f, 1.0f ) );
    m_gui.add( m_diffVSlider.setup( "Diffustion V", m_parameters.diffV, 0.0f, 1.0f ) );
    m_gui.add( m_feedSlider.setup( "Feed Rate", m_parameters.feed, 0.0f, 0.1f ) );
	m_gui.add( m_killSlider.setup( "Death Rate", m_parameters.kill, 0.0f, 0.073f ) );
    m_gui.add( m_brushSizeSlider.setup( "Brush Size", m_parameters.brushSize, 1.0f, 20.0f ) );
    m_gui.add( m_timeSlider.setup( "Time Multiplier", m_parameters.timeMultiplier, 0.0f, 60.0 ) );
    
    m_gui.add( m_color1Slider.setup( "Color 1", m_parameters.color1, ofFloatColor( 0.0f, 0.0f, 0.0f, 0.0f ), ofFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ) ) );
    m_gui.add( m_color2Slider.setup( "Color 2", m_parameters.color2, ofFloatColor( 0.0f, 0.0f, 0.0f, 0.0f ), ofFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ) ) );
    m_gui.add( m_color3Slider.setup( "Color 3", m_parameters.color3, ofFloatColor( 0.0f, 0.0f, 0.0f, 0.0f ), ofFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ) ) );
    m_gui.add( m_color4Slider.setup( "Color 4", m_parameters.color4, ofFloatColor( 0.0f, 0.0f, 0.0f, 0.0f ), ofFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ) ) );
    m_gui.add( m_color5Slider.setup( "Color 5", m_parameters.color5, ofFloatColor( 0.0f, 0.0f, 0.0f, 0.0f ), ofFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ) ) );
    
    m_gui.setSize(600, 600);
    m_gui.setWidthElements(600);
    m_gui.loadFromFile("settings.xml");
    m_showGUI = true;
    
    m_diffUSlider.addListener( this, &ofApp::onDiffUValueChanged );
    m_diffVSlider.addListener( this, &ofApp::onDiffVValueChanged );
    m_feedSlider.addListener( this, &ofApp::onFeedValueChanged );
    m_killSlider.addListener( this, &ofApp::onKillValueChanged );
    m_brushSizeSlider.addListener( this, &ofApp::onBrushSizeValueChanged );
    m_timeSlider.addListener( this, &ofApp::onTimeValueChanged );
    
    
    /// Images
    ///-------------------------
    m_obstacleImage.load( "K07inv.png" );
    m_starterImage.load( "K07_starter.png" );
    
    
    /// Syphon
    ///-------------------------
    m_showSyphonObstacle = true;
    m_useSyphonAsObstacle = true;
    m_syphonClientObstacles.setup();
    m_syphonClientObstacles.set("obstacles","Arena");

    m_showSyphonStarter = true;
    m_useSyphonAsStarter = false;
    m_syphonClientStarter.setup();
    m_syphonClientStarter.set("starter","Arena");

    /// Final Render FBO
    ///-------------------------
    ofFbo::Settings renderFboSettings;
    renderFboSettings.width = width;
    renderFboSettings.height = height;
    renderFboSettings.internalformat = GL_RGBA;
    renderFboSettings.numSamples = 1;
    renderFboSettings.useDepth = true;
    renderFboSettings.useStencil = false;
    renderFboSettings.textureTarget = GL_TEXTURE_2D;
    renderFboSettings.minFilter = GL_LINEAR;
    renderFboSettings.maxFilter = GL_LINEAR;
    renderFboSettings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
    renderFboSettings.wrapModeVertical = GL_CLAMP_TO_EDGE;
    
    m_renderFbo.allocate( width, height ); //renderFboSettings);
    
    
    ofFbo::Settings syphonFboSettings;
    syphonFboSettings.width = width;
    syphonFboSettings.height = height;
    syphonFboSettings.internalformat = GL_RGBA;
    syphonFboSettings.numSamples = 1;
    syphonFboSettings.useDepth = false;
    syphonFboSettings.useStencil = false;
    syphonFboSettings.textureTarget = GL_TEXTURE_2D;
    syphonFboSettings.minFilter = GL_LINEAR;
    syphonFboSettings.maxFilter = GL_LINEAR;
    syphonFboSettings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
    syphonFboSettings.wrapModeVertical = GL_CLAMP_TO_EDGE;

    m_syphonFboObstacles.allocate( fboSettings );
    m_syphonFboStarter.allocate( fboSettings );
    
    /// OSC Receiver
    ///-----------------
    m_oscReceiver.setup(12345);
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
    
    updateOSC();
  
    /// Draw Syphons into FBOs
    /////////////////////////

    if (m_useSyphonAsObstacle)
    {
        m_syphonFboObstacles.begin();
        {
            ofClear( 0, 0, 0, 255 );
            ofSetColor( 255, 255, 255, 255 );
            m_syphonClientObstacles.draw( 0, 0 );
        }
        m_syphonFboObstacles.end();
    }
    
    if (true)
    {
        // draw syphon into fbo
        m_syphonFboStarter.begin();
        {
            ofClear( 0, 0, 0, 255 );
            ofSetColor( 255, 255, 255, 255 );
            m_syphonClientStarter.draw( 0, 0 );
        }
        m_syphonFboStarter.end();
    }
}

void ofApp::runSimulation()
{
    /// Draw GrayScott
    ////////////////////
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
 
    ofDisableDepthTest();
    ofEnableAlphaBlending();

    float currTime = ofGetElapsedTimef();
    float dt = ( currTime - m_lastTime ) * m_parameters.timeMultiplier;
    
    if ( dt > 1.01f || dt <= 0.0f )
    {
        dt = 1.01f;
    }
    
    dt = m_parameters.timeMultiplier / 60.0f * 1.01f;
    
    m_lastTime = currTime;
    
    m_grayscottShader.begin();
        m_grayscottShader.setUniform1f( "diffU", m_parameters.diffU );
        m_grayscottShader.setUniform1f( "diffV", m_parameters.diffV );
        m_grayscottShader.setUniform1f( "feed", m_parameters.feed );
        m_grayscottShader.setUniform1f( "kill", m_parameters.kill );
        m_grayscottShader.setUniform1f( "delta", dt );
    
        if(true == m_useSyphonAsObstacle)
        {
            m_grayscottShader.setUniformTexture( "tInfluence", m_syphonFboObstacles.getTexture(),1 );
        }
        else
        {
            m_grayscottShader.setUniformTexture( "tInfluence", m_obstacleImage.getTexture(), 1 );
        }
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
}

//--------------------------------------------------------------
void ofApp::draw()
{
    // clear to green as grayScott runs in red and green channels
    ofClear( 0, 255, 0, 255 );
    ofDisableDepthTest();

    int numSimulations = 2;
    
    for ( int i = 0; i < numSimulations; ++i )
    {
        runSimulation();
    }
    
    /// Final Render
    ////////////////
    m_fbos[ 1 ].getTexture().bind( 3 );
    
    m_renderFbo.begin();
    {
        ofClear(255,0,0,255);

        if ( true == m_bDebugMode )
        {
            m_fbos[ 0 ].draw( 0, 0 );
        }
        else
        {
            m_screenShader.begin();
            m_screenShader.setUniform4f( "color1", m_parameters.color1.r, m_parameters.color1.g, m_parameters.color1.b, m_parameters.color1.a );
            m_screenShader.setUniform4f( "color2", m_parameters.color2.r, m_parameters.color2.g, m_parameters.color2.b, m_parameters.color2.a );
            m_screenShader.setUniform4f( "color3", m_parameters.color3.r, m_parameters.color3.g, m_parameters.color3.b, m_parameters.color3.a );
            m_screenShader.setUniform4f( "color4", m_parameters.color4.r, m_parameters.color4.g, m_parameters.color4.b, m_parameters.color4.a );
            m_screenShader.setUniform4f( "color5", m_parameters.color5.r, m_parameters.color5.g, m_parameters.color5.b, m_parameters.color5.a );
            m_screenShader.setUniform1i( "tSource", 3 );
            
            m_fsQuadVbo.draw();
            m_screenShader.end();
        }
    }
    m_renderFbo.end();
    
    glDisable( GL_CULL_FACE );

   
    /// Draw To Screen
    ////////////////////
    
    ofSetColor(ofColor::white);
    m_renderFbo.draw(0,0);
    m_renderFbo.draw(0,0,192,108);

    if(m_showGUI) m_gui.draw();
    
    /// Draw previews and debugs
    
    int previewWidth = 200;

    if(m_showSyphonObstacle)
    {
        m_syphonFboObstacles.draw(ofGetWidth()-previewWidth,0,previewWidth,previewWidth/1.7777);//,m_syphonClientObstacles.getWidth()/10.0,m_syphonClientObstacles.getHeight()/10.0);
    }

    if(m_showSyphonStarter)
    {
        m_syphonFboStarter.draw(ofGetWidth()-previewWidth,200,previewWidth,previewWidth/1.7777);//,m_syphonClientObstacles.getWidth()/10.0,m_syphonClientObstacles.getHeight()/10.0);
    }

    m_syphonClientObstacles.draw(ofGetWidth()-previewWidth,400,previewWidth,previewWidth/1.7777);
    m_syphonClientStarter.draw(ofGetWidth()-previewWidth,600,previewWidth,previewWidth/1.7777);
    
    if(ofGetFrameNum()%4==0) drawStarterIntoFbo();
}

//--------------------------------------------------------------
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
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

    if ( 'd' == key )
    {
        m_bDebugMode = !m_bDebugMode;
    }
    else if ( 'c' == key )
    {
        m_fbos[ 1 ].begin();
            ofClear( 200, 0, 0, 255 );
            ofSetColor( 0, 225, 0, 255 );
        
            ofDisableBlendMode();
        m_fbos[ 1 ].end();
    }
    else if ( ' ' == key )
    {
        drawStarterIntoFbo();
    }
    else if (key == 'h')
    {
        m_showGUI = !m_showGUI;
    }
    else if (key == 'O')
    {
        m_showSyphonObstacle = !m_showSyphonObstacle;
    }
    else if (key == 'S')
    {
        m_showSyphonStarter = !m_showSyphonStarter;
    }
    else if (key == 'o')
    {
        m_useSyphonAsObstacle = !m_useSyphonAsObstacle;
    }

    else if (key == 's')
    {
        m_useSyphonAsStarter = !m_useSyphonAsStarter;
    }

    
    
    // 0 save and load
    else if (key == '0')
    {
        m_gui.loadFromFile("settings.xml");
    }
    else if (key == '=')
    {
        m_gui.saveToFile("settings.xml");
    }
    // 1 save and load
    //------------------
    else if (key == '1')
    {
            m_gui.loadFromFile("settings1.xml");
    }
    else if (key == '!')
    {
        m_gui.saveToFile("settings1.xml");
    }

    // 2 save and load
    //------------------
    else if (key == '2')
    {
        m_gui.loadFromFile("settings2.xml");
    }
    else if (key == '"')
    {
        m_gui.saveToFile("settings2.xml");
    }

    // 3 save and load
    //------------------
    else if (key == '3')
    {
        m_gui.loadFromFile("settings3.xml");
    }
    else if (key == 'á')
    {
        m_gui.saveToFile("settings3.xml");
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
//--------------------------------------------------------------
void ofApp::onDiffUValueChanged( float& _value )
{
    m_parameters.diffU = _value;
}

//--------------------------------------------------------------
void ofApp::onDiffVValueChanged( float& _value )
{
    m_parameters.diffV = _value;
}

//--------------------------------------------------------------
void ofApp::onFeedValueChanged( float& _value )
{
    m_parameters.feed = _value;
}

//--------------------------------------------------------------
void ofApp::onKillValueChanged( float& _value )
{
    m_parameters.kill = _value;
}

//--------------------------------------------------------------
void ofApp::onBrushSizeValueChanged( float& _value )
{
    m_parameters.brushSize = _value;
}

//--------------------------------------------------------------
void ofApp::onTimeValueChanged( float& _value )
{
    m_parameters.timeMultiplier = _value;
}

//--------------------------------------------------------------
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
void ofApp::updateOSC()
{
    while(m_oscReceiver.hasWaitingMessages())
    {
        ofxOscMessage* m;
        m = new ofxOscMessage();
        m_oscReceiver.getNextMessage(m);
        
        string address = m->getAddress();
        cout << "Osc in :: " << address << endl;
        cout << m->getArgAsFloat(0) << endl;
        
        if(address=="/feed")
        {
            m_feedSlider = m->getArgAsFloat(0);
        }
        else if (address=="/death")
        {
            m_killSlider = m->getArgAsFloat(0);
        }
        else if (address=="/diffU")
        {
            m_diffUSlider = m->getArgAsFloat(0);
        }
        else if (address=="/diffV")
        {
            m_diffVSlider = m->getArgAsFloat(0);
        }
        
    }
}

//--------------------------------------------------------------
void ofApp::drawStarterIntoFbo()
{
    m_fbos[ 1 ].begin();
    //ofClear( 200, 0, 0, 255 );
    ofSetColor( 0, 225, 0, 255 );
    
    ofEnableBlendMode( OF_BLENDMODE_ADD );
    m_syphonFboStarter.draw( 0, 0, 1920, 1080 );
    //m_starterImage.draw(0,0,1920,1080);
    
    ofDisableBlendMode();
    m_fbos[ 1 ].end();
    
    //cout << "drawing starter into FBO" << ofGetElapsedTimef()<< endl;

}