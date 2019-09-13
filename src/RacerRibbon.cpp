//
//  RacerRibbon.cpp
//  lineRacer
//
//  Created by Blair Neal on 5/24/16.
//
//

#include "RacerRibbon.h"

void RacerRibbon::setup(int _length, int _id, int _totalRibbons, int _randomSeed, lineRacerModeType drawModeType){
    particles.clear();
    meshParticle.clear();
    pts.clear();
    
    chosenType = drawModeType;
    
    randomSeed = _randomSeed;
    length = _length;
    if (length<=0) {
        length=150;
    }
    

    totalRibbons = _totalRibbons;
   // startColor = ofColor(206,255,8,255);
   // endColor = ofColor(255,10,129,0);
    startColor = neonPink;
    endColor = neonGreen;
    
    
    ribbonNoiseCursor = 0.01;
    colorNoiseCursor = 0.02;
    
    startNoiseAmt = 0.4;
    tailNoiseAmt = 0.4;
    noiseStep = 0.0005;//+ofRandom(-0.003,0.0003);
    randomize = 0;
    ribbonID = _id;
    thickness = 2;
    thicknessRandom = ofRandom(0.2,1);
    tipSharpness = 0;
    if (totalRibbons<80){
        maxNumParticles = 200;
    }else{
        maxNumParticles = ofRandom(50,150);
    }
    
    //Particles
    if(chosenType==NORMAL){
        meshParticle.setMode(OF_PRIMITIVE_POINTS);
        particleSize = ofRandom(0.5, 7);
        particleStartPercent = ofRandom(0.5,1.0);
        particleStepSize = ofRandom(15,30);
    }
    
    ptNumCounter = 0;
    overallAlpha = 1.0;
   
}

void RacerRibbon::addPoint(ofPoint _pt){
    tipPt = _pt;
    ribbonData tempData;
    tempData.pos = _pt;
    tempData.ptNum = ptNumCounter;
    
    pts.push_back(tempData);
    ptNumCounter++;
    
    if(pts.size()>length){
        pts.erase(pts.begin(), pts.begin()+1);
    }
    
}

void RacerRibbon::addPoint(float x, float y){
    addPoint(ofPoint( x,y));
}

void RacerRibbon::update(){
    
    updateNoise();

    int stepSize = particleStepSize +ofRandom(-10,10); //skip a bunch of particles to spread them out more..
    
    
    for(int i=0; i<pts.size(); i++){
        pts[i].pos.y = pts[i].pos.y + maxStartNoiseAmp*getStartNoiseVal(pts[i].ptNum, 0.02, ribbonID, randomize);
        
        if(zNoiseAmp>0){
            float fade = ofMap(i,0,pts.size(), 0,1);
            pts[i].pos.z = pts[i].pos.z + fade*zNoiseAmp*getStartNoiseVal(pts[i].ptNum, 0.01, ribbonID, randomize);
        }
        
        /*
        if(chosenType==NORMAL){
            if(i<pts.size()*particleStartPercent && i%stepSize==0){
                addParticle(pts[i].pos);
            }
        }*/
    }
    /*
    if(chosenType==NORMAL){
        updateParticles();
    }*/
}

void RacerRibbon::addParticle(ofPoint _pt){
    followPoint p;
    p.pos = _pt;
    p.noiseScale = 200;
    p.noiseStep = ofRandom(-1,1);
    p.noiseStepAccum = 0;
    p.noiseStrength = ofRandom(1, 5);
    p.duration = ofRandom(15, 200);
    p.alpha = 255;
    p.percentPlayed = 0;
    p.time = 0;
    p.damping = ofRandom(0.95, 1.0);
    p.isPlaying = true;
    p.color = startColor;
    float maxVel = 4;
    p.velocity = ofPoint(ofRandom(-maxVel, maxVel),ofRandom(-maxVel, maxVel),ofRandom(-maxVel, maxVel));
    particles.push_back(p);
    p.norm = ofPoint(1,0,0);
    p.accel = ofPoint(ofRandom(-0.1, 0.1), ofRandom(-0.1, 0.1));
    
    
    particles.push_back(p);
    
    //make sure the number of particles doesn't explode..
    if(particles.size()>maxNumParticles){
        particles.erase(particles.begin(), particles.begin()+(particles.size()-maxNumParticles));
    }
    
}

