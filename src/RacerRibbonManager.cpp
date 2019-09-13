//
//  RacerRibbonManager.cpp
//  lineRacer
//
//  Created by Blair Neal on 5/23/16.
//
//

#include "RacerRibbonManager.h"

void RacerRibbonManager::setup(int _numRibbons, int randomSeed, int _maxLength, lineRacerModeType _type){
    
    numRibbons = _numRibbons;
    
    ribbons.clear();
    
    for(int i=0; i<numRibbons; i++){
        RacerRibbon temp;
        int _length = 100;
        if(_type==DASHES){
            _length=ofRandom(50,_maxLength+20);
        }else{
            _length = ofRandom(50,_maxLength);
        }
        temp.setup(_length, i, numRibbons,randomSeed + (int)ofRandom(0,1000), _type);
        ribbons.push_back(temp);
    }
    
    ofLogNotice()<<"Setting up Line racer with: " << numRibbons << " ribbons"<<endl;
    alpha = 1.0;
    
    pickNewColors();
    
}

void RacerRibbonManager::addPoint(ofPoint _pt){
    for(int i=0; i<numRibbons; i++){
        ribbons[i].addPoint(_pt);
    }

}

void RacerRibbonManager::addPoint(float x, float y, float z){
    addPoint(ofPoint(x,y,z));
}

void RacerRibbonManager::update(){
    for(int i=0; i<numRibbons; i++){
        ribbons[i].update();
    }
}

void RacerRibbonManager::updateVals(float _startNoiseAmp, float _startNoiseAmt, float _tailNoiseAmp,float _tailNoiseAmt, float _randomize, float _thickness, float _tipSharpness, float _zNoiseAmp, int _tipSharpLength){
    for(int i=0; i<numRibbons; i++){
        ribbons[i].setRandomize(_randomize);
        ribbons[i].setStartNoiseAmt(_startNoiseAmt);
        ribbons[i].setTailNoiseAmt(_tailNoiseAmt);
        ribbons[i].setStartNoiseAmp(_startNoiseAmp);
        ribbons[i].setTailNoiseAmp( _tailNoiseAmp);
        ribbons[i].setThickness(_thickness);
        ribbons[i].setTipSharpness(_tipSharpness);
        ribbons[i].setZNoiseAmp(_zNoiseAmp);
        ribbons[i].setTipLength(_tipSharpLength);
    }
}

void RacerRibbonManager::setOverallAlpha(float _alpha){
    
    alpha = _alpha;
    for(int i=0; i<numRibbons; i++){
        ribbons[i].setOverallAlpha(alpha);
    }
}



void RacerRibbonManager::draw(){
    for(int i=0; i<numRibbons; i++){
        ribbons[i].draw();
    //ofDrawBitmapString("numpts: " + ofToString(ribbons[i].getPoints().size()), 300,200+20*i);
    }
    ofSetColor(255);
    //ofDrawLine(0, outputFbo.getHeight()/2, outputFbo.getWidth(), outputFbo.getHeight()/2);

}

void RacerRibbonManager::pickNewColors(){
    int chosenColor = (int)ofRandom(0,20)%5;
    
    ofColor startColor;
    ofColor endColor;
    
    if(chosenColor==0){
        startColor = neonGreen;
        endColor = neonPink;
    }else if(chosenColor==1){
        startColor = neonPink;
        endColor = neonGreen;
    }else if(chosenColor==2){
        startColor = neonOrange;
        endColor = neonGreen;
    }else if(chosenColor==3){
        startColor = ofColor(255);
        endColor = neonPink;
    }else if(chosenColor==4){
        startColor = neonGreen;
        endColor = ofColor(255);
    }else{
        startColor = neonGreen;
        endColor = neonPink;
    }
    
    for(int i=0; i<numRibbons; i++){
        ribbons[i].setEndColor(endColor);
        ribbons[i].setStartColor(startColor);
    }
}

void RacerRibbonManager::resetAll(){
    
    for(int i=0; i<numRibbons; i++){
        ribbons[i].reset();
    }
    
    pickNewColors();
    
}

