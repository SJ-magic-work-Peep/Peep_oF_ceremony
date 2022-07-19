/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "sj_common.h"

#include "RotStage.h"
#include "Shutter.h"
#include "Table.h"
#include "Dmx.h"
#include "mov.h"
#include "voice.h"

/************************************************************
************************************************************/
// #define SJ_DEBUG_TIMING

/************************************************************
************************************************************/
class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	/********************
	■C++ enum class【C++で導入された新しい列挙型の使い方】
		https://monozukuri-c.com/langcpp-funclist-enum-class/
	********************/
	enum class APP_STATE{
		CheckArdBeat_Setting,
		Initialize,
		SystemCheck,
		Movie,
		Mov_Break,
		Before_TableUp,
		TableUp,
		UpKime_wait,
		UpKime_Break,
		Speech,
		Speech_Break,
		Before_Rot,
		Rot,
		RotKime_wait,
		RotKime_Break,
		Closing,
		Closed,
		ReadyToNext,
		
		SystemPausedForTest,
		
		NumAppStates,
	};
	enum class ARD_DEVICE{
		Table,
		RotStage,
		Shutter,
		
		NumArdDevices,
	};
	
	/****************************************
	****************************************/
	int now;
	ofTrueTypeFont font_L;
	ofTrueTypeFont font_M;
	
	ofImage img_Back[ int(APP_STATE::NumAppStates) ];
	int timeout[ int(APP_STATE::NumAppStates) ];
	
	APP_STATE AppState = APP_STATE::CheckArdBeat_Setting;
	int t_ThisStateFrom = 0;
	bool b_initializeError = false;
	
	bool b_keyInput_Initiallize = false;
	bool b_keyInput_StartMovie = false;
	
	ofFbo fbo_Monitor;
	ofFbo fbo_mov;
	
	ofSoundPlayer sound_Table;
	ofSoundPlayer sound_Rotate;
	ofSoundPlayer sound_Speech;
	
	ofImage img_Checker;
	
	/********************
	********************/
	ROT_STAGE RotStage;
	SHUTTER Shutter;
	TABLE Table;
	DMX_SYSTEM DmxSystem;
	MOV mov;
	VOICE voice;
	
	bool b_ArduinoAlive[ int(ARD_DEVICE::NumArdDevices) ];
	
	int c_Loop_for_debug = 0;
	
	/****************************************
	****************************************/
	void set_sound(ofSoundPlayer& sound);
	void printDate();
	void load_BackImg();
	void setup_Gui();
	void Clear_keyInput();
	void StateChart_App();
	void load_gui_SettingFile();
	bool is_FileExist(string str_FileName);
	
	void update_AllArduinos();
	bool is_AllArduinoAlive();
	
	void Process__GuiCommand();
	void ClearGuiCommand();
	
	void Process__CheckArdBeat_Setting();
	void Process__Initialize();
	void Process__SystemCheck();
	void Process__Movie();
	void Process__Mov_Break();
	void Process__Before_TableUp();
	void Process__TableUp();
	void Process__UpKime_wait();
	void Process__UpKime_Break();
	void Process__Speech();
	void Process__Speech_Break();
	void Process__Before_Rot();
	void Process__Rot();
	void Process__RotKime_wait();
	void Process__RotKime_Break();
	void Process__Closing();
	void Process__Closed();
	void Process__ReadyToNext();
	
	void draw_on_eachState();
	void draw__timeout_passed(int _timeout);
	void draw__timeout_SoundPos(ofSoundPlayer& sound, int _timeout);
	void draw__passed();
	void draw__CheckArdBeat_Setting();
	void draw__SystemCheck();
	void draw__Movie();
	
	void update_fboMonitor__Black();
	void update_fboMonitor__Checker();
	void update_fboMonitor__mov();
	
public:
	/****************************************
	****************************************/
	ofApp();
	~ofApp();
	
	void setup();
	void update();
	void draw();
	void exit();
	
	ofFbo& getFbo_Monitor();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};
