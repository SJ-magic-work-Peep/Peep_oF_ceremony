/************************************************************
************************************************************/
#include <sj_common.h>
#include <time.h>

/************************************************************
************************************************************/
/********************
********************/
int GPIO_0 = 0;
int GPIO_1 = 0;

const float _PI = 3.1415;

/********************
********************/
GUI_GLOBAL* Gui_Global = NULL;

FILE* fp_Log = nullptr;


/************************************************************
func
************************************************************/
/******************************
******************************/
double LPF(double LastVal, double NewVal, double Alpha_dt, double dt)
{
	double Alpha;
	if((Alpha_dt <= 0) || (Alpha_dt < dt))	Alpha = 1;
	else									Alpha = 1/Alpha_dt * dt;
	
	return NewVal * Alpha + LastVal * (1 - Alpha);
}

/******************************
******************************/
double LPF(double LastVal, double NewVal, double Alpha)
{
	if(Alpha < 0)		Alpha = 0;
	else if(1 < Alpha)	Alpha = 1;
	
	return NewVal * Alpha + LastVal * (1 - Alpha);
}

/******************************
******************************/
bool checkIf_ContentsExist(char* ret, char* buf)
{
	if( (ret == NULL) || (buf == NULL)) return false;
	
	string str_Line = buf;
	Align_StringOfData(str_Line);
	vector<string> str_vals = ofSplitString(str_Line, ",");
	if( (str_vals.size() == 0) || (str_vals[0] == "") ){ // no_data or exist text but it's",,,,,,,".
		return false;
	}else{
		return true;
	}
}

/******************************
******************************/
void Align_StringOfData(string& s)
{
	size_t pos;
	while((pos = s.find_first_of(" 　\t\n\r")) != string::npos){ // 半角・全角space, \t 改行 削除
		s.erase(pos, 1);
	}
}

/******************************
******************************/
void print_separatoin()
{
	printf("---------------------------------\n");
}

/******************************
******************************/
void ClearFbo(ofFbo& fbo)
{
	fbo.begin();
		// ofClear(ofColor(Gui_Global->col_ClearFbo, Gui_Global->col_ClearFbo, Gui_Global->col_ClearFbo, 255));
		ofClear(0, 0, 0, 255);
	fbo.end();
}

/******************************
******************************/
float toRad(float val){
	return val * _PI / 180.0;
}

/******************************
******************************/
float toDeg(float val){
	return val * 180.0 / _PI;
}

/************************************************************
class
************************************************************/

