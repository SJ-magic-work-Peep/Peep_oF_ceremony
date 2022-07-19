/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofxNetwork.h"
#include "sj_common.h"
#include "BeatMonitor.h"

/************************************************************
************************************************************/
class SHUTTER{
private:
	/****************************************
	****************************************/
	ofxUDPManager udp_Send;
	ofxUDPManager udp_Receive;
	
	BEAT_MONITOR BeatMonitor;
	
	/****************************************
	****************************************/
	void setup_udp();
	void sendUdpTo_PrjShutter__setAngle(int angle);
	
public:
	/****************************************
	****************************************/
	SHUTTER();
	~SHUTTER();
	
	void setup();
	void update(int now);
	bool isAlive(int now);
	void sendUdpTo_PrjShutter__open();
	void sendUdpTo_PrjShutter__close();
};


