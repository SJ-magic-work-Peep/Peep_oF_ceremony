/************************************************************
************************************************************/
#include "Dmx.h"

/************************************************************
************************************************************/

/******************************
******************************/
DMX_SYSTEM::DMX_SYSTEM()
: col_Black(0.0, 0.0, 0.0, 0.0)
, col_White(1.0, 1.0, 1.0, 0.0)
, col_SimpleWhite(0.0, 0.0, 0.0, 1.0)
, col_StrongWhite(1.0, 1.0, 1.0, 1.0)
{
	col_Performance[0] = LIGHT_COLOR(1.0, 1.0, 0.0, 0.0);	// Yellow
	col_Performance[1] = LIGHT_COLOR(0.0, 1.0, 1.0, 0.0);	// Cian
	col_Performance[2] = LIGHT_COLOR(1.0, 0.0, 1.0, 0.0);	// Pink
	col_Performance[3] = LIGHT_COLOR(0.5, 0.0, 1.0, 0.0);	// Purple
	
	for(int i = 0; i < 4; i++){
		Order_ColPerformance[i] = i;
	}
}

/******************************
******************************/
DMX_SYSTEM::~DMX_SYSTEM(){
	ClearDmxData();
	SendDmx();
}

/******************************
******************************/
void DMX_SYSTEM::setup(){
	/********************
	********************/
	LightParam[ int(DEVICE::Main) ].setup(0, LIGHT_PARAM::DEVICE_TYPE::Fixed);
	LightParam[ int(DEVICE::Back) ].setup(6, LIGHT_PARAM::DEVICE_TYPE::Fixed);
	LightParam[ int(DEVICE::Moving_L) ].setup(12, LIGHT_PARAM::DEVICE_TYPE::Moving);
	LightParam[ int(DEVICE::Moving_R) ].setup(25, LIGHT_PARAM::DEVICE_TYPE::Moving);
	
	/********************
	********************/
	ClearDmxData();
	artnet.setup("10.7.153.16"); // Target IP.
	
#ifdef ARTNET_THREADED
	artnet.enableThread(30);	// main windowと同じにしておくのが安全かな.
#endif
}

/******************************
******************************/
void DMX_SYSTEM::ClearDmxData(){
	for(int i = 0; i < DMX_SIZE; i++){
		DmxData[i] = 0;
	}
}

/******************************
******************************/
void DMX_SYSTEM::update(int now){
	update_Param(now);			// LightParam[4]をupdateし、
	update_DmxDataArray();		// LightParam[4]をDataArrayにset.
}

/******************************
******************************/
void DMX_SYSTEM::update_Param(int now){
	/********************
	set target
	********************/
	switch(State){
		case STATE::Test:
			update_Param__Test();
			break;
		case STATE::InitialImmediate:
			update_Param__InitialImmediate();
			break;
		case STATE::InitialFade:
			update_Param__InitialFade();
			break;
		case STATE::Kime:
			update_Param__Kime();
			break;
		case STATE::wait_Kime:
			update_Param__waitKime();
			break;
		case STATE::Figure8:
			update_Param__Figure8(now);
			break;
		case STATE::Figure8_Color:
			update_Param__Figure8_color(now);
			break;
		case STATE::Coloring:
			update_Param__Coloring(now);
			break;
	}
	
	/********************
	and fade (color).
	********************/
	for(int i = 0; i < int(DEVICE::NUM_DEVICES); i++){
		LightParam[i].update_color();
	}
}

