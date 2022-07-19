/************************************************************
************************************************************/
#include "mov.h"

/************************************************************
************************************************************/

/******************************
******************************/
MOV::MOV(){
}

/******************************
******************************/
MOV::~MOV(){
}

/******************************
******************************/
void MOV::setup(){
	/********************
	********************/
	VideoPlayer.load("mov/mov.mp4");
	VideoPlayer.setLoopState(OF_LOOP_NONE);
	// VideoPlayer.setLoopState(OF_LOOP_NORMAL);
	VideoPlayer.setSpeed(1);
	VideoPlayer.setVolume(1.0);
	// VideoPlayer.setVolume(0.0);
	VideoPlayer.setPaused(false);
	VideoPlayer.stop();
	// VideoPlayer.setPosition(0); // *.movは、"OF_LOOP_NONE"の時、明示的にsetPosition(0) しないと、戻らないようだ. : seekをplay の後に移動(2020.10.08)
	
	/********************
	********************/
	printf("video size = (%f, %f)\n", VideoPlayer.getWidth(), VideoPlayer.getHeight());
	fflush(stdout);
}

/******************************
******************************/
void MOV::Process_Gui_Seek(int now){
	/********************
	********************/
	if(b_GuiSeek){
		if(100 < now - t_KickSeek){
			b_GuiSeek = false;
			VideoPlayer.play();
		}
	}else{
		if(Gui_Global->Video_Button_Seek){
			Gui_Global->Video_Button_Seek = false;
			if(VideoPlayer.isPlaying()){
				float pos = Gui_Global->Video_SeekPos;
				
				VideoPlayer.setPaused(true);
				VideoPlayer.setPosition(pos);
				
				b_GuiSeek = true;
				t_KickSeek = now;
			}
		}
	}
}

/******************************
******************************/
void MOV::draw(ofFbo& fbo){
	ofEnableAntiAliasing();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA); // OF_BLENDMODE_DISABLED, OF_BLENDMODE_ALPHA, OF_BLENDMODE_ADD
	
	if(VideoPlayer.isPlaying()){
		VideoPlayer.update();
		if(VideoPlayer.isFrameNew()) {
			fbo.begin();
				ofBackground(0);
				ofSetColor(255);
				VideoPlayer.draw(0, 0, fbo.getWidth(), fbo.getHeight());
			fbo.end();
		}
		
	}else{
		ClearFbo(fbo);
	}
}

/******************************
******************************/
void MOV::start(){
	VideoPlayer.play();
}

/******************************
******************************/
void MOV::stop(){
	VideoPlayer.stop();
}

/******************************
******************************/
bool MOV::isPlaying(){
	if(b_GuiSeek)	return true;
	else			return VideoPlayer.isPlaying();
}

/******************************
******************************/
float MOV::getPosition(){
	return VideoPlayer.getPosition();
}

/******************************
******************************/
int MOV::getTotalNumFrames(){
	return VideoPlayer.getTotalNumFrames();
}

/******************************
******************************/
int MOV::getCurrentFrame(){
	int val = VideoPlayer.getCurrentFrame();
	if(val < 0)	return 0;
	else		return val;
}

/******************************
******************************/
bool MOV::getIsMovieDone(){
	return VideoPlayer.getIsMovieDone();
}
