/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include <stdio.h>

#include <ofMain.h>
#include <ofxGui.h>

/************************************************************
************************************************************/
#define ERROR_MSG(); printf("Error in %s:%d\n", __FILE__, __LINE__);

/************************************************************
************************************************************/
enum{
	MAIN_WINDOW_W = 1280,
	MAIN_WINDOW_H = 460 * 2,
	
	MONITOR_WINDOW_W = 1920,
	MONITOR_WINDOW_H = 1080,
};

enum{
	BUF_SIZE_S = 500,
	BUF_SIZE_M = 1000,
	BUF_SIZE_L = 6000,
	BUF_SIZE_LL = 100000,
	
	UDP_BUF_SIZE = 100000,
};

enum{
	AUDIO_BUF_SIZE = 512,
	AUDIO_BUFFERS = 2,			// 使われてないっぽい
	AUDIO_SAMPLERATE = 44100,
};

/************************************************************
************************************************************/

/**************************************************
**************************************************/
class GUI_GLOBAL{
private:
	/****************************************
	****************************************/
	
public:
	/****************************************
	****************************************/
	void setup(string GuiName, string FileName = "gui.xml", float x = 10, float y = 10);
	
	ofxGuiGroup Group_Video;
		ofxToggle Video_Button_Seek;
		ofxFloatSlider Video_SeekPos;
		
	ofxGuiGroup Group_Arduino;
		ofxToggle Arduino_use_RotStage;
		ofxToggle Arduino_use_Table;
		ofxToggle Arduino_use_Shutter;
		
	ofxGuiGroup Group_RotStage;
		ofxFloatSlider RotStage_threshToDetectBlack;
		ofxFloatSlider RotStage_timeout_1R_init;
		ofxFloatSlider RotStage_timeout_1R_operation;
		
	ofxGuiGroup Group_Table;
		ofxFloatSlider Table_dAngle_Down;
		ofxFloatSlider Table_dAngle_Up;
		ofxFloatSlider Table_PushTime;
		ofxToggle Table_ReverseDirection;
		
	ofxGuiGroup Group_PrjShutter;
		ofxFloatSlider Shutter_Angle_Open;
		ofxFloatSlider Shutter_Angle_Close;
		
	ofxGuiGroup Group_DMX_test;
		ofxToggle DMX_test_Main;
		ofxToggle DMX_test_Back;
		ofxToggle DMX_test_L;
		ofxToggle DMX_test_R;
		ofxColorSlider DMX_test__Color;
		ofxFloatSlider DMX_test__Pan_L;
		ofxFloatSlider DMX_test__Pan_R;
		ofxFloatSlider DMX_test__Tilt_L;
		ofxFloatSlider DMX_test__Tilt_R;
		
	ofxGuiGroup Group_DMX_common;
		ofxFloatSlider DMX_T__Figure8;
		ofxFloatSlider DMX_T__Coloring;
		ofxFloatSlider DMX_Kime_Speed;
		ofxColorSlider DMX_Figure8__Color;
		
	ofxGuiGroup Group_DMX_L;
		ofxToggle DMX_L__Figure8_Pan_Reverse;
		ofxToggle DMX_L__Figure8_Tilt_Reverse;
		ofxFloatSlider DMX_L__Pan_min;
		ofxFloatSlider DMX_L__Pan_max;
		ofxFloatSlider DMX_L__Pan_init;
		ofxFloatSlider DMX_L__Pan_Kime;
		ofxFloatSlider DMX_L__Tilt_min;
		ofxFloatSlider DMX_L__Tilt_max;
		ofxFloatSlider DMX_L__Tilt_Figure8_ofs;
		ofxFloatSlider DMX_L__Tilt_init;
		ofxFloatSlider DMX_L__Tilt_Kime;
		
	ofxGuiGroup Group_DMX_R;
		ofxToggle DMX_R__Figure8_Pan_Reverse;
		ofxToggle DMX_R__Figure8_Tilt_Reverse;
		ofxFloatSlider DMX_R__Pan_min;
		ofxFloatSlider DMX_R__Pan_max;
		ofxFloatSlider DMX_R__Pan_init;
		ofxFloatSlider DMX_R__Pan_Kime;
		ofxFloatSlider DMX_R__Tilt_min;
		ofxFloatSlider DMX_R__Tilt_max;
		ofxFloatSlider DMX_R__Tilt_Figure8_ofs;
		ofxFloatSlider DMX_R__Tilt_init;
		ofxFloatSlider DMX_R__Tilt_Kime;
	
	ofxGuiGroup Group_C_RotStage;
		ofxToggle Command_Rot_action;
		ofxToggle Command_Rot_AcOn;
		ofxToggle Command_Rot_AcOff;
		ofxToggle Command_Rot_Meas;
		
	ofxGuiGroup Group_C_Table;
		ofxToggle Command_Table_Up;
		ofxToggle Command_Table_Down;
		
	ofxGuiGroup Group_C_Shutter;
		ofxToggle Command_Shutter_Open;
		ofxToggle Command_Shutter_Close;
		
	ofxGuiGroup Group_C_DMX;
		ofxToggle Command_DMX_test;
		ofxToggle Command_DMX_init_immediate;
		ofxToggle Command_DMX_init_Fade;
		ofxToggle Command_DMX_wait_Kime;
		ofxToggle Command_DMX_Kime;
		ofxToggle Command_DMX_Figure8;
		ofxToggle Command_DMX_Figure8_Color;
		ofxToggle Command_DMX_Coloring;
	
	ofxPanel gui;
	
	bool b_Disp = false;
};

/************************************************************
************************************************************/
template <typename T>
T sj_max(T a, T b)
{
	if(a < b)	return b;
	else		return a;
}

/************************************************************
************************************************************/
double LPF(double LastVal, double NewVal, double Alpha_dt, double dt);
double LPF(double LastVal, double NewVal, double Alpha);

bool checkIf_ContentsExist(char* ret, char* buf);
void Align_StringOfData(string& s);
void print_separatoin();
void ClearFbo(ofFbo& fbo);

float toRad(float val);
float toDeg(float val);


/************************************************************
************************************************************/
extern GUI_GLOBAL* Gui_Global;

extern FILE* fp_Log;

extern int GPIO_0;
extern int GPIO_1;

extern const float _PI;

/************************************************************
************************************************************/

