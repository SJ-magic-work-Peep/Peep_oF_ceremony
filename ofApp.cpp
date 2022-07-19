/************************************************************
************************************************************/
#include "ofApp.h"
#include <time.h>

/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(){
	for(int i = 0; i < int(ARD_DEVICE::NumArdDevices); i++){
		b_ArduinoAlive[i] = false;
	}
	
	/********************
	********************/
	font_L.load("font/RictyDiminished-Regular.ttf", 34/* font size in pixels */, true/* _bAntiAliased */, true/* _bFullCharacterSet */, false/* makeContours */, 0.3f/* simplifyAmt */, 72/* dpi */);
	font_M.load("font/RictyDiminished-Regular.ttf", 20/* font size in pixels */, true/* _bAntiAliased */, true/* _bFullCharacterSet */, false/* makeContours */, 0.3f/* simplifyAmt */, 72/* dpi */);
	
	/********************
	********************/
	timeout[ int(APP_STATE::CheckArdBeat_Setting) ]		= 0;
	timeout[ int(APP_STATE::Initialize) ]				= 0;
	timeout[ int(APP_STATE::SystemCheck) ]				= 0;
	timeout[ int(APP_STATE::Movie) ]					= 0;
	timeout[ int(APP_STATE::Mov_Break) ]				= 3000;
	timeout[ int(APP_STATE::Before_TableUp) ]			= 2000;
	timeout[ int(APP_STATE::TableUp) ]					= 12011;
	// timeout[ int(APP_STATE::TableUp) ]					= 12032;
	timeout[ int(APP_STATE::UpKime_wait) ]				= 19584;
	timeout[ int(APP_STATE::UpKime_Break) ]				= 5737;
	timeout[ int(APP_STATE::Speech) ]					= 0;
	timeout[ int(APP_STATE::Speech_Break) ]				= 1498;
	timeout[ int(APP_STATE::Before_Rot) ]				= 2000;
	timeout[ int(APP_STATE::Rot) ]						= 31000;
	timeout[ int(APP_STATE::RotKime_wait) ]				= 3000;
	timeout[ int(APP_STATE::RotKime_Break) ]			= 12366;
	
#ifdef SJ_DEBUG_TIMING
	timeout[ int(APP_STATE::Closing) ]					= 3000;
	timeout[ int(APP_STATE::Closed) ]					= 3000;
	// timeout[ int(APP_STATE::Closed) ]					= 120000;
#else
	timeout[ int(APP_STATE::Closing) ]					= 30000;
	timeout[ int(APP_STATE::Closed) ]					= 120000;
#endif
	
	
	timeout[ int(APP_STATE::ReadyToNext) ]				= 5000;
	
	timeout[ int(APP_STATE::SystemPausedForTest) ]		= 0;
}

/******************************
******************************/
ofApp::~ofApp(){
	if(Gui_Global)	delete Gui_Global;
	if(fp_Log)		fclose(fp_Log);
}

/******************************
******************************/
void ofApp::exit(){
	printf("Good-bye\n");
	fflush(stdout);
}

/******************************
******************************/
void ofApp::setup(){
	/********************
	********************/
	ofSetBackgroundAuto(true);
	
	ofSetWindowTitle("Peep:ceremony");
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	ofSetWindowShape(MAIN_WINDOW_W, MAIN_WINDOW_H);
	ofSetEscapeQuitsApp(false);
	
	/********************
	********************/
	ofSeedRandom();
	
	ofEnableAntiAliasing();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA); // OF_BLENDMODE_DISABLED, OF_BLENDMODE_ALPHA, OF_BLENDMODE_ADD
	
	/********************
	********************/
	setup_Gui();
	load_gui_SettingFile();
	load_BackImg();
	
	/********************
	********************/
	fbo_Monitor.allocate(MONITOR_WINDOW_W, MONITOR_WINDOW_H, GL_RGBA, 4);
	ClearFbo(fbo_Monitor);
	
	fbo_mov.allocate(MONITOR_WINDOW_W, MONITOR_WINDOW_H, GL_RGBA, 4);
	ClearFbo(fbo_mov);
	
	/********************
	********************/
	sound_Table.load("music/TableUp.wav");
	set_sound(sound_Table);
	sound_Rotate.load("music/Rotate.wav");
	set_sound(sound_Rotate);
	sound_Speech.load("music/Speech.wav");
	set_sound(sound_Speech);
	
	/********************
	********************/
	img_Checker.load("img/img_Checker.png");
	
	/********************
	********************/
	Shutter.setup();
	RotStage.setup();
	Table.setup();
	
	DmxSystem.setup();
	mov.setup();
	
	voice.setup();
}

