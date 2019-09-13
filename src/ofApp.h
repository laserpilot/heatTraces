#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "RacerRibbonManager.h"

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
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void videoPctChanged(float & videoPctVal);
    
    void debugView();
		
    
    bool liveCam;
    
    ofVideoPlayer vidPlayer;
    ofVideoGrabber heatCamera;
    ofxCvColorImage            colorImg;
    ofVec2f inputDims;
    
    ofxCvGrayscaleImage     grayImage;
    ofxCvGrayscaleImage     grayBg;
    ofxCvGrayscaleImage     grayDiff;
    
    ofxCvContourFinder     contourFinder;
    
    ofxPanel gui;
    ofxIntSlider threshold;
    ofxFloatSlider videoPct;
    ofxIntSlider smoothAmt;
    ofxIntSlider numBlobsToCheck;
     ofxFloatSlider travelSpeed;
         ofxFloatSlider drawScale;
    
    ofParameter <float> startNoiseAmt;
    ofParameter <float> tailNoiseAmt;
    ofParameter <float> startNoiseAmp;
    ofParameter <float> randomize;
    ofParameter <float> tailNoiseAmp;
    ofParameter <float> zNoiseAmp;
    ofParameter <bool> mouseFollow;
    ofParameter <bool> addBlend;
    ofParameter <float> thickness;
    
    
    vector < RacerRibbonManager > ribbons;
    
    float percentage;
    vector < vector< ofVec3f > > tracePts; //blob travel points
    vector< float > indices; // Segment where is each positions2 (float)
    
    vector <ofVec2f> firstPt;
    float numPositions;
    
    vector < vector <ofVec3f> > prevTracePts;
    float prevLength;

    ofPoint lerped;
    
    bool bShowDebug;
    vector < vector< float > > lengths;
    
    bool                bLearnBackground;
    vector < float > travelers;
    vector<ofPolyline> bigLines, smoothLines, resampledLines;
};
