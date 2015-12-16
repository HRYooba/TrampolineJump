#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // OSCの初期設定
    receiver.setup(PORT);
    
    ofSetFrameRate(60);
    ofBackground(0);
    width       = ofGetWidth();
    height      = ofGetHeight();
    fadeinCount = 40;
    isStart     = false;
    isShowPic   = false;
    
    // 画像
    for ( int i=0; i<3; i++ ) {
        countImg[i].loadImage("image/count" + ofToString(i+1) + ".png");
    }
    joney.loadImage("image/joney2.png");
    
    // 音
    flashSE.loadSound("sound/flash.mp3");
    for ( int i=0; i<3; i++ ) {
        countSE[i].loadSound("sound/count" + ofToString(i+1) + ".mp3");
        countSE[i].setSpeed(2.0);
        countSE[i].setVolume(1.0);
        countSE[i].setLoop(false);
        isCountSE[i] = false;
    }
    
    //background
    position      = -12288;
    ratioSpeed    = 0.1;
    gravity       = 1.6;
    speed         = 0;
    isGoal        = false;
    
    //cam
    camWidth    = 640;
    camHeight   = 480;
    camera.setVerbose(true);
    /*ofSetLogLevel(OF_LOG_VERBOSE);
     camera.listDevices();*/
    camera.setDeviceID(0);
    camera.initGrabber(camWidth, camHeight);
    nowImage.allocate(camWidth, camHeight, GL_RGB);
    backgroundTex.allocate(width/2, height, GL_RGB);
    ofLoadImage(backgroundTex, "image/back.png");
    resultChromaKey.allocate(camWidth, camHeight, GL_RGB);
    resultComposition.allocate(camWidth, camHeight, GL_RGB);
    chromaKey.load("", "shader/Chroma_key.frag");
    composition.load("", "shader/Composition.frag");
    fboChromaKey.allocate(camWidth, camHeight);
    fboComposition.allocate(width/2, height);
    
    // 画面の比
    printf("\nwidth : %d\n", width);
    printf("height : %d\n", height);
}

