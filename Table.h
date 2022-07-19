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
class TABLE{
private:
	/****************************************
	****************************************/
	ofxUDPManager udp_Send;
	ofxUDPManager udp_Receive;
	
	BEAT_MONITOR BeatMonitor;
	
	/****************************************
	****************************************/
	void setup_udp();
	void sendUdpTo_Table__set_dAngle(int dAngle, int PushTime_ms);
	
public:
	/****************************************
	****************************************/
	TABLE();
	~TABLE();
	
	void setup();
	void update(int now);
	bool isAlive(int now);
	void sendUdpTo_Table__PushUp();
	void sendUdpTo_Table__PushDown();
};