/******************************
******************************/
void ofApp::set_sound(ofSoundPlayer& sound){
	sound.setLoop(false);
	sound.setMultiPlay(true);
	sound.setSpeed(1.0);
	sound.setVolume(1.0);
}

/******************************
C初級：日付と時刻を取得する
	https://dianxnao.com/c%E5%88%9D%E7%B4%9A%EF%BC%9A%E6%97%A5%E4%BB%98%E3%81%A8%E6%99%82%E5%88%BB%E3%82%92%E5%8F%96%E5%BE%97%E3%81%99%E3%82%8B/
******************************/
void ofApp::printDate(){
	time_t timer;		// 時刻を取り出すための型（実際はunsigned long型）
	struct tm *local;	// tm構造体（時刻を扱う
	
	int year, month, day, hour, minute, second;
	
	timer = time(NULL);		// 現在時刻を取得
	local = localtime(&timer);	// 地方時に変換
	
	year = local->tm_year + 1900;	// 1900年からの年数が取得されるため
	month = local->tm_mon + 1;		// 0を1月としているため
	day = local->tm_mday;
	hour = local->tm_hour;
	minute = local->tm_min;
	second = local->tm_sec;
 
	printf("%d年%d月%d日 %d時%d分%d秒です\n", year, month, day, hour, minute, second);
	fflush(stdout);
}

/******************************
******************************/
void ofApp::load_BackImg()
{
	for(int i = 0; i < int(APP_STATE::NumAppStates); i++){
		char buf[100];
		sprintf(buf, "ScreenDesign/PeepScreen-%02d.png", i + 1);
		
		img_Back[i].load(buf);
	}
}

/******************************
description
	memoryを確保は、app start後にしないと、
	segmentation faultになってしまった。
******************************/
void ofApp::setup_Gui()
{
	/********************
	********************/
	Gui_Global = new GUI_GLOBAL;
	Gui_Global->setup("param", "gui.xml", 10, 10);
}

/******************************
******************************/
void ofApp::load_gui_SettingFile(){
	if(is_FileExist("../../../data/gui.xml")){
		Gui_Global->gui.loadFromFile("gui.xml");
		printf("> load gui setting\n");
		fflush(stdout);
	}else{
		printf("> GUI setting file not exists\n");
		fflush(stdout);
	}
}

/******************************
******************************/
bool ofApp::is_FileExist(string str_FileName){
	FILE* fp = fopen(str_FileName.c_str(), "r");
	if(fp){
		fclose(fp);
		return true;
	}else{
		return false;
	}
}

/******************************
******************************/
void ofApp::update(){
	/********************
	********************/
	ofSoundUpdate();
	
	now = ofGetElapsedTimeMillis();
	ClearFbo(fbo_Monitor);
	
	/********************
	********************/
	update_AllArduinos();
#ifndef SJ_DEBUG_TIMING
	if(!is_AllArduinoAlive()){
		AppState = APP_STATE::CheckArdBeat_Setting;
		DmxSystem.set_State(now, DMX_SYSTEM::STATE::Test);
		mov.stop();
	}
#endif

	/********************
	********************/
	Process__GuiCommand();
	
	StateChart_App();
	
	Clear_keyInput();
	
	DmxSystem.update(now);
	mov.Process_Gui_Seek(now);
	voice.update(now);
}