//--------------------------------------------------------------
void ofApp::update(){
    // osc---------------------------------------------
    while ( receiver.hasWaitingMessages() )
    {
        //次のメッセージを取得
        ofxOscMessage m;
        receiver.getNextMessage( &m );
        
        // 登ったら
        if ( m.getAddress() == "/getOn" ) {
            printf("\n------on------\n");
            fadeinCount  = 40;
            isStart      = true;
            jumpCount    = 0;
            jumpLastTime = 0;
            jumpBetween  = 0;
            jumpSum      = 0;
            jumpAvarage  = 0;
            startShutter = 0;
            for ( int i=0; i<3; i++ ) {
                countSE[i].setPosition(0);
                isCountSE[i] = false;
            }
        }
        
        // 降りたら
        if ( m.getAddress() == "/getOff" ) {
            printf("\n------off------\n");
            fadeinCount  = 40;
            isStart      = false;
            jumpCount    = 0;
            jumpLastTime = 0;
            jumpBetween  = 0;
            jumpSum      = 0;
            jumpAvarage  = 0;
            startShutter = 0;
            for ( int i=0; i<3; i++ ) {
                countSE[i].setPosition(0);
                isCountSE[i] = false;
            }
        }
        
        // ジャンプしたら
        if ( m.getAddress() == "/jump" && !isShowPic ){
            printf("\n   JUMP\n");
            ratioSpeed = sqrt(-position / 1000000.0) - 1.0/((1.1-position/12288) * 50.0);
            speed = m.getArgAsInt32(0) * ratioSpeed;
            if ( speed >= 100 ) {
                speed = 100;
            }
            if ( isStart && position >= -ofGetHeight() - 3000 ) {
                if ( jumpLastTime == 0 ) {
                    jumpLastTime = ofGetElapsedTimef();
                } else {
                    jumpCount ++;
                    jumpBetween  = ofGetElapsedTimef() - jumpLastTime;
                    jumpLastTime = ofGetElapsedTimef();
                    jumpSum      += jumpBetween;
                    jumpAvarage  = jumpSum / jumpCount;
                    printf("Between : %f\n", jumpBetween);
                    printf("Average : %f\n", jumpAvarage);
                }
                if ( jumpCount == 4 && startShutter == 0 ) {
                    startShutter = ofGetElapsedTimef();
                }
            }
        }
    }
    // ------------------------------------------------
    
    // もしスタートしたらファードインする
    if ( isStart ) {
        if ( fadeinCount >= 255 ) {
            fadeinCount = 255;
        } else {
            fadeinCount += 10;
        }
    } else {
        jumpCount    = 0;
        jumpLastTime = 0;
        jumpBetween  = 0;
        jumpSum      = 0;
        jumpAvarage  = 0;
        startShutter = 0;
        for ( int i=0; i<3; i++ ) {
            countSE[i].setPosition(0);
            isCountSE[i] = false;
        }
    }
    
    //　撮った写真を表示してないなら
    if ( !isShowPic ) {
        //backgorund
        gravity =  -position / 10000.0;
        speed -= gravity;
        position += speed;
        if ( position <= -12288 ) {
            position = -12288;
        }
        if ( position >=  -ofGetHeight()) {
            speed    = 0;
            position = -ofGetHeight();
        }
        
        if ( position >= -ofGetHeight() - 3000 ) {
            isGoal   = true;
        }
        if ( isGoal ) {
            if ( position <= -ofGetHeight() - 3000) {
                position = -ofGetHeight() - 3000;
            }
        }
        
        // camera
        fboChromaKey.begin();
        chromaKey.begin();
        chromaKey.setUniformTexture("u_nowImage", nowImage, 0);
        ofRect(0, 0, camWidth, camHeight);
        chromaKey.end();
        fboChromaKey.end();
        resultChromaKey = fboChromaKey.getTextureReference();
        fboComposition.begin();
        composition.begin();
        composition.setUniformTexture("u_chromaKey", resultChromaKey, 0);
        composition.setUniformTexture("u_background", backgroundTex, 1);
        composition.setUniform1f("u_position", position);
        composition.setUniform2f("u_resolution", (float)width, (float)height);
        ofRect(0, 0, width/2, height);
        composition.end();
        fboComposition.end();
        resultComposition = fboComposition.getTextureReference();
        camera.update();
        if ( camera.isFrameNew() ) {
            unsigned char *pixels = camera.getPixels();
            nowImage.loadData(pixels, camWidth, camHeight, GL_RGB);
        }
    } else {
        position      = -12288;
        ratioSpeed    = 0.1;
        gravity       = 1.6;
        speed         = 0;
        isGoal        = false;
        jumpCount    = 0;
        jumpLastTime = 0;
        jumpBetween  = 0;
        jumpSum      = 0;
        jumpAvarage  = 0;
        startShutter = 0;
        for ( int i=0; i<3; i++ ) {
            countSE[i].setPosition(0);
            isCountSE[i] = false;
        }
    }
    
    // 写真を撮る
    if ( isGoal ) {
        if ( startShutter != 0 ) {
            float start = ofGetElapsedTimef() - startShutter;
            if ( start >= jumpAvarage/4.0 ) {
                flashSE.play();
                outImg.grabScreen((width - width/2)/2, 0, ofGetWidth()/2, ofGetHeight());
                string imgName =  ofToString(ofGetYear()) + "_" + ofToString(ofGetMonth()) + ":" + ofToString(ofGetDay()) + "_" + ofToString(ofGetHours()) + "_" + ofToString(ofGetMinutes()) + "_" + ofToString(ofGetSeconds()) + ".png";
                outImg.saveImage("/Users/oobahiroya/Google ドライブ/SaveImage/" + imgName, OF_IMAGE_QUALITY_BEST);
                printf("\n==========Shutter==========\n");
                isShowPic    = true;
                showPicCount = 0;
                
            }
        }
        joneyCount ++;
    } else {
        joneyCount = 0;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if ( !isShowPic ) {
        
        //　カメラ
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        ofPushMatrix();
        ofTranslate((width - width/2) / 2 + width/2, 0);
        ofScale(-1, 1);
        ofSetColor(255, fadeinCount);
        fboComposition.draw(0, 0);
        ofPopMatrix();
        
        ofPushMatrix();
        ofTranslate(width/2, height/2);
        ofScale(0.5, 0.5);
        
        // 宇宙飛行士
        if ( joneyCount > 0 && jumpCount < 4) {
            float x = 500 - sqrt(joneyCount) * 100;
            if ( x <= - 200) {
                x = -200;
            }
            ofSetColor(255, 255);
            joney.draw(x, -joney.height);
        }
        // カウントダウン
        ofSetColor(255, 255);
        if ( isGoal && jumpCount < 4) {
            if ( jumpCount == 1 ) {
                if ( !isCountSE[2] ) {
                    countSE[2].play();
                    isCountSE[2] = true;
                }
                countImg[2].draw(- countImg[2].width/2, -countImg[2].height/2);
            }
            if ( jumpCount == 2 ) {
                if ( !isCountSE[1] ) {
                    countSE[1].play();
                    isCountSE[1] = true;
                }
                countImg[1].draw(- countImg[1].width/2, -countImg[1].height/2);
            }
            if ( jumpCount == 3 ) {
                if ( !isCountSE[0] ) {
                    countSE[0].play();
                    isCountSE[0] = true;
                }
                countImg[0].draw(- countImg[0].width/2, -countImg[0].height/2);
            }
        }
        ofDisableBlendMode();
        ofPopMatrix();
    } else {
        // 撮った写真を表示
        if ( showPicCount < 180) {
            ofSetColor(0);
            ofRect(0, 0, width, height);
            showPicCount += 30;
        } else if (showPicCount >= 180 && showPicCount < 380) {
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
            ofSetColor(255, showPicCount*1.5 - 180);
            outImg.draw((width - width/2)/2, 0);
            ofDisableBlendMode();
            showPicCount ++;
        } else {
            isShowPic = false;
        }
    }
    // サイドを消してる
    ofSetColor(0);
    ofRect(0, 0, (width - width/2)/2, height);
    ofRect((width - width/2)*3/2, 0, width - (width - width/2)*3/2, height);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if ( key == ' ' ) {
        printf("\n   JUMP\n");
        isStart     = true;
        ratioSpeed = sqrt(-position / 1000000.0) - 1.0/((1.1-position/12288) * 50.0);
        speed = 500 * ratioSpeed;
        if ( speed >= 100 ) {
            speed = 100;
        }
        if ( isStart && position >= -ofGetHeight() - 3000 ) {
            if ( jumpLastTime == 0 ) {
                jumpLastTime = ofGetElapsedTimef();
            } else {
                jumpCount ++;
                jumpBetween  = ofGetElapsedTimef() - jumpLastTime;
                jumpLastTime = ofGetElapsedTimef();
                jumpSum      += jumpBetween;
                jumpAvarage  = jumpSum / jumpCount;
                printf("Between : %f\n", jumpBetween);
                printf("Average : %f\n", jumpAvarage);
            }
            if ( jumpCount == 4 && startShutter == 0 ) {
                startShutter = ofGetElapsedTimef();
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
