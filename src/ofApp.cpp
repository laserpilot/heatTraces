#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    liveCam = false;
    
    inputDims.x = 320;
    inputDims.y = 240;
    
    if(!liveCam){
        //vidPlayer.load("/Users/laser/Dropbox/PROJECTS/HeatCamera/sped_up_heat.mp4");
        vidPlayer.load("/Users/laser/Dropbox/PROJECTS/HeatCamera/HandPrint_normalspeed2.mp4");
        vidPlayer.play();
        vidPlayer.setPosition(0.085);
    }else{
        heatCamera.setup(inputDims.x, inputDims.y);
    }
    
    
    colorImg.allocate(inputDims.x, inputDims.y);
    grayImage.allocate(inputDims.x, inputDims.y);
    grayBg.allocate(inputDims.x, inputDims.y);
    grayDiff.allocate(inputDims.x, inputDims.y);
    
    bLearnBackground = true;
    threshold = 80;
    
    videoPct.addListener(this, &ofApp::videoPctChanged);
    
    gui.setup();
    gui.add(threshold.setup("Threshold",56,0,255));
    gui.add(videoPct.setup("Video Pct",0.085,0,1));
    gui.add(smoothAmt.setup("SmoothAmt",4,0,30));
    gui.add(numBlobsToCheck.setup("Blobs to draw",7,0,10));
    gui.add(travelSpeed.setup("Travel Speed",2,-5, 5));
    gui.add(drawScale.setup("DrawScale",3,0.1, 5));
    gui.add(startNoiseAmp.set("StartNoiseAmp", 1, 1,200));
    gui.add(startNoiseAmt.set("StartNoiseAmt", 0.07, 0,2));
    gui.add(tailNoiseAmp.set("TailNoiseAmp", 116, 1,800));
    gui.add(tailNoiseAmt.set("TailNoiseAmt", 0.4, 0,2));
    gui.add(randomize.set("Randomize", 0.0005,0,0.002));
    gui.add(thickness.set("Thickness", 10,0.1,50));
    gui.add(zNoiseAmp.set("Z_Noise", 0,0,500));
    gui.add(mouseFollow.set("Mouse Follow", false));
    
    bShowDebug = true;
    
    numPositions = 9;
    //lengths.resize(numPositions);
    //tracePts.resize(numPositions);
      //  prevTracePts.resize(20);
    //prevTracePts.push_back(ofVec3f(0,0,0));
    
    //To Do: Record video of heat appearing
        //--See if you can reliably pick out the heat area that appears and start a process
        //Generate particles based on heat gradient area
        //Make Debug View and then special view
    
    ofSetFrameRate(60);
    //traveler = 0;
    
    for (int i=0; i<9; i++){
        RacerRibbonManager tempRibbon;
        tempRibbon.setup(ofRandom (5,15), (int)ofRandom(20,5000),ofRandom(100,500),NORMAL);
        
        ribbons.push_back(tempRibbon);
    }
    
    for (int i = 0; i<10; i++){
        vector < ofVec3f > tempPts;
        vector < float > tempLengths;
        
        for (int j = 0; j<20; j++){
            tempPts.push_back(ofVec3f (0,0,0));
            tempLengths.push_back(0);
            travelers.push_back(0);
        }
        prevTracePts.push_back(tempPts);
        tracePts.push_back(tempPts);
        lengths.push_back(tempLengths);
    }

}

