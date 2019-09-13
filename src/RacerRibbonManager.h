//
//  RacerRibbonManager.h
//  lineRacer
//
//  Created by Blair Neal on 5/23/16.
//
//


#include "ofMain.h"
#include "RacerRibbon.h"


class RacerRibbonManager {

    
public:

    void setup(int numRibbons, int randomSeed,int maxLength, lineRacerModeType _type = NORMAL);
    void addPoint(ofPoint __pt);
    void addPoint(float x, float y, float z);
    
    void update();
    void updateVals(float _startNoiseAmp, float _startNoiseAmt, float _tailNoiseAmp,float _tailNoiseAmt, float randomize, float _thickness, float tipSharpness, float zNoiseAmp, int _tipSharpLength);
    
    void setOverallAlpha(float _alpha);
    
    void draw();
    void resetAll();
    void pickNewColors();
    
private:
    vector <RacerRibbon> ribbons;
    int numRibbons;
    int drawMode;
    float alpha; //0-1 - gets converted later...


};

