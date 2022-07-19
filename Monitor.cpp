/************************************************************
************************************************************/
#include "MONITOR.h"

/************************************************************
************************************************************/

/******************************
******************************/
MONITOR::MONITOR(){
	printf("> MONITOR::ctr\n");
	fflush(stdout);
}

/******************************
******************************/
MONITOR::~MONITOR(){
	printf("> MONITOR::dtr\n");
	fflush(stdout);
}

/******************************
******************************/
void MONITOR::exit(){
	printf("> MONITOR::exit\n");
	fflush(stdout);	
}

/******************************
******************************/
void MONITOR::setup(){
	/********************
	********************/
	ofSetBackgroundAuto(true);
	
	ofSetWindowTitle("Peep : Monitor");
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	ofSetWindowShape(MONITOR_WINDOW_W, MONITOR_WINDOW_H);
	ofSetEscapeQuitsApp(false);
}

/******************************
******************************/
void MONITOR::update(){

}

/******************************
******************************/
void MONITOR::draw(){
	/********************
	********************/
	ofEnableAntiAliasing();
	ofEnableBlendMode(OF_BLENDMODE_DISABLED); // OF_BLENDMODE_DISABLED, OF_BLENDMODE_ALPHA, OF_BLENDMODE_ADD
	
	ofBackground(0, 0, 0, 255);
	
	ofSetColor(255);
	if(main_obj->getFbo_Monitor().isAllocated()){
		main_obj->getFbo_Monitor().draw(0, 0, ofGetWidth(), ofGetHeight());
	}
}

/******************************
******************************/
void MONITOR::keyPressed(int key){
	switch(key){
		case 'j':
			img_Screen.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			img_Screen.save("screenshot.jpg");
			
			printf("> saved image\n");
			fflush(stdout);
			break;
			
		case 'p':
			img_Screen.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			img_Screen.save("screenshot.png");
			
			printf("> saved image\n");
			fflush(stdout);
			break;
	}
}

/******************************
******************************/
void MONITOR::keyReleased(int key){

}

/******************************
******************************/
void MONITOR::mouseMoved(int x, int y ){

}

/******************************
******************************/
void MONITOR::mouseDragged(int x, int y, int button){

}

/******************************
******************************/
void MONITOR::mousePressed(int x, int y, int button){

}

/******************************
******************************/
void MONITOR::mouseReleased(int x, int y, int button){

}

/******************************
******************************/
void MONITOR::mouseEntered(int x, int y){

}

/******************************
******************************/
void MONITOR::mouseExited(int x, int y){

}

/******************************
******************************/
void MONITOR::windowResized(int w, int h){

}

/******************************
******************************/
void MONITOR::gotMessage(ofMessage msg){

}

/******************************
******************************/
void MONITOR::dragEvent(ofDragInfo dragInfo){ 

}