/******************************
******************************/
void ofApp::update_AllArduinos(){
	Shutter.update(now);
	if(Shutter.isAlive(now))	b_ArduinoAlive[ int(ARD_DEVICE::Shutter) ] = true;
	else						b_ArduinoAlive[ int(ARD_DEVICE::Shutter) ] = false;
	
	RotStage.update(now);
	if(RotStage.isAlive(now))	b_ArduinoAlive[ int(ARD_DEVICE::RotStage) ] = true;
	else						b_ArduinoAlive[ int(ARD_DEVICE::RotStage) ] = false;
	
	Table.update(now);
	if(Table.isAlive(now))		b_ArduinoAlive[ int(ARD_DEVICE::Table) ] = true;
	else						b_ArduinoAlive[ int(ARD_DEVICE::Table) ] = false;
}

/******************************
******************************/
bool ofApp::is_AllArduinoAlive(){
#ifdef SJ_DEBUG_TIMING
	return true;
#else
	int counter = 0;
	for(int i = 0; i < int(ARD_DEVICE::NumArdDevices); i++){
		if(b_ArduinoAlive[i])	counter++;
	}
	
	if( int(ARD_DEVICE::NumArdDevices) <= counter )	return true;
	else											return false;
	
#endif
}

/******************************
******************************/
void ofApp::Clear_keyInput(){
	b_keyInput_Initiallize = false;
	b_keyInput_StartMovie = false;
}

/******************************
******************************/
void ofApp::Process__GuiCommand(){
	if(AppState == APP_STATE::CheckArdBeat_Setting){
		if(Gui_Global->Command_Rot_action)			RotStage.sendUdpTo_RotStage__action(now, Gui_Global->RotStage_timeout_1R_init);
		if(Gui_Global->Command_Rot_AcOn)			RotStage.sendUdpTo_RotStage__AC_on();
		if(Gui_Global->Command_Rot_AcOff)			RotStage.sendUdpTo_RotStage__AC_off();
		if(Gui_Global->Command_Rot_Meas)			RotStage.sendUdpTo_RotStage__Meas();
		
		if(Gui_Global->Command_Table_Up)			Table.sendUdpTo_Table__PushUp();
		if(Gui_Global->Command_Table_Down)			Table.sendUdpTo_Table__PushDown();
		
		if(Gui_Global->Command_Shutter_Open)		Shutter.sendUdpTo_PrjShutter__open();
		if(Gui_Global->Command_Shutter_Close)		Shutter.sendUdpTo_PrjShutter__close();
		
		if(Gui_Global->Command_DMX_test)			DmxSystem.set_State(now, DMX_SYSTEM::STATE::Test);
		if(Gui_Global->Command_DMX_init_immediate)	DmxSystem.set_State(now, DMX_SYSTEM::STATE::InitialImmediate);
		if(Gui_Global->Command_DMX_init_Fade)		DmxSystem.set_State(now, DMX_SYSTEM::STATE::InitialFade);
		if(Gui_Global->Command_DMX_wait_Kime)		DmxSystem.set_State(now, DMX_SYSTEM::STATE::wait_Kime);
		if(Gui_Global->Command_DMX_Kime)			DmxSystem.set_State(now, DMX_SYSTEM::STATE::Kime);
		if(Gui_Global->Command_DMX_Figure8)			DmxSystem.set_State(now, DMX_SYSTEM::STATE::Figure8);
		if(Gui_Global->Command_DMX_Figure8_Color)	DmxSystem.set_State(now, DMX_SYSTEM::STATE::Figure8_Color);
		if(Gui_Global->Command_DMX_Coloring)		DmxSystem.set_State(now, DMX_SYSTEM::STATE::Coloring);
		
	}
	
	ClearGuiCommand();
}

