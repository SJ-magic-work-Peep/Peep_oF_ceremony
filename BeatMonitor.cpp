/************************************************************
************************************************************/
#include "BeatMonitor.h"

/************************************************************
************************************************************/

/******************************
******************************/
BEAT_MONITOR::BEAT_MONITOR(){
}

/******************************
******************************/
BEAT_MONITOR::~BEAT_MONITOR(){
}

/******************************
description
	Beat入ったので -> 現状update
******************************/
void BEAT_MONITOR::beat(int now){
	switch(State){
		case STATE::NoBeat:
			State = STATE::Beat;
			t_from = now;
			break;
			
		case STATE::Beat:
			t_from = now;
			break;
	}
}

/******************************
description
	Beatと関係なく、現状update
******************************/
void BEAT_MONITOR::update(int now){
	switch(State){
		case STATE::NoBeat:
			break;
			
		case STATE::Beat:
			if(1000 < now - t_from){
				State = STATE::NoBeat;
			}
			break;
	}
}

/******************************
description
******************************/
bool BEAT_MONITOR::isAlive(int now){
	/********************
	main側で明示的にupdateせずとも、聞かれたtimingで更新すれば良い。
	********************/
	update(now);
	
	/********************
	********************/
	if(State == STATE::Beat)	return true;
	else						return false;
}