/******************************
******************************/
void GUI_GLOBAL::setup(string GuiName, string FileName, float x, float y)
{
	/********************
	********************/
	gui.setup(GuiName.c_str(), FileName.c_str(), x, y);
	
	/********************
	********************/
	Group_Video.setup("Video");
		Group_Video.add(Video_Button_Seek.setup("Seek", false));
		Group_Video.add(Video_SeekPos.setup("SeekPos", 0.98, 0.0, 1.0));
	gui.add(&Group_Video);
	
	Group_Arduino.setup("Arduino");
		Group_Arduino.add(Arduino_use_Table.setup("Ard_use_Table", true));
		Group_Arduino.add(Arduino_use_RotStage.setup("Ard_use_RotStage", true));
		Group_Arduino.add(Arduino_use_Shutter.setup("Ard_use_Shutter", true));
	gui.add(&Group_Arduino);
		
	Group_RotStage.setup("RotStage");
		Group_RotStage.add(RotStage_threshToDetectBlack.setup("th:DetectBlack", 250, 10, 1000));
		Group_RotStage.add(RotStage_timeout_1R_init.setup("t_1R_init", 55, 5, 100));
		Group_RotStage.add(RotStage_timeout_1R_operation.setup("t_1R_ope", 32, 5, 100));
	gui.add(&Group_RotStage);
	
	Group_Table.setup("Table");
		// Group_Table.add(Table_dAngle_Down.setup("dAngle_Down", 28, 0, 90));
		Group_Table.add(Table_dAngle_Down.setup("dAngle_Down", 26, 0, 90));
		Group_Table.add(Table_dAngle_Up.setup("dAngle_Up", 37, 0, 90));
		Group_Table.add(Table_PushTime.setup("PushTime", 700, 100, 2000));
		Group_Table.add(Table_ReverseDirection.setup("Reverse", false));
	gui.add(&Group_Table);
	
	Group_PrjShutter.setup("Shutter");
		Group_PrjShutter.add(Shutter_Angle_Open.setup("Open", 0, 0, 180));
		Group_PrjShutter.add(Shutter_Angle_Close.setup("Close", 180, 0, 180));
	gui.add(&Group_PrjShutter);
	
	Group_DMX_test.setup("DMX test");
		Group_DMX_test.add(DMX_test_Main.setup("DMX_test_Main", true));
		Group_DMX_test.add(DMX_test_Back.setup("DMX_test_Back", true));
		Group_DMX_test.add(DMX_test_L.setup("DMX_test_L", true));
		Group_DMX_test.add(DMX_test_R.setup("DMX_test_R", true));
		{
			ofColor initColor = ofColor(0, 0, 255, 255);
			ofColor minColor = ofColor(0, 0, 0, 0);
			ofColor maxColor = ofColor(255, 255, 255, 255);
			Group_DMX_test.add(DMX_test__Color.setup("color", initColor, minColor, maxColor));
		}
		Group_DMX_test.add(DMX_test__Pan_L.setup("Pan_L", 0, 0, 65535));
		Group_DMX_test.add(DMX_test__Pan_R.setup("Pan_R", 0, 0, 65535));
		Group_DMX_test.add(DMX_test__Tilt_L.setup("Tilt_L", 0, 0, 65535));
		Group_DMX_test.add(DMX_test__Tilt_R.setup("Tilt_R", 0, 0, 65535));
	gui.add(&Group_DMX_test);
	
	Group_DMX_common.setup("DMX common");
		Group_DMX_common.add(DMX_T__Figure8.setup("T:Figure8", 2200, 500, 30000));
		Group_DMX_common.add(DMX_T__Coloring.setup("T:Coloring", 2388, 100, 4000));
		Group_DMX_common.add(DMX_Kime_Speed.setup("DMX_Kime_Speed", 200, 0, 255));
		{
			ofColor initColor = ofColor(255, 0, 0, 255);
			ofColor minColor = ofColor(0, 0, 0, 0);
			ofColor maxColor = ofColor(255, 255, 255, 255);
			Group_DMX_common.add(DMX_Figure8__Color.setup("Figure8_col", initColor, minColor, maxColor));
		}
	gui.add(&Group_DMX_common);
	
	Group_DMX_L.setup("DMX L");
		Group_DMX_L.add(DMX_L__Figure8_Pan_Reverse.setup("L_Fig8_Pan_Rev", false));
		Group_DMX_L.add(DMX_L__Figure8_Tilt_Reverse.setup("L_Fig8_Tilt_Rev", false));
		Group_DMX_L.add(DMX_L__Pan_min.setup("L_Pan_min", 37000, 0, 65535));
		Group_DMX_L.add(DMX_L__Pan_max.setup("L_Pan_max", 52000, 0, 65535));
		Group_DMX_L.add(DMX_L__Pan_init.setup("L_Pan_init", 44100, 0, 65535));
		Group_DMX_L.add(DMX_L__Pan_Kime.setup("L_Pan_Kime", 44100, 0, 65535));
		Group_DMX_L.add(DMX_L__Tilt_min.setup("L_Tilt_min", 6000, 0, 65535));
		Group_DMX_L.add(DMX_L__Tilt_max.setup("L_Tilt_max", 21000, 0, 65535));
		Group_DMX_L.add(DMX_L__Tilt_Figure8_ofs.setup("L_Tilt_Figure8_ofs", 0, -90, 90));
		Group_DMX_L.add(DMX_L__Tilt_init.setup("L_Tilt_init", 6000, 0, 65535));
		Group_DMX_L.add(DMX_L__Tilt_Kime.setup("L_Tilt_Kime", 6000, 0, 65535));
	gui.add(&Group_DMX_L);
	
	Group_DMX_R.setup("DMX R");
		Group_DMX_R.add(DMX_R__Figure8_Pan_Reverse.setup("R_Fig8_Pan_Rev", false));
		Group_DMX_R.add(DMX_R__Figure8_Tilt_Reverse.setup("R_Fig8_Tilt_Rev", false));
		Group_DMX_R.add(DMX_R__Pan_min.setup("R_Pan_min", 37000, 0, 65535));
		Group_DMX_R.add(DMX_R__Pan_max.setup("R_Pan_max", 52000, 0, 65535));
		Group_DMX_R.add(DMX_R__Pan_init.setup("R_Pan_init", 44100, 0, 65535));
		Group_DMX_R.add(DMX_R__Pan_Kime.setup("R_Pan_Kime", 44100, 0, 65535));
		Group_DMX_R.add(DMX_R__Tilt_min.setup("R_Tilt_min", 6000, 0, 65535));
		Group_DMX_R.add(DMX_R__Tilt_max.setup("R_Tilt_max", 21000, 0, 65535));
		Group_DMX_R.add(DMX_R__Tilt_Figure8_ofs.setup("R_Tilt_Figure8_ofs", 0, -90, 90));
		Group_DMX_R.add(DMX_R__Tilt_init.setup("R_Tilt_init", 6000, 0, 65535));
		Group_DMX_R.add(DMX_R__Tilt_Kime.setup("R_Tilt_Kime", 6000, 0, 65535));
	gui.add(&Group_DMX_R);
	
	Group_C_RotStage.setup("C:RotState");
		Group_C_RotStage.add(Command_Rot_action.setup("action", false));
		Group_C_RotStage.add(Command_Rot_AcOn.setup("AC ON", false));
		Group_C_RotStage.add(Command_Rot_AcOff.setup("AC OFF", false));
		Group_C_RotStage.add(Command_Rot_Meas.setup("Meas", false));
	gui.add(&Group_C_RotStage);
	
	Group_C_Table.setup("C:Table");
		Group_C_Table.add(Command_Table_Up.setup("Up", false));
		Group_C_Table.add(Command_Table_Down.setup("Down", false));
	gui.add(&Group_C_Table);
	
	Group_C_Shutter.setup("C:Shutter");
		Group_C_Shutter.add(Command_Shutter_Open.setup("Open", false));
		Group_C_Shutter.add(Command_Shutter_Close.setup("Close", false));
	gui.add(&Group_C_Shutter);
	
	Group_C_DMX.setup("C:DMX");
		Group_C_DMX.add(Command_DMX_test.setup("Test", false));
		Group_C_DMX.add(Command_DMX_init_immediate.setup("init_immediate", false));
		Group_C_DMX.add(Command_DMX_init_Fade.setup("init_Fade", false));
		Group_C_DMX.add(Command_DMX_wait_Kime.setup("wait_Kime", false));
		Group_C_DMX.add(Command_DMX_Kime.setup("Kime", false));
		Group_C_DMX.add(Command_DMX_Figure8.setup("Figure8", false));
		Group_C_DMX.add(Command_DMX_Figure8_Color.setup("Figure8_Color", false));
		Group_C_DMX.add(Command_DMX_Coloring.setup("Coloring", false));
	gui.add(&Group_C_DMX);
	
	/********************
	********************/
	gui.minimizeAll();
}