/******************************
******************************/
void ofApp::ClearGuiCommand(){
	Gui_Global->Command_Rot_action = false;
	Gui_Global->Command_Rot_AcOn = false;
	Gui_Global->Command_Rot_AcOff = false;
	Gui_Global->Command_Rot_Meas = false;
	
	Gui_Global->Command_Table_Up = false;
	Gui_Global->Command_Table_Down = false;
	
	Gui_Global->Command_Shutter_Open = false;
	Gui_Global->Command_Shutter_Close = false;
	
	Gui_Global->Command_DMX_test = false;
	Gui_Global->Command_DMX_init_immediate = false;
	Gui_Global->Command_DMX_init_Fade = false;
	Gui_Global->Command_DMX_wait_Kime = false;
	Gui_Global->Command_DMX_Kime = false;
	Gui_Global->Command_DMX_Figure8 = false;
	Gui_Global->Command_DMX_Figure8_Color = false;
	Gui_Global->Command_DMX_Coloring = false;
}

/******************************
******************************/
void ofApp::StateChart_App(){
	switch(AppState){
		case APP_STATE::CheckArdBeat_Setting:
			Process__CheckArdBeat_Setting();
			break;
		case APP_STATE::Initialize:
			Process__Initialize();
			break;
		case APP_STATE::SystemCheck:
			Process__SystemCheck();
			break;
		case APP_STATE::Movie:
			Process__Movie();
			break;
		case APP_STATE::Mov_Break:
			Process__Mov_Break();
			break;
		case APP_STATE::Before_TableUp:
			Process__Before_TableUp();
			break;
		case APP_STATE::TableUp:
			Process__TableUp();
			break;
		case APP_STATE::UpKime_wait:
			Process__UpKime_wait();
			break;
		case APP_STATE::UpKime_Break:
			Process__UpKime_Break();
			break;
		case APP_STATE::Speech:
			Process__Speech();
			break;
		case APP_STATE::Speech_Break:
			Process__Speech_Break();
			break;
		case APP_STATE::Before_Rot:
			Process__Before_Rot();
			break;
		case APP_STATE::Rot:
			Process__Rot();
			break;
		case APP_STATE::RotKime_wait:
			Process__RotKime_wait();
			break;
		case APP_STATE::RotKime_Break:
			Process__RotKime_Break();
			break;
		case APP_STATE::Closing:
			Process__Closing();
			break;
		case APP_STATE::Closed:
			Process__Closed();
			break;
		case APP_STATE::ReadyToNext:
			Process__ReadyToNext();
			break;
		case APP_STATE::SystemPausedForTest:
			// 行き止まり : for test.
			break;
		default:
			break;
	}
}

/******************************
******************************/
void ofApp::Process__CheckArdBeat_Setting(){
	if(b_keyInput_Initiallize){
		if(is_AllArduinoAlive()){
			AppState = APP_STATE::Initialize;
			t_ThisStateFrom = now;
			
			Shutter.sendUdpTo_PrjShutter__open();
			Table.sendUdpTo_Table__PushDown();
			RotStage.sendUdpTo_RotStage__action(now, Gui_Global->RotStage_timeout_1R_init);
			DmxSystem.set_State(now, DMX_SYSTEM::STATE::Test);
			b_initializeError = false;
		}
	}
}

/******************************
******************************/
void ofApp::Process__Initialize(){
#ifdef SJ_DEBUG_TIMING
	if(3000 < now - t_ThisStateFrom){
		AppState = APP_STATE::SystemCheck;
		t_ThisStateFrom = now;
	}
#else
	if( (RotStage.get_LastResult() == ROT_STAGE::RESULT::OverRun) || (RotStage.get_LastResult() == ROT_STAGE::RESULT::Timeout) ){
		AppState = APP_STATE::CheckArdBeat_Setting;
		b_initializeError = true;
		t_ThisStateFrom = now;
	}else if(RotStage.get_LastResult() == ROT_STAGE::RESULT::Succeed){
		AppState = APP_STATE::SystemCheck;
		t_ThisStateFrom = now;
	}
#endif
}