void RacerRibbon::updateParticles(){
    
    meshParticle.clear();
    for(int i = 0; i < particles.size(); i++){
        
        if(particles[i].time < particles[i].duration){
            
            //float angle = ofNoise(particles[i].pos.x/particles[i].noiseScale, particles[i].pos.y/particles[i].noiseScale) * particles[i].noiseStrength;
            
            //float angle1 = ofNoise(particles[i].pos.x/particles[i].noiseScale, particles[i].pos.y/particles[i].noiseScale) * particles[i].noiseStrength/2;
            
            ofVec2f bar = particles[i].pos;
            ofVec2f foo = ofPoint(1, 1, 1);
            
            //particles[i].noiseStepAccum= particles[i].noiseStepAccum+ particles[i].noiseStep;
            //particles[i].velocity = ofPoint(sin(angle1)*particles[i].noiseStep, cos(angle1)*particles[i].noiseStep);// * foo.getNormalized() + foo + foo.getPerpendicular() + foo.getRotated(90, ofVec2f(0, 1)+ foo.getRotated(-90, ofVec2f(0, 1)));
            
            float posInVec = ofMap(i,0,particles.size(), 0,1);
            particles[i].time++;
            
            float pctLife =(float)particles[i].time/particles[i].duration;

            particles[i].percentPlayed = pctLife;
            
            
            float stepFunc = MathUtils::smoothStepInOut(0, 0.5, 0.5, 1.0, pctLife);
            
            //particles[i].pos.y = particles[i].pos.y + maxStartNoiseAmp*getStartNoiseVal(i, 0.02, ribbonID, randomize);
            
            particles[i].velocity +=particles[i].accel;
            particles[i].pos += particles[i].velocity;
            //particles[i].velocity *= particles[i].damping;
            
            particles[i].alpha = 255 * stepFunc *overallAlpha;
            
            meshParticle.addVertex(particles[i].pos);
            ofColor tempColor = ofColor::white;
            /*
            tempColor = endColor;
            tempColor.lerp(startColor, posInVec);
            tempColor.setHueAngle(ofWrap(30*ofGetElapsedTimef()+ofMap(posInVec, 0,1,0,360), 0,360));
            */
            tempColor.a = particles[i].alpha;
            
            meshParticle.addColor(ofColor(tempColor));
            
        }else{
            particles.erase(particles.begin()+i);
        }
    }

}

void RacerRibbon::updateNoise(){
    
    ribbonNoiseCursor += noiseStep;
    colorNoiseCursor += noiseStep*4;
    //ribbonNoiseCursor = ofWrap(ribbonNoiseCursor,0,1);

}

float RacerRibbon::getStartNoiseVal(int _ptPosInVec,float multiplier, int _ribbonID, float randomize){
    
   float currentNoiseVal = startNoiseAmt * ofSignedNoise( ribbonNoiseCursor+_ribbonID*randomize+_ptPosInVec*multiplier + randomSeed*randomize);
    
    return currentNoiseVal;
    
}

float RacerRibbon::getTailNoiseVal(int _ptPosInVec,float multiplier, int _ribbonID, float randomize){
    
    float currentNoiseVal = tailNoiseAmt * ofSignedNoise( ribbonNoiseCursor+_ribbonID*randomize+_ptPosInVec*multiplier + randomSeed*randomize);
    
    return currentNoiseVal;
    
}

void RacerRibbon::draw(){
    
    ofLight light;
    ofLight light2;
    light.enable();
    light2.enable();
    ofEnableLighting();
    light.setPointLight();
    light2.setPointLight();
    light.setPosition(ofGetMouseX() , ofGetMouseY(), 0);
    light2.setPosition(pts.back().pos.x, pts.back().pos.y, pts.back().pos.z +200);
    
    if(chosenType == NORMAL){
        drawThickMesh();
        //drawParticles();
    }else if(chosenType ==DASHES){
        drawLineMesh();
    }
    

    ofDisableLighting();
 
}

