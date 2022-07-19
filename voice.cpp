/************************************************************
************************************************************/
#include "voice.h"

/* for dir search */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <unistd.h> 
#include <dirent.h>
#include <string>

/************************************************************
************************************************************/

/******************************
******************************/
VOICE::VOICE(){
}

/******************************
******************************/
VOICE::~VOICE(){
	/********************
	********************/
	for(int i = 0; i < Sounds.size(); i++){
		delete Sounds[i];
	}
	
	for(int i = 0; i < SoundFileNames.size(); i++){
		delete SoundFileNames[i];
	}
}

/******************************
******************************/
void VOICE::setup(){
	/********************
	********************/
	makeup_music_table();
	load_music_table();
	
	/********************
	********************/
	init_order();
	shuffle(order);
	id = 0;
}

/******************************
******************************/
void VOICE::init_order()
{
	order.resize(Sounds.size());
	
	for(int i = 0; i < order.size(); i++){
		order[i] = i;
	}
}

/******************************
******************************/
void VOICE::makeup_music_table()
{
	/********************
	********************/
	DIR *pDir;
	struct dirent *pEnt;
	struct stat wStat;
	string wPathName;

	pDir = opendir( dirname.c_str() );
	if ( NULL == pDir ) { ERROR_MSG(); std::exit(1); }
	
	pEnt = readdir( pDir );
	while ( pEnt ) {
		// .と..は処理しない
		if ( strcmp( pEnt->d_name, "." ) && strcmp( pEnt->d_name, ".." ) ) {
		
			// wPathName = dirname + "/" + pEnt->d_name;
			wPathName = dirname + pEnt->d_name;
			
			// ファイルの情報を取得
			if ( stat( wPathName.c_str(), &wStat ) ) {
				printf( "Failed to get stat %s \n", wPathName.c_str() );
				fflush(stdout);
				break;
			}
			
			if ( S_ISDIR( wStat.st_mode ) ) {
				// nothing.
			} else {
			
				vector<string> str = ofSplitString(pEnt->d_name, ".");
				if(str[str.size()-1] == "wav"){
					// string str_NewFileName = wPathName;
					// string str_NewFileName = pEnt->d_name;
					string* str_NewFileName = new string(pEnt->d_name);
					
					SoundFileNames.push_back(str_NewFileName);
				}
			}
		}
		
		pEnt = readdir( pDir ); // 次のファイルを検索する
	}

	closedir( pDir );
	
	/********************
	********************/
	if(SoundFileNames.size() == 0){
		ERROR_MSG();
		std::exit(1);
	}
 }

/******************************
******************************/
void VOICE::load_music_table()
{
	/********************
	********************/
	if(SoundFileNames.size() <= 0)	return;

	/********************
	********************/
	printf("> load Sound Files\n");
	Sounds.resize(SoundFileNames.size());
	for(int i = 0; i < Sounds.size(); i++){
		Sounds[i] = new ofSoundPlayer;
		
		Sounds[i]->loadSound(dirname + SoundFileNames[i]->c_str());
		Sounds[i]->setLoop(false);
		Sounds[i]->setMultiPlay(true);
		Sounds[i]->setVolume(1.0);
		
		printf("%3d:%s\n", i, SoundFileNames[i]->c_str());
	}
	printf("--------------------\n");
	fflush(stdout);
}

/******************************
******************************/
void VOICE::update(int now){
	StateChart(now);
}

/******************************
******************************/
void VOICE::prep_NextSoundInfo()
{
	t_interval = ofRandom(d_INTERVAL_MIN, d_INTERVAL_MAX);
	
	id++;
	if(Sounds.size() <= id){
		int LastId = order[ order.size() - 1 ];
		shuffle(order);
		if(order[0] == LastId) my_slide(order);
		
		id = 0;
	}
}

/******************************
description
	fisher yates法
	偏りをなくすため、回を追うごとに乱数範囲を狭めていくのがコツ
	http://www.fumiononaka.com/TechNotes/Flash/FN0212003.html
******************************/
void VOICE::shuffle(vector<int>& data)
{
	// int i = data.size();
	// while(i--){
	// while(--i){
	
	for(int i = data.size() - 1; 0 < i; i--){
		/********************
		int j = rand() % (i + 1);
		********************/
		int j = (int)( ((double)rand() / ((double)RAND_MAX + 1)) * (i + 1) );
		
		/********************
		********************/
		int temp = data[i];
		data[i] = data[j];
		data[j] = temp;
	}
}

/******************************
******************************/
void VOICE::my_slide(vector<int>& data){
	int temp = data[0];
	for(int i = 0; i < data.size() - 1; i++){
		data[i] = data[i + 1];
	}
	
	data[data.size() - 1] = temp;
}

/******************************
******************************/
void VOICE::StateChart(int now){
	/********************
	********************/
	if(b_stop){
		State = STATE::stop;
		shuffle(order);
		id = 0;
	}
	
	/********************
	********************/
	switch(State){
		case STATE::stop:
			if(b_start){
				State = STATE::interval;
				t_From = now;
				t_interval = ofRandom(d_INTERVAL_MIN, d_INTERVAL_MAX);
				stop_all_voice();
			}
			break;
		case STATE::interval:
			if(t_interval < now - t_From){
				State = STATE::waitEnd;
				Sounds[ order[id] ]->play();
				
				printf("play : %s\n", SoundFileNames[ order[id] ]->c_str());
				fflush(stdout);
			}
			break;
		case STATE::waitEnd:
			if(!Sounds[ order[id] ]->isPlaying()){
				State = STATE::interval;
				prep_NextSoundInfo();
				t_From = now;
			}
			break;
	}
	
	/********************
	********************/
	b_start = false;
	b_stop = false;
}

/******************************
******************************/
void VOICE::stop_all_voice(){
	for(int i = 0; i < Sounds.size(); i++){
		Sounds[i]->stop();
	}
}

/******************************
******************************/
VOICE::STATE VOICE::get_state(){
	return State;
}

/******************************
******************************/
void VOICE::PlayStart(){
	b_start = true;
}

/******************************
******************************/
void VOICE::PlayStop(){
	b_stop = true;
}