/******************************
******************************/
void ofApp::Process__SystemCheck(){
	if(b_keyInput_StartMovie){
		AppState = APP_STATE::Movie;
		ofResetElapsedTimeCounter(); 	// must be called before update "now"
		now = ofGetElapsedTimeMillis();	// counter reset後なので、改めて時刻取得する必要あり(not "now").
		t_ThisStateFrom = now;
		
		mov.start();
		DmxSystem.set_State(now, DMX_SYSTEM::STATE::InitialImmediate);
		RotStage.clearCounter_Succeed();
	}
}

/******************************
******************************/
void ofApp::Process__Movie(){
	if(mov.getIsMovieDone()){
		AppState = APP_STATE::Mov_Break;
		t_ThisStateFrom = now;
	}
}

/******************************
******************************/
void ofApp::Process__Mov_Break(){
	if(timeout[ int(APP_STATE::Mov_Break) ] < now - t_ThisStateFrom){
		AppState = APP_STATE::Before_TableUp;
		t_ThisStateFrom = now;
		
		sound_Table.play();
		DmxSystem.set_State(now, DMX_SYSTEM::STATE::Figure8);
	}
}

/******************************
******************************/
void ofApp::Process__Before_TableUp(){
	if(timeout[ int(APP_STATE::Before_TableUp) ] < sound_Table.getPositionMS()){
		AppState = APP_STATE::TableUp;
		t_ThisStateFrom = now;
		
		Table.sendUdpTo_Table__PushUp();
		Shutter.sendUdpTo_PrjShutter__close();
	}
}

/******************************
******************************/
void ofApp::Process__TableUp(){
	if(timeout[ int(APP_STATE::TableUp) ] < sound_Table.getPositionMS()){
		AppState = APP_STATE::UpKime_wait;
		t_ThisStateFrom = now;
		
		DmxSystem.set_State(now, DMX_SYSTEM::STATE::Figure8_Color);
	}
}

/******************************
******************************/
void ofApp::Process__UpKime_wait(){
	if(timeout[ int(APP_STATE::UpKime_wait) ] < sound_Table.getPositionMS()){
		AppState = APP_STATE::UpKime_Break;
		t_ThisStateFrom = now;
		
		DmxSystem.set_State(now, DMX_SYSTEM::STATE::Kime);
	}
}

/******************************
******************************/
void ofApp::Process__UpKime_Break(){
	if(timeout[ int(APP_STATE::UpKime_Break) ] < now - t_ThisStateFrom){
		AppState = APP_STATE::Speech;
		t_ThisStateFrom = now;
		
		sound_Speech.play();
	}
}

/******************************
******************************/
void ofApp::Process__Speech(){
	if(!sound_Speech.isPlaying()){
		AppState = APP_STATE::Speech_Break;
		t_ThisStateFrom = now;
	}
}

/******************************
******************************/
void ofApp::Process__Speech_Break(){
	if(timeout[ int(APP_STATE::Speech_Break) ] < now - t_ThisStateFrom){
		AppState = APP_STATE::Before_Rot;
		t_ThisStateFrom = now;
		
		sound_Rotate.play();
		DmxSystem.set_State(now, DMX_SYSTEM::STATE::Coloring);
		RotStage.sendUdpTo_RotStage__action(now, Gui_Global->RotStage_timeout_1R_operation);
	}
}

/******************************
******************************/
void ofApp::Process__Before_Rot(){
	if(timeout[ int(APP_STATE::Before_Rot) ] < sound_Rotate.getPositionMS()){
		AppState = APP_STATE::Rot;
		t_ThisStateFrom = now;
	}
}

/******************************
******************************/
void ofApp::Process__Rot(){
#ifdef SJ_DEBUG_TIMING
	if(timeout[ int(APP_STATE::Rot) ] < sound_Rotate.getPositionMS()){
		AppState = APP_STATE::RotKime_Break;
		
		t_ThisStateFrom = now;
		
		DmxSystem.set_State(now, DMX_SYSTEM::STATE::Kime);
	}
#else
	if( (RotStage.get_LastResult() == ROT_STAGE::RESULT::Succeed) || (RotStage.get_LastResult() == ROT_STAGE::RESULT::OverRun) ){
		AppState = APP_STATE::RotKime_Break;
		
		t_ThisStateFrom = now;
		
		DmxSystem.set_State(now, DMX_SYSTEM::STATE::Kime);
	}
#endif
}

