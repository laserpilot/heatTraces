//
//  RacerRibbon.h
//  lineRacer
//
//  Created by Blair Neal on 5/24/16.
//
//

#include  "ofMain.h"
#include "MathUtils.h"
#include "constants.h"
#pragma once


enum lineRacerModeType { NORMAL, DASHES, NORMAL_NOPARTICLES };

class RacerRibbon {

    
public:
    struct ribbonData{
        ofPoint pos;
        int ptNum;
    };
    
    void setup(int _length, int _id, int totalRibbons, int randomSeed, lineRacerModeType drawModeType = NORMAL);
    void addPoint(ofPoint _pt);
    void addPoint(float x, float y);
    void update();
    void draw();
    
    vector <ribbonData> getPoints();
    
    void setStartNoiseAmp( float _maxAmplitude);
    void setTailNoiseAmp( float _maxSecondAmp);
    void setStartNoiseAmt( float _startNoiseAmt);
    void setTailNoiseAmt(float _tailNoiseAmt);
    void setNoiseStep(float _noiseStep);
    void setRandomize(float _randomize);
    void setThickness(float thickness);
    void setTipSharpness(float tipSharpness);
    void setTipLength(int _tipLength);
    void setZNoiseAmp(float _zNoiseAmp);
    void setStartColor(ofColor _startColor);
    void setEndColor(ofColor _endColor);
    void setOverallAlpha(float _alpha);
    void reset();
    
private:
    
    void updateNoise();
    float getStartNoiseVal(int _ptPosInVec,float multiplier, int _ribbonID, float randomize);
    float getTailNoiseVal(int _ptPosInVec,float multiplier, int _ribbonID, float randomize);
    ofFloatColor ofColorToFloatColor(ofColor _color);
    
    void drawThickMesh();
    void drawLineMesh();
    void addParticle(ofPoint _pt);
    void updateParticles();
    void drawParticles();
    
    int randomSeed;
    int ribbonID; //just an int to pass in to distinguish it from others
    int length;
    float lineWidth;
    ofColor startColor;
    ofColor endColor;
    float overallAlpha;
    
    vector <ribbonData> pts;
    int ptNumCounter;
    ofPoint tipPt;
    float zNoiseAmp;
    
    float	*ribbonNoise;
    int		nRibbonNoise;
    float   ribbonNoiseCursor;
    float colorNoiseCursor;
    float currentRibbonNoiseVal;
    
    float startNoiseAmt;
    float tailNoiseAmt;
    float noiseStep;
    float maxStartNoiseAmp;
    float maxTailNoiseAmp;
    float randomize;
    float thickness;
    float thicknessRandom;
    float tipSharpness;
    int tipLength;
    int totalRibbons;
    bool tailNoiseFlavor;
    
    int maxNumParticles;
    float particleSize;
    int particleStepSize;
    float particleStartPercent;
    

    lineRacerModeType chosenType;

    
    //particles
    
    struct followPoint{
        ofPoint pos;
        ofPoint newPos;
        ofVec2f delta;
        ofVec2f accel;
        ofVec2f velocity;
        ofVec2f norm;
        ofColor color;
        
        float alpha;
        float spring;
        float damping;
        float duration;
        float noiseStep;
        float noiseStepAccum;
        float noiseStrength;
        float noiseScale;
        float percentPlayed;
        bool isPlaying;
        int time;
    };
    
    
    ofMesh meshParticle;

    vector<followPoint> particles;
    
    
    
    
    
};