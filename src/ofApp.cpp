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
    gui.add(timeSmoothAmt.setup("Time Smooth Amt",4,0,15));
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
    fireOnce = true;
    
    numPositions = 9; //i dont expect more than 9 blobs
    
    ofSetFrameRate(60);
    //traveler = 0;
    
    for (int i=0; i<numPositions; i++){
        RacerRibbonManager tempRibbon;
        tempRibbon.setup(ofRandom (5,15), (int)ofRandom(20,5000),ofRandom(100,500),NORMAL);
        
        ribbons.push_back(tempRibbon);
    }
    
    
    //set up the various vectors/fill them with some data
    for (int i = 0; i<10; i++){
        vector < ofVec3f > tempPts;
        vector < float > tempLengths;
        
        for (int j = 0; j<20; j++){
            tempPts.push_back(ofVec3f (0,0,0));
            tempLengths.push_back(0);
            travelers.push_back(0);
            centroids.push_back(ofVec3f (0,0,0));
        }
        prevTracePts.push_back(tempPts);
        tracePts.push_back(tempPts);
        lengths.push_back(tempLengths);
    }

}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    if(fireOnce && ofGetFrameNum()>15){ //seems to not fire if the video isnt loaded yet...need to wait a few frames
        fireOnce = false;
        vidPlayer.setPosition(0.065); //move the video to a better position at the beginning to save yourself a step with my default vid
        vidPlayer.update();
    }
    
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
        if (bLearnBackground == true){ //im not using background subtraction currently
            grayBg = grayImage;
            bLearnBackground = false;
        }
        grayImage.threshold(threshold);
        contourFinder.findContours(grayImage, 20, (inputDims.x*inputDims.y)/2, 10, false);

        
        //just turn the contour data into smoothed polylines....may eventually break this out into adding each individual point of the contour versus the entire thing. Adding individual points one at a time will let me do some extra manipulation/shaping prior to adding to the line?
        smoothLines.clear();
        centroids.clear();
        
        for(int i=0; i<contourFinder.nBlobs;i++){
            if(i<numBlobsToCheck){
                ofPolyline cur;
                cur.addVertices(contourFinder.blobs[i].pts);
                cur.setClosed(true);
            
                smoothLines.push_back(cur.getSmoothed(smoothAmt));
                centroids.push_back(contourFinder.blobs[i].centroid);
            }
        }
       
        //This loop is for calculating and updating the point that revolves around the contour - there might be a better way to do this though
        //Code snippet adapted from //from the second version included in this post https://forum.openframeworks.cc/t/interpolating-through-vector-of-points-from-a-polyline/29592/4
        for(unsigned int i = 0; i < smoothLines.size(); i++) {
            float totalLength = smoothLines[i].getLengthAtIndex( smoothLines[i].size()-1 );
            
            
               // int lengthDelta = totalLength-prevLength;
                travelers[i] = travelers[i] +travelSpeed;// + lengthDelta; //maybe some way to smooth the delta between changes in the length of the polyline/contour?
                travelers[i] = fmod(travelers[i], totalLength);
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
                
                //Most of this is just for temporal smoothing of the point following the outline of the contour. Since the point is based on the length of the contour, it can get jumpy very easily as the contour gets larger. Might need another approach to this altogether
                ofVec3f avgTracePt;
                
                for(int i=0; i<prevTracePts[j].size(); i++){
                    avgTracePt = avgTracePt+prevTracePts[j][i];
                }
                
                avgTracePt = avgTracePt/prevTracePts[j].size();
                

                ribbons[j].addPoint(avgTracePt.x*drawScale, avgTracePt.y*drawScale, avgTracePt.z);
   
                prevTracePts[j].push_back(tracePts[j].front());
                
                if(prevTracePts[j].size()>timeSmoothAmt){
                    prevTracePts[j].erase(prevTracePts[j].begin(), prevTracePts[j].begin()+1);
                }
            }
        }
    }
    
    for (int i=0; i< ribbons.size(); i++){
        //lol the shear number of parameters im passing in here
        ribbons[i].updateVals(startNoiseAmp, startNoiseAmt, tailNoiseAmp, tailNoiseAmt, randomize, thickness, 0, zNoiseAmp, 15);
        ribbons[i].update();
    }
    

    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(50);
    
    
    if(bShowDebug){
        //TODO: Cleanup
        debugView();
        
        for( int i = 0; i < numPositions; i++ )
        {
            if(tracePts.size()>0){
                ofSetColor( 255,255,ofMap(i,0,numPositions, 0,255) );
                ofDrawCircle( tracePts[ i ].front()*drawScale, 5.f );
                //ofDrawBitmapStringHighlight(ofToString(i), tracePts[ i ].front()*drawScale);
                ofDrawBitmapStringHighlight(ofToString(i), centroids[ i ]*drawScale);
            }
        }
        
        ofSetColor(255, 0, 255);
        
        glPushMatrix();
        //glTranslated(20, 20, 0);
        ofScale(drawScale, drawScale);
        ofSetColor(255,255,0);
        ofSetColor(255);
        for(unsigned int i = 0; i < smoothLines.size(); i++) {
            ofSetColor(255,255,ofMap(i,0,smoothLines.size(), 0,255));
            smoothLines[i].draw();
        }
        glPopMatrix();
    }
    
    for (int i=0; i< ribbons.size(); i++){
        ribbons[i].draw();
    }

 
}

void ofApp::videoPctChanged(float &videoPctChanged){
    vidPlayer.setPosition(videoPct);
}

void ofApp::debugView(){
    
    ofSetColor(255);
    colorImg.draw(20,20);
    grayImage.draw(360,20);
    grayBg.draw(20,280);
    grayDiff.draw(360,280);
    
    ofFill();
    ofSetHexColor(0x333333);
    ofDrawRectangle(800,20,320,240);
    ofSetHexColor(0xffffff);
    
    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].draw(800,20);
    }
    
    ofSetColor(255);
    stringstream reportStr;
    reportStr << "threshold " << threshold << " (press: +/-)" << endl
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
