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
class ROT_STAGE{
public:
	enum class RESULT{
		None,
		Succeed,
		OverRun,
		Timeout,
	};
	
private:
	/****************************************
	****************************************/
	enum class STATE{
		StandBy,
		wait_1R,
	};
	
	/****************************************
	****************************************/
	STATE State = STATE::StandBy;
	int t_from_1R = 0;
	RESULT LastResult = RESULT::None;
	
	ofxUDPManager udp_Send;
	ofxUDPManager udp_Receive;
	
	BEAT_MONITOR BeatMonitor;
	
	int c_Succeeded = 0;
	int c_OverRun = 0;
	int c_timeout = 0;
	
	/****************************************
	****************************************/
	void setup_udp();
	string get_str_date();
	void sendUdpTo_RotStage(string message);
	void check_timeout(int now);
	void LogResult();
	
public:
	/****************************************
	****************************************/
	ROT_STAGE();
	~ROT_STAGE();
	
	void setup();
	void update(int now);
	bool isAlive(int now);
	
	void sendUdpTo_RotStage__action(int now, int t_SafetyGuard);
	void sendUdpTo_RotStage__AC_on();
	void sendUdpTo_RotStage__AC_off();
	void sendUdpTo_RotStage__Meas();
	void clearCounter_Succeed();
	ROT_STAGE::RESULT get_LastResult();
};


