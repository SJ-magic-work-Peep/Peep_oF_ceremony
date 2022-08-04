/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"

#define ERROR_MSG(); printf("Error in %s:%d\n", __FILE__, __LINE__);

/************************************************************
************************************************************/
class VOICE{
public:
	/****************************************
	****************************************/
	enum class STATE{
		stop,
		interval,
		waitEnd,
		LastOne,
	};
private:
	
	/****************************************
	****************************************/
	string dirname = "../../../data/voice/";
	
	STATE State = STATE::stop;
	
	float t_interval;
	int id = 0;
	float t_From = 0;
	
	const int d_INTERVAL_MIN = 3000;
	const int d_INTERVAL_MAX = 7000;
	
	vector<int> order;
	
	vector<ofSoundPlayer*> Sounds;
	vector<string*> SoundFileNames;
	
	bool b_start = false;
	bool b_stop = false;
	
	/****************************************
	****************************************/
	void init_order();
	void makeup_music_table();
	void load_music_table();
	void prep_NextSoundInfo();
	void shuffle(vector<int>& data);
	void my_slide(vector<int>& data);
	void StateChart(int now);
	
public:
	/****************************************
	****************************************/
	VOICE();
	~VOICE();
	void setup();
	void update(int now);
	void PlayStart();
	void PlayStop();
	VOICE::STATE get_state();
	void stop_all_voice();
};