/******************************
******************************/
void ofApp::Process__RotKime_wait(){
	if(timeout[ int(APP_STATE::RotKime_wait) ] < sound_Rotate.getPositionMS()){
		AppState = APP_STATE::RotKime_Break;
		t_ThisStateFrom = now;
		
		DmxSystem.set_State(now, DMX_SYSTEM::STATE::Kime);
	}
}

/******************************
******************************/
void ofApp::Process__RotKime_Break(){
	if(timeout[ int(APP_STATE::RotKime_Break) ] < now - t_ThisStateFrom){
		AppState = APP_STATE::Closing;
		t_ThisStateFrom = now;
		
		Shutter.sendUdpTo_PrjShutter__open();
		Table.sendUdpTo_Table__PushDown();
		DmxSystem.set_State(now, DMX_SYSTEM::STATE::InitialImmediate);
	}
}

/******************************
******************************/
void ofApp::Process__Closing(){
	if(timeout[ int(APP_STATE::Closing) ] < now - t_ThisStateFrom){
		AppState = APP_STATE::Closed;
		t_ThisStateFrom = now;
		
		voice.PlayStart();
	}
}

/******************************
******************************/
void ofApp::Process__Closed(){
	/********************
	********************/
	if(timeout[ int(APP_STATE::Closed) ] < now - t_ThisStateFrom){
		AppState = APP_STATE::ReadyToNext;
		t_ThisStateFrom = now;
		
		voice.PlayStop();
		
		c_Loop_for_debug++;
	}
}

/******************************
******************************/
void ofApp::Process__ReadyToNext(){
	if(timeout[ int(APP_STATE::ReadyToNext) ] < now - t_ThisStateFrom){
		AppState = APP_STATE::Movie;
		ofResetElapsedTimeCounter(); 	// must be called before update "now"
		now = ofGetElapsedTimeMillis();	// counter reset後なので、改めて時刻取得する必要あり(not "now").
		t_ThisStateFrom = now;
		
		mov.start();
		DmxSystem.set_State(now, DMX_SYSTEM::STATE::InitialImmediate);
		
		voice.stop_all_voice();
	}
}

/******************************
******************************/
void ofApp::draw(){
	/********************
	********************/
	ofEnableAntiAliasing();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA); // OF_BLENDMODE_DISABLED, OF_BLENDMODE_ALPHA, OF_BLENDMODE_ADD
	
	/********************
	********************/
	ofBackground(20);
	ofSetColor(255);
	
	/********************
	********************/
	img_Back[int(AppState)].draw(0, 0);
	
	/********************
	********************/
	DmxSystem.SendDmx();
	mov.draw(fbo_mov);
	
	/********************
	********************/
	draw_on_eachState();
	
	/********************
	********************/
	if(Gui_Global->b_Disp)	Gui_Global->gui.draw();
}

