#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

#define PORT 5555 //ポート番号

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    int     width; // 画面の幅
    int     height; // 画面の高さ
    int     fadeinCount; // フェードインするカウント
    int     showPicCount; // 撮った写真を表示する時間
    int     joneyCount; //ジョニーのカウント
    float   jumpAvarage; // ジャンプの時間の平均
    float   jumpSum; // ジャンプ時間の合計
    float   jumpCount; // ジャンプのカウント
    float   jumpLastTime; // 前のジャンプしたタイミング
    float   jumpBetween; // ジャンプ間の時間
    float   startShutter; // シャッターのタイミング
    bool    isStart; // スタート
    bool    isShowPic; // 撮った写真を表示するか
    bool    isCountSE[3];
    //background
    float   ratioSpeed; // speedにかける比率
    float   position; // 後ろの空の画像を動かすための座標
    float   speed; // それに伴う速度
    float   gravity; // 重力加速度
    bool    isGoal; //　ゴールしたかの判定
    //cam
    int       camWidth; // カメラのwidth
    int       camHeight; // カメラのheight
    ofTexture   nowImage; // 現在のカメラを保存するテクスチャ
    ofTexture   backgroundTex;
    ofTexture   resultChromaKey; // クロマキー合成した結果のテクスチャ
    ofTexture   resultComposition; // 背景と合成した結果のテクスチャ
    ofShader    chromaKey; // クロマキー合成するシャーだ
    ofShader    composition; // 背景合成のシェーダ
    ofFbo       fboChromaKey;
    ofFbo       fboComposition;
    ofVideoGrabber camera;
    
    
    // 画像
    ofImage     outImg; // 写真
    ofImage     countImg[3]; // カウントの画像
    ofImage     joney; // 宇宙飛行士の画像
    
    
    // 音
    ofSoundPlayer   flashSE;
    ofSoundPlayer   countSE[3];
    
    // osc
    ofxOscReceiver receiver;
};