void RacerRibbon::drawThickMesh(){
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

    ofVec3f     a, b, tangent, normal, mappedA, mappedB;
    

    if(ribbonID%3==0){
        tailNoiseFlavor = true;
    }else{
        tailNoiseFlavor = false;
    }


    for(int i=0; i<pts.size(); i++){

        //float stepFunc = MathUtils::smoothStep(0.9, 0.0, ofMap(i,0,pts.size(), 0,1));
        float stepFunc = ofMap(i,0,pts.size(), 1,0, true);
        int indexOffset;

        if(tailNoiseFlavor){
            indexOffset= i;
        }else{
            indexOffset = pts[i].ptNum;
        }
        
        
        a = pts[i].pos;
        b = pts[i+1].pos;
        
        float noiseOffset =stepFunc*maxTailNoiseAmp*getTailNoiseVal(indexOffset, 0.02, ribbonID, randomize);
        
        if(i>pts.size()-tipLength){
            
            float slide = ofMap(i,pts.size(), pts.size()-tipLength, 1,0);
            b.y = ofLerp(b.y, tipPt.y, tipSharpness*slide);
            b.z = ofLerp(b.z, tipPt.z, tipSharpness*slide);
            a.y = ofLerp(a.y, tipPt.y, tipSharpness*slide);
            a.z = ofLerp(a.z, tipPt.z, tipSharpness*slide);
            a.y = a.y +noiseOffset;
            b.y = b.y +noiseOffset;
        }else{
            a.y = a.y +noiseOffset;
            b.y = b.y +noiseOffset;
        }
        
 
        tangent = b-a;
        normal = tangent.getRotated(90, ofVec3f(0,0,1));
        normal.normalize();
        
        float fade = ofMap(i,0,pts.size(), 0,1);
        ofVec3f corner;
        float tempThickness = thickness*MathUtils::smoothStepInOut(0, 0.2, 0.8, 1.0, fade);
        //float tempThickness = thickness*MathUtils::smoothStep(0, 1.0, fade);
        
        //Top Corner
        corner.x = a.x + normal.x*tempThickness;
        corner.y = a.y + normal.y*tempThickness+ maxTailNoiseAmp*stepFunc*getTailNoiseVal(indexOffset, 0.06, ribbonID, randomize);
        corner.z = a.z;
        mesh.addVertex(corner);
        mesh.addNormal(corner.getPerpendicular(b));
        
        //TODO: swap start and end color for a different look every time
        ofColor tempColor;
        tempColor = endColor;
        tempColor.lerp(startColor, fade);
        //tempColor.setHueAngle(ofWrap(60*colorNoiseCursor+ofMap(fade, 0,1,0,360), 0,360));
        tempColor.setSaturation(ofMap(ribbonID, 0, totalRibbons, 0,255));
        //Fade the tail?
        tempColor.a = 255*MathUtils::smoothStep(0, 0.5, fade) * overallAlpha;
        //tempColor.lerp(ofColor(0,0,0,255),ofMap(ribbonID, 0, totalRibbons, 0,1));
        
        mesh.addColor(tempColor);
        
        //BottomColor
        normal = tangent.getRotated(-90,ofVec3f(0,0,1));
        normal.normalize();
        
        corner.x = a.x + normal.x*thickness;
        corner.y = a.y + normal.y*thickness+ maxTailNoiseAmp*stepFunc*getTailNoiseVal(indexOffset, 0.06, ribbonID, randomize);
        corner.z = a.z;
        mesh.addVertex(corner);
        mesh.addNormal(corner.getPerpendicular(b));
        tempColor.a = 0;
        mesh.addColor(tempColor);
        
    }
    mesh.draw();
}