/******************************
******************************/
void ofApp::draw_on_eachState(){
	switch(AppState){
		case APP_STATE::CheckArdBeat_Setting:
			draw__CheckArdBeat_Setting();
			update_fboMonitor__Checker();
			break;
		case APP_STATE::Initialize:
			draw__passed();
			update_fboMonitor__Checker();
			break;
		case APP_STATE::SystemCheck:
			// nothing to add.
			update_fboMonitor__Checker();
			break;
		case APP_STATE::Movie:
			draw__Movie();
			update_fboMonitor__mov();
			break;
		case APP_STATE::Mov_Break:
			draw__timeout_passed(timeout[ int(APP_STATE::Mov_Break) ]);
			break;
		case APP_STATE::Before_TableUp:
			draw__timeout_SoundPos(sound_Table, timeout[ int(APP_STATE::Before_TableUp) ]);
			break;
		case APP_STATE::TableUp:
			draw__timeout_SoundPos(sound_Table, timeout[ int(APP_STATE::TableUp) ]);
			break;
		case APP_STATE::UpKime_wait:
			draw__timeout_SoundPos(sound_Table, timeout[ int(APP_STATE::UpKime_wait) ]);
			break;
		case APP_STATE::UpKime_Break:
			draw__timeout_passed(timeout[ int(APP_STATE::UpKime_Break) ]);
			break;
		case APP_STATE::Speech:
			draw__passed();
			update_fboMonitor__Black();
			break;
		case APP_STATE::Speech_Break:
			draw__timeout_passed(timeout[ int(APP_STATE::Speech_Break) ]);
			break;
		case APP_STATE::Before_Rot:
			draw__timeout_SoundPos(sound_Rotate, timeout[ int(APP_STATE::Before_Rot) ]);
			break;
		case APP_STATE::Rot:
			draw__timeout_SoundPos(sound_Rotate, timeout[ int(APP_STATE::Rot) ]);
			break;
		case APP_STATE::RotKime_wait:
			draw__timeout_SoundPos(sound_Rotate, timeout[ int(APP_STATE::RotKime_wait) ]);
			break;
		case APP_STATE::RotKime_Break:
			draw__timeout_passed(timeout[ int(APP_STATE::RotKime_Break) ]);
			break;
		case APP_STATE::Closing:
			draw__timeout_passed(timeout[ int(APP_STATE::Closing) ]);
			break;
		case APP_STATE::Closed:
			draw__timeout_passed(timeout[ int(APP_STATE::Closed) ]);
			break;
		case APP_STATE::ReadyToNext:
			draw__timeout_passed(timeout[ int(APP_STATE::ReadyToNext) ]);
			break;
		case APP_STATE::SystemPausedForTest:
			// nothing to add.
			update_fboMonitor__Checker();
			break;
		default:
			break;
	}
}

/******************************
******************************/
void ofApp::draw__timeout_passed(int _timeout){
	/********************
	********************/
	ofSetColor(255);
	
	/********************
	********************/
	char buf[100];
	sprintf(buf, "%4d", _timeout / 1000);
	font_M.drawString(buf, 1008, 218);
	
	/********************
	********************/
	sprintf(buf, "%4d", (now - t_ThisStateFrom)/1000);
	font_M.drawString(buf, 1008, 253);
	
	/********************
	********************/
	update_fboMonitor__Black();
}

/******************************
******************************/
void ofApp::draw__timeout_SoundPos(ofSoundPlayer& sound, int _timeout){
	/********************
	********************/
	ofSetColor(255);
	
	font_M.drawString("music time", 817, 170);
	
	/********************
	********************/
	char buf[100];
	sprintf(buf, "%4d", _timeout / 1000);
	font_M.drawString(buf, 1008, 218);
	
	/********************
	********************/
	sprintf(buf, "%4d", sound.getPositionMS()/1000);
	font_M.drawString(buf, 1008, 253);
	
	/********************
	********************/
	update_fboMonitor__Black();
}

/******************************
******************************/
void ofApp::draw__passed(){
	/********************
	********************/
	char buf[100];
	sprintf(buf, "%4d", (now - t_ThisStateFrom)/1000);
	
	ofSetColor(255);
	font_M.drawString(buf, 1008, 253);
}

/******************************
******************************/
void ofApp::draw__CheckArdBeat_Setting(){
	/********************
	********************/
	if(b_ArduinoAlive[ int(ARD_DEVICE::Table) ])	{ ofSetColor(0, 0, 255, 255); font_L.drawString("OK", 1034, 134); }
	else											{ ofSetColor(255, 0, 0, 255); font_L.drawString("NG", 1034, 134); }
	
	if(b_ArduinoAlive[ int(ARD_DEVICE::RotStage) ])	{ ofSetColor(0, 0, 255, 255); font_L.drawString("OK", 1034, 193); }
	else											{ ofSetColor(255, 0, 0, 255); font_L.drawString("NG", 1034, 193); }
	
	if(b_ArduinoAlive[ int(ARD_DEVICE::Shutter) ])	{ ofSetColor(0, 0, 255, 255); font_L.drawString("OK", 1034, 251); }
	else											{ ofSetColor(255, 0, 0, 255); font_L.drawString("NG", 1034, 251); }
	
	if(b_initializeError){
		ofSetColor(255, 0, 0, 255);
		font_M.drawString("> initialize error. Please check RotStage.", 737, 334);
	}
}