/******************************
******************************/
void DMX_SYSTEM::update_Param__Test(){
	float d_Color = 1.0;
	
	ofColor color = Gui_Global->DMX_test__Color;
	LIGHT_COLOR LightColor = LIGHT_COLOR(color.r / 255.0, color.g / 255.0, color.b / 255.0, 0.0);
	
	if(Gui_Global->DMX_test_Main)	LightParam[ int(DEVICE::Main) ].set_target(LightColor, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	else							LightParam[ int(DEVICE::Main) ].set_target(col_Black, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	
	if(Gui_Global->DMX_test_Back)	LightParam[ int(DEVICE::Back) ].set_target(LightColor, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	else							LightParam[ int(DEVICE::Back) ].set_target(col_Black, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	
	if(Gui_Global->DMX_test_L)		LightParam[ int(DEVICE::Moving_L) ].set_target(LightColor, PAN_TILT(Gui_Global->DMX_test__Pan_L, Gui_Global->DMX_test__Tilt_L, 0.0), d_Color);
	else							LightParam[ int(DEVICE::Moving_L) ].set_target(col_Black, PAN_TILT(Gui_Global->DMX_test__Pan_L, Gui_Global->DMX_test__Tilt_L, 0.0), d_Color);
	
	if(Gui_Global->DMX_test_R)		LightParam[ int(DEVICE::Moving_R) ].set_target(LightColor, PAN_TILT(Gui_Global->DMX_test__Pan_R, Gui_Global->DMX_test__Tilt_R, 0.0), d_Color);
	else							LightParam[ int(DEVICE::Moving_R) ].set_target(col_Black, PAN_TILT(Gui_Global->DMX_test__Pan_R, Gui_Global->DMX_test__Tilt_R, 0.0), d_Color);
}

/******************************
******************************/
void DMX_SYSTEM::update_Param__InitialImmediate(){
	float d_Color = 1.0;
	
	ofColor color(0, 0, 0);
	LightParam[ int(DEVICE::Main) ].set_target(col_Black, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	LightParam[ int(DEVICE::Back) ].set_target(col_Black, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	
	LightParam[ int(DEVICE::Moving_L) ].set_target(col_Black, PAN_TILT(Gui_Global->DMX_L__Pan_init, Gui_Global->DMX_L__Tilt_init, 0.0), d_Color);
	LightParam[ int(DEVICE::Moving_R) ].set_target(col_Black, PAN_TILT(Gui_Global->DMX_R__Pan_init, Gui_Global->DMX_R__Tilt_init, 0.0), d_Color);
}
/******************************
******************************/
void DMX_SYSTEM::update_Param__InitialFade(){
	float d_Color = 0.01;
	
	LightParam[ int(DEVICE::Main) ].set_target(col_Black, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	LightParam[ int(DEVICE::Back) ].set_target(col_Black, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	
	LightParam[ int(DEVICE::Moving_L) ].set_target(col_Black, PAN_TILT(Gui_Global->DMX_L__Pan_init, Gui_Global->DMX_L__Tilt_init, 0.0), d_Color);
	LightParam[ int(DEVICE::Moving_R) ].set_target(col_Black, PAN_TILT(Gui_Global->DMX_R__Pan_init, Gui_Global->DMX_R__Tilt_init, 0.0), d_Color);
}

/******************************
******************************/
void DMX_SYSTEM::update_Param__Kime(){
	float d_Color = 0.02;
	
	LightParam[ int(DEVICE::Main) ].set_target(col_StrongWhite, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	LightParam[ int(DEVICE::Back) ].set_target(col_StrongWhite, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	
	LightParam[ int(DEVICE::Moving_L) ].set_target(col_StrongWhite, PAN_TILT(Gui_Global->DMX_L__Pan_Kime, Gui_Global->DMX_L__Tilt_Kime, Gui_Global->DMX_Kime_Speed), d_Color);
	LightParam[ int(DEVICE::Moving_R) ].set_target(col_StrongWhite, PAN_TILT(Gui_Global->DMX_R__Pan_Kime, Gui_Global->DMX_R__Tilt_Kime, Gui_Global->DMX_Kime_Speed), d_Color);
}

/******************************
******************************/
void DMX_SYSTEM::update_Param__waitKime(){
	float d_Color = 1.0;
	
	LightParam[ int(DEVICE::Main) ].set_target(col_Black, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	LightParam[ int(DEVICE::Back) ].set_target(col_Black, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	
	LightParam[ int(DEVICE::Moving_L) ].set_target(col_Black, PAN_TILT(Gui_Global->DMX_L__Pan_Kime, Gui_Global->DMX_L__Tilt_Kime, Gui_Global->DMX_Kime_Speed), d_Color);
	LightParam[ int(DEVICE::Moving_R) ].set_target(col_Black, PAN_TILT(Gui_Global->DMX_R__Pan_Kime, Gui_Global->DMX_R__Tilt_Kime, Gui_Global->DMX_Kime_Speed), d_Color);
}

/******************************
******************************/
void DMX_SYSTEM::update_Param__Figure8(int now){
	float d_Color = 0.1;
	
	LightParam[ int(DEVICE::Main) ].set_target(col_Black, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	LightParam[ int(DEVICE::Back) ].set_target(col_White, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	
	PAN_TILT PanTilt_L = cal_PanTilt_Figure8(now, Gui_Global->DMX_L__Pan_min, Gui_Global->DMX_L__Pan_max, Gui_Global->DMX_L__Tilt_min, Gui_Global->DMX_L__Tilt_max, Gui_Global->DMX_L__Tilt_Figure8_ofs, Gui_Global->DMX_L__Figure8_Pan_Reverse ? -1 : 1, Gui_Global->DMX_L__Figure8_Tilt_Reverse ? -1 : 1);
	PAN_TILT PanTilt_R = cal_PanTilt_Figure8(now, Gui_Global->DMX_R__Pan_min, Gui_Global->DMX_R__Pan_max, Gui_Global->DMX_R__Tilt_min, Gui_Global->DMX_R__Tilt_max, Gui_Global->DMX_R__Tilt_Figure8_ofs, Gui_Global->DMX_R__Figure8_Pan_Reverse ? -1 : 1, Gui_Global->DMX_R__Figure8_Tilt_Reverse ? -1 : 1);
	LightParam[ int(DEVICE::Moving_L) ].set_target(col_White, PanTilt_L, d_Color);
	LightParam[ int(DEVICE::Moving_R) ].set_target(col_White, PanTilt_R, d_Color);
}

/******************************
******************************/
void DMX_SYSTEM::update_Param__Figure8_color(int now){
	float d_Color = 1.0;
	
	ofColor color = Gui_Global->DMX_Figure8__Color;
	LIGHT_COLOR LightColor = LIGHT_COLOR(color.r / 255.0, color.g / 255.0, color.b / 255.0, 0.0);
	LightParam[ int(DEVICE::Main) ].set_target(col_Black, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	LightParam[ int(DEVICE::Back) ].set_target(LightColor, PAN_TILT(0.0, 0.0, 0.0), d_Color);
	
	PAN_TILT PanTilt_L = cal_PanTilt_Figure8(now, Gui_Global->DMX_L__Pan_min, Gui_Global->DMX_L__Pan_max, Gui_Global->DMX_L__Tilt_min, Gui_Global->DMX_L__Tilt_max, Gui_Global->DMX_L__Tilt_Figure8_ofs, Gui_Global->DMX_L__Figure8_Pan_Reverse ? -1 : 1, Gui_Global->DMX_L__Figure8_Tilt_Reverse ? -1 : 1);
	PAN_TILT PanTilt_R = cal_PanTilt_Figure8(now, Gui_Global->DMX_R__Pan_min, Gui_Global->DMX_R__Pan_max, Gui_Global->DMX_R__Tilt_min, Gui_Global->DMX_R__Tilt_max, Gui_Global->DMX_R__Tilt_Figure8_ofs, Gui_Global->DMX_R__Figure8_Pan_Reverse ? -1 : 1, Gui_Global->DMX_R__Figure8_Tilt_Reverse ? -1 : 1);
	LightParam[ int(DEVICE::Moving_L) ].set_target(LightColor, PanTilt_L, d_Color);
	LightParam[ int(DEVICE::Moving_R) ].set_target(LightColor, PanTilt_R, d_Color);
}

/******************************
******************************/
void DMX_SYSTEM::update_Param__Coloring(int now){
	/********************
	********************/
	if(Gui_Global->DMX_T__Coloring < now - t_Coloring_LastUpdate){
		if(c_Coloring_update % 2 == 0)	Coloring_invert_order();
		else							Coloring_rotate_order();
		
		t_Coloring_LastUpdate = now;
		c_Coloring_update++;
	}
	
	/********************
	********************/
	float d_Color = 0.05;
	
	LightParam[ int(DEVICE::Main) ].set_target(col_Performance[Order_ColPerformance[0]], PAN_TILT(0.0, 0.0, 0.0), d_Color);
	LightParam[ int(DEVICE::Back) ].set_target(col_Performance[Order_ColPerformance[1]], PAN_TILT(0.0, 0.0, 0.0), d_Color);
	
	LightParam[ int(DEVICE::Moving_L) ].set_target(col_Performance[Order_ColPerformance[2]], PAN_TILT(Gui_Global->DMX_L__Pan_Kime, Gui_Global->DMX_L__Tilt_Kime, 0.0), d_Color);
	LightParam[ int(DEVICE::Moving_R) ].set_target(col_Performance[Order_ColPerformance[3]], PAN_TILT(Gui_Global->DMX_R__Pan_Kime, Gui_Global->DMX_R__Tilt_Kime, 0.0), d_Color);	
}

/******************************
******************************/
void DMX_SYSTEM::Coloring_invert_order(){
	swap(Order_ColPerformance[0], Order_ColPerformance[2]);
	swap(Order_ColPerformance[1], Order_ColPerformance[3]);
}

/******************************
******************************/
void DMX_SYSTEM::swap(int& val_0, int& val_1){
	int temp = val_1;
	val_1 = val_0;
	val_0 = temp;
}

/******************************
******************************/
void DMX_SYSTEM::Coloring_rotate_order(){
	int temp = Order_ColPerformance[3];
	for(int i = 3; 0 < i; i--){
		Order_ColPerformance[i] = Order_ColPerformance[i - 1];
	}
	Order_ColPerformance[0] = temp;
}

/******************************
******************************/
PAN_TILT DMX_SYSTEM::cal_PanTilt_Figure8(int now, float Pan_min, float Pan_max, float Tilt_min, float Tilt_max, float theta_ofs, float sgn_p, float sgn_t){
	/********************
	********************/
	if(sgn_p < 0)	sgn_p = -1;
	else			sgn_p = 1;
	
	if(sgn_t < 0)	sgn_t = -1;
	else			sgn_t = 1;
	
	/********************
	********************/
	float T = Gui_Global->DMX_T__Figure8;
	// float t = ofGetElapsedTimef();
	float t = now;
	float t_ofs = toRad(theta_ofs) / (2 * _PI) * T;
	
	float _pan	= sgn_p * sin(2 * _PI * t / (2 * T));
	float _tilt	= sgn_t * sin(2 * _PI * (t + t_ofs) / T);
	
	float pan	= ofMap(_pan, -1, 1, Pan_min, Pan_max, true);
	float tilt	= ofMap(_tilt, -1, 1, Tilt_min, Tilt_max, true);
	
	PAN_TILT PanTilt(pan, tilt, 0.0);
	return PanTilt;
}

/******************************
******************************/
void DMX_SYSTEM::update_DmxDataArray(){
	for(int i = 0; i < int(DEVICE::NUM_DEVICES); i++){
		int ofs = LightParam[i].get_ofs_DmxData();
		
		if(LightParam[i].get_DeviceType() == LIGHT_PARAM::DEVICE_TYPE::Fixed){
			DmxData[ofs + 0] = 255; // dimder
			DmxData[ofs + 1] = int(LightParam[i].get_LighgColor().get_r() * 255);
			DmxData[ofs + 2] = int(LightParam[i].get_LighgColor().get_g() * 255);
			DmxData[ofs + 3] = int(LightParam[i].get_LighgColor().get_b() * 255);
			DmxData[ofs + 4] = int(LightParam[i].get_LighgColor().get_w() * 255);
			DmxData[ofs + 5] = 1; // strobe : open
		}else{
			DmxData[ofs + 0]	= (LightParam[i].get_PanTilt().get_Pan() >> 8) & 0xFF;	// Pan H
			DmxData[ofs + 1]	= (LightParam[i].get_PanTilt().get_Pan() >> 0) & 0xFF;	// Pan L
			DmxData[ofs + 2]	= (LightParam[i].get_PanTilt().get_Tilt() >> 8) & 0xFF;	// Tilt H
			DmxData[ofs + 3]	= (LightParam[i].get_PanTilt().get_Tilt() >> 0) & 0xFF;	// Tilt L
			DmxData[ofs + 4]	= int(LightParam[i].get_LighgColor().get_r() * 255);	// R
			DmxData[ofs + 5]	= int(LightParam[i].get_LighgColor().get_g() * 255);	// G
			DmxData[ofs + 6]	= int(LightParam[i].get_LighgColor().get_b() * 255);	// B
			DmxData[ofs + 7]	= int(LightParam[i].get_LighgColor().get_w() * 255);	// W
			DmxData[ofs + 8]	= 8;													// strobe : 8 = open
			DmxData[ofs + 9]	= 255;													// dimmder
			DmxData[ofs + 10]	= LightParam[i].get_PanTilt().get_Speed();				// Pan/ Tilt 動作速度 : 速 = 0 - 255 = 遅
			DmxData[ofs + 11]	= 0;
			DmxData[ofs + 12]	= 0;													// dimmer curve : 0 = standard.
			
		}
	}
}

/******************************
******************************/
void DMX_SYSTEM::SendDmx(){
	ofxArtnetMessage message;
	message.setData(DmxData, DMX_SIZE);
	artnet.sendArtnet(message);
}

/******************************
******************************/
void DMX_SYSTEM::set_State(int now, STATE _State){
	State = _State;
	
	if(State == STATE::Coloring){
		t_Coloring_LastUpdate = now;
		c_Coloring_update = 0;
	}

}