void RacerRibbon::drawLineMesh(){
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_LINES);
    
    if(ribbonID%3==0){
        tailNoiseFlavor = true;
    }else{
        tailNoiseFlavor = false;
    }
    //dont make this tip as blunt..
    tipSharpness = tipSharpness/3;
    
    tipSharpness = ofClamp(tipSharpness, 0, 1);
    
    for(int i=0; i<pts.size(); i++){
        
        //float stepFunc = MathUtils::smoothStep(0.9, 0.0, ofMap(i,0,pts.size(), 0,1));
        float stepFunc = ofMap(i,0,pts.size(), 1,0, true);

        ofPoint temp;
        int indexOffset;
        
        if(tailNoiseFlavor){
            indexOffset= i;
        }else{
            indexOffset = pts[i].ptNum;
        }

        temp = pts[i].pos;

        float noiseOffset =stepFunc*maxTailNoiseAmp*getTailNoiseVal(indexOffset, 0.03, ribbonID, randomize);
        
        if(i>pts.size()-tipLength){
            
            float slide = ofMap(i,pts.size(), pts.size()-tipLength, 1,0);
            temp.y = ofLerp(temp.y, tipPt.y, tipSharpness*slide);
            temp.z = ofLerp(temp.z, tipPt.z, tipSharpness*slide);
            temp.y = temp.y + noiseOffset;
            temp.z = temp.z + zNoiseAmp*getStartNoiseVal(pts[i].ptNum, 0.01, ribbonID, randomize);
        }else{
            temp.y = temp.y + noiseOffset;
            temp.z = temp.z + zNoiseAmp*getStartNoiseVal(pts[i].ptNum, 0.01, ribbonID, randomize);
        }
        
       // temp.y = temp.y + 100*ofSignedNoise(colorNoiseCursor, pts[i].ptNum);

        
        ofColor tempColor;
        tempColor = ofColor(255,255,255,255*overallAlpha);
       
        mesh.addVertex(temp);
        mesh.addColor(tempColor);
        
    }
    ofSetLineWidth(thickness);
    mesh.draw();
}

void RacerRibbon::drawParticles(){
    
    glPointSize(particleSize);
    meshParticle.draw();

}

void RacerRibbon::reset(){
    pts.clear();
    particles.clear();
    ribbonNoiseCursor = 0;
    ptNumCounter =0;
    randomSeed = ofRandom(0,5000);
    
    /*ofColor tempColor;
    //float btwnColor = ofRandom(0,1);
    
    //tempColor = startColor;
    //startColor = endColor;
    //endColor = tempColor;*/
}

ofFloatColor RacerRibbon::ofColorToFloatColor(ofColor _color){
    ofFloatColor temp;
    temp.set((float)_color.r/255,(float) _color.g/255, (float)_color.b/255, (float) _color.a/255);
    
    return temp;
}

vector <RacerRibbon::ribbonData> RacerRibbon::getPoints(){
    return pts;
}

//SETTERS
void RacerRibbon::setStartNoiseAmp(float _maxAmp){
    maxStartNoiseAmp = _maxAmp;
}
void RacerRibbon::setTailNoiseAmp(float _maxAmp){
    maxTailNoiseAmp = _maxAmp;
}

void RacerRibbon::setStartNoiseAmt(float _startNoiseAmt){
    startNoiseAmt = _startNoiseAmt;
}
void RacerRibbon::setTailNoiseAmt(float _tailNoiseAmt){
    tailNoiseAmt = _tailNoiseAmt;
}

void RacerRibbon::setNoiseStep(float _noiseStep){
    noiseStep = _noiseStep; //probably best to leave this..
}
void RacerRibbon::setRandomize(float _randomize){
    //this is whether all the ribbons are largely uniform or more randomized
    randomize = _randomize;
}

void RacerRibbon::setThickness(float _thickness){
    thickness = _thickness;
    thickness *=thicknessRandom;
}

void RacerRibbon::setStartColor(ofColor _startColor){
    startColor = _startColor;
}

void RacerRibbon::setEndColor(ofColor _endColor){
    endColor = _endColor;
}

void RacerRibbon::setTipSharpness(float _tipSharpness){
    tipSharpness = _tipSharpness;
}
void RacerRibbon::setTipLength(int _tipLength){
    
    if(_tipLength>pts.size()-5){
        tipLength = pts.size()-5;

    }else{
        tipLength = _tipLength;
    }
   
}

void RacerRibbon::setZNoiseAmp(float _ZNoiseAmp){
    zNoiseAmp = _ZNoiseAmp;
}

void RacerRibbon::setOverallAlpha(float _alpha){
    overallAlpha = _alpha;
}