/******************************
******************************/
void ofApp::draw__SystemCheck(){
	update_fboMonitor__Checker();
}

/******************************
******************************/
void ofApp::draw__Movie(){
	/********************
	********************/
	ofSetColor(255);
	fbo_mov.draw(612, 50, 640, 360);
	
	/********************
	********************/
	char buf[100];
	sprintf(buf, "%6d / %6d [frames]", mov.getCurrentFrame(), mov.getTotalNumFrames());
	
	ofSetColor(255);
	font_M.drawString(buf, 1012, 442);
}

/******************************
******************************/
void ofApp::update_fboMonitor__Black(){
	ClearFbo(fbo_Monitor);
}

/******************************
******************************/
void ofApp::update_fboMonitor__Checker(){
	ofEnableAntiAliasing();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA); // OF_BLENDMODE_DISABLED, OF_BLENDMODE_ALPHA, OF_BLENDMODE_ADD
	
	fbo_Monitor.begin();
		ofClear(0, 0, 0, 255);
		ofSetColor(255);
		
		img_Checker.draw(0, 0, fbo_Monitor.getWidth(), fbo_Monitor.getHeight());
	fbo_Monitor.end();
}

/******************************
******************************/
void ofApp::update_fboMonitor__mov(){
	ofEnableAntiAliasing();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA); // OF_BLENDMODE_DISABLED, OF_BLENDMODE_ALPHA, OF_BLENDMODE_ADD
	
	fbo_Monitor.begin();
		ofClear(0, 0, 0, 255);
		ofSetColor(255);
		
		fbo_mov.draw(0, 0, fbo_Monitor.getWidth(), fbo_Monitor.getHeight());
	fbo_Monitor.end();
}

/******************************
******************************/
ofFbo& ofApp::getFbo_Monitor(){
	return fbo_Monitor;
}

/******************************
******************************/
void ofApp::keyPressed(int key){
	switch(key){
		case 'd':
			Gui_Global->b_Disp = !Gui_Global->b_Disp;
			break;
			
		case 'm':
			Gui_Global->gui.minimizeAll();
			break;
			
		case ' ':
			b_keyInput_Initiallize = true;
			break;
			
		case 'g':
			b_keyInput_StartMovie = true;
			break;
			
		case 't':
			printf(">--------------------\n");
			if(sound_Table.isPlaying()){
				printf("> sound_Table : %d [ms]\n", sound_Table.getPositionMS());
			}else if(sound_Rotate.isPlaying()){
				printf("> sound_Rotate : %d [ms]\n", sound_Rotate.getPositionMS());
			}
			
			printf("> staying at this state for : %d [ms]\n", ofGetElapsedTimeMillis() - t_ThisStateFrom);
			
			fflush(stdout);
			break;
			
		case 'c':
			printf("c_Loop_for_debug = %d\n", c_Loop_for_debug);
			fflush(stdout);
			break;
	}
}

/******************************
******************************/
void ofApp::keyReleased(int key){

}

/******************************
******************************/
void ofApp::mouseMoved(int x, int y ){

}

/******************************
******************************/
void ofApp::mouseDragged(int x, int y, int button){

}

/******************************
******************************/
void ofApp::mousePressed(int x, int y, int button){

}

/******************************
******************************/
void ofApp::mouseReleased(int x, int y, int button){

}

/******************************
******************************/
void ofApp::mouseEntered(int x, int y){

}

/******************************
******************************/
void ofApp::mouseExited(int x, int y){

}

/******************************
******************************/
void ofApp::windowResized(int w, int h){

}

/******************************
******************************/
void ofApp::gotMessage(ofMessage msg){

}

/******************************
******************************/
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