//--------------------------------------------------------------
void ofApp::update(){
    
    bool bNewFrame = false;
   
    if(liveCam){
        heatCamera.update();
        bNewFrame = heatCamera.isFrameNew();
    }else{
        vidPlayer.update();
        bNewFrame = vidPlayer.isFrameNew();
    }

    
    if (bNewFrame){
        if(liveCam){
            colorImg.setFromPixels(heatCamera.getPixels());
        }else{
            colorImg.setFromPixels(vidPlayer.getPixels());
        }
        
        grayImage = colorImg;
        if (bLearnBackground == true){
            grayBg = grayImage;        // the = sign copys the pixels from grayImage into grayBg (operator overloading)
            bLearnBackground = false;
        }
        
        // take the abs value of the difference between background and incoming and then threshold:
        //grayDiff.absDiff(grayBg, grayImage);
        grayImage.threshold(threshold);
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayImage, 20, (340*240)/2, 10, false);    // find holes
        

        smoothLines.clear();

        firstPt.clear();
        for(int i=0; i<contourFinder.nBlobs;i++){
            if(i<numBlobsToCheck){ //lets not go crazy here
                ofPolyline cur;
                cur.addVertices(contourFinder.blobs[i].pts);
                cur.setClosed(true);
            
                smoothLines.push_back(cur.getSmoothed(smoothAmt));
            }
        }
       
        for(unsigned int i = 0; i < smoothLines.size(); i++) {
            float totalLength = smoothLines[i].getLengthAtIndex( smoothLines[i].size()-1 );
            
            
                int lengthDelta = totalLength-prevLength;
                travelers[i] = travelers[i] +travelSpeed + lengthDelta; //try adding traveler delta
                travelers[i] = fmod(travelers[i], totalLength+ lengthDelta);
                prevLength = totalLength;
            
                for( size_t k = 0; k < numPositions ; k++ ){
                        // Place each shapes3 at a random location
                        lengths[i][k] = totalLength / float( numPositions ) * k + travelers[i];
                    
                    if( lengths[i][k] < 0.f ){
                        lengths[i][k] += totalLength;
                    }
                    
                    if( lengths[i][k] >= totalLength ) {
                        lengths[i][k] -= totalLength;
                    }
                       // lengths[ k ] += 5.f;
                        tracePts[i][k] = smoothLines[i].getPointAtLength( lengths[i][k] );
                    }
                
        }
    }
    
    ofPoint newPt;
    newPt.set(0,0,0);
    
    if(mouseFollow){
        newPt.x = ofGetMouseX();
        newPt.y = ofGetMouseY();
        for (int i=0; i< ribbons.size(); i++){
            ribbons[i].addPoint(newPt);
        }
    }else{
        if(tracePts.size()>0){
            for (int j=0; j< ribbons.size(); j++){
                ofVec3f avgTracePt;
                
                for(int i=0; i<prevTracePts[j].size(); i++){
                    avgTracePt = avgTracePt+prevTracePts[j][i];
                }
                
                avgTracePt = avgTracePt/prevTracePts[j].size();
                lerped.set(avgTracePt);
                lerped = lerped.getInterpolated(avgTracePt, 0.5);

                ribbons[j].addPoint(avgTracePt.x*drawScale, avgTracePt.y*drawScale, avgTracePt.z);
   
                prevTracePts[j].push_back(tracePts[j].front());
                
                if(prevTracePts[j].size()>4){
                    prevTracePts[j].erase(prevTracePts[j].begin(), prevTracePts[j].begin()+1);
                }
            }
        }
    }
    
    for (int i=0; i< ribbons.size(); i++){
        ribbons[i].updateVals(startNoiseAmp, startNoiseAmt, tailNoiseAmp, tailNoiseAmt, randomize, thickness, 0, zNoiseAmp, 15);
        ribbons[i].update();
    }
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    if(bShowDebug){
        debugView();
    }
    
    
    for (int i=0; i< ribbons.size(); i++){
        ribbons[i].draw();
    }
    

    /*
    for(unsigned int i = 0; i < bigLines.size(); i++) {
        bigLines[i].draw();
    }*/
    ofSetColor(255,255,255);
    if(bShowDebug){
        for( int i = 0; i < numPositions; i++ )
        {
            if(tracePts.size()>0){
                ofSetColor( 255,255,ofMap(i,0,numPositions, 0,255) );
                ofDrawCircle( tracePts[ i ].front()*drawScale, 5.f );
                ofDrawBitmapStringHighlight(ofToString(i), tracePts[ i ].front()*drawScale);
            }
        }
        ofSetColor(255, 0, 255);
        ofDrawCircle( lerped , 5.f );
        glPushMatrix();
        //glTranslated(20, 20, 0);
        ofScale(drawScale, drawScale);
        ofSetColor(255,255,0);
        ofSetColor(255);
        for(unsigned int i = 0; i < smoothLines.size(); i++) {
            smoothLines[i].draw();
        }
            glPopMatrix();
    }
 
    


    
    /*
       ofSetColor(100,100,255);
    for(unsigned int i = 0; i < resampledLines.size(); i++) {
        resampledLines[i].draw();
    }*/
   
    
}

void ofApp::videoPctChanged(float &videoPctChanged){
    vidPlayer.setPosition(videoPct);
}

void ofApp::debugView(){
    
    
    
    // draw the incoming, the grayscale, the bg and the thresholded difference
    ofSetColor(255);
    colorImg.draw(20,20);
    grayImage.draw(360,20);
    grayBg.draw(20,280);
    grayDiff.draw(360,280);
    
    // then draw the contours:
    
    ofFill();
    ofSetHexColor(0x333333);
    ofDrawRectangle(800,20,320,240);
    ofSetHexColor(0xffffff);
    
    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].draw(800,20);
    }
    
    // finally, a report:
    ofSetHexColor(0xffffff);
    stringstream reportStr;
    reportStr << "bg subtraction and blob detection" << endl
    << "press ' ' to capture bg" << endl
    << "threshold " << threshold << " (press: +/-)" << endl
    << "num blobs found " << contourFinder.nBlobs << ", fps: " << ofGetFrameRate();
    ofDrawBitmapString(reportStr.str(), 20, 600);
    
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    switch (key){
        case ' ':
            bLearnBackground = true;
            break;
        case 'f':
            ofToggleFullscreen();
            break;
        case 'd':
            bShowDebug = !bShowDebug;
            break;
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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
