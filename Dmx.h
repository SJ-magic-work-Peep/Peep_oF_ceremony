/************************************************************
■概要
	ofxArtnetが "comletely different" なモノとして更新されていたのでtest
		https://github.com/hiroyuki/ofxArtnet
	
■細かい設定
	subnet, universe Idは、ofxArtnetMessage::setUniverse() からsetできる
	その他、Addonのsource codeを読めば色々わかる : 難しくないので読んでみること.

■Ch設定
	soft		: 0-5	/ 6-11	/ 12-17
	HardOffset	: 1		/ 7		/ 13
	
■dependency
	ofxNetwork 

■メモ
	・	Firewallをoffにしなくても動作した(old versionはoffにする必要があった)
************************************************************/
#pragma once

// #define ARTNET_THREADED

/************************************************************
************************************************************/
#include "ofxArtnet.h"
#include "sj_common.h"

/************************************************************
************************************************************/

/**************************************************
**************************************************/
class LIGHT_COLOR{
public:
	LIGHT_COLOR() : r(0), g(0), b(0), w(0) { }
	LIGHT_COLOR(float _r, float _g, float _b, float _w) : r(_r), g(_g), b(_b), w(_w) { }
	
	const LIGHT_COLOR& operator=(const LIGHT_COLOR& org){
		r = org.r;
		g = org.g;
		b = org.b;
		w = org.w;
		return *this;
	}
	
	void Fade(const LIGHT_COLOR& target, float dColor){
		r = do_Fade(r, target.r, dColor);
		g = do_Fade(g, target.g, dColor);
		b = do_Fade(b, target.b, dColor);
		w = do_Fade(w, target.w, dColor);
	}
	
private:
	float r;
	float g;
	float b;
	float w;
	
	float limit(float val){
		if(val < 0)			return 0;
		else if(1 < val)	return 1;
		else				return val;
	}
	
	float _sgn(float val){
		if(val < 0) return -1;
		else		return 1;
	}
	float do_Fade(float current, const float target, float dColor){
		float diff = target - current;
		if(abs(diff) < dColor){
			return target;
		}else{
			float val = current + _sgn(diff) * dColor;
			return limit(val);
		}
	}
	
public:
	void set_r(float _val)	{ float val = limit(_val); r = val; }
	void set_g(float _val)	{ float val = limit(_val); g = val; }
	void set_b(float _val)	{ float val = limit(_val); b = val; }
	void set_w(float _val)	{ float val = limit(_val); w = val; }
	
	float get_r()			{ return r; }
	float get_g()			{ return g; }
	float get_b()			{ return b; }
	float get_w()			{ return w; }
};

/**************************************************
**************************************************/
class PAN_TILT{
public:
	PAN_TILT() : Pan(0), Tilt(0), Speed(0) { }
	PAN_TILT(int _Pan, int _Tilt, int _Speed) : Pan(_Pan), Tilt(_Tilt), Speed(_Speed) { }
	
	const PAN_TILT& operator=(const PAN_TILT& org){
		Pan = org.Pan;
		Tilt = org.Tilt;
		Speed = org.Speed;
		return *this;
	}
	
private:
	enum{
		VAL_MAX = 65535,
	};
	
	int Pan, Tilt;
	int Speed; // 速 = 0 - 255 = 遅
	
	int limit(int val, int MAX){
		if(0 < val)				return 0;
		else if(MAX < val)		return MAX;
		else					return val;
	}
	
public:
	void set_Pan(int _val)		{ int val = limit(_val, VAL_MAX);	Pan = val; }
	void set_Tilt(int _val)		{ int val = limit(_val, VAL_MAX);	Tilt = val; }
	void set_Speed(int _val)	{ int val = limit(_val, 255);		Speed = val; }
	
	int get_Pan()			{ return Pan; }
	int get_Tilt()			{ return Tilt; }
	int get_Speed()			{ return Speed; }
};

/**************************************************
**************************************************/
class LIGHT_PARAM{
public:
	enum class DEVICE_TYPE{
		Fixed,
		Moving,
	};
	LIGHT_PARAM() { }
	~LIGHT_PARAM() { }
	
	
private:
	LIGHT_COLOR LightColor_target;
	LIGHT_COLOR LightColor_current;
	PAN_TILT PanTilt;
	float d_Color = 1.0;
	
	int ofs_DmxData = 0;
	DEVICE_TYPE DeviceType = DEVICE_TYPE::Fixed;
	
public:
	void setup(int _ofs_DmxData, DEVICE_TYPE _DeviceType){
		ofs_DmxData = _ofs_DmxData;
		DeviceType = _DeviceType;
	}
	
	void update_color(){
		LightColor_current.Fade(LightColor_target, d_Color);
	}
	
	void set_target(const LIGHT_COLOR& _col, const PAN_TILT& _PanTilt, const float _d_Color){
		LightColor_target = _col;
		PanTilt = _PanTilt;
		d_Color = _d_Color;
	}
	
	int get_ofs_DmxData()			{ return ofs_DmxData; }
	DEVICE_TYPE get_DeviceType()	{ return DeviceType; }
	
	LIGHT_COLOR get_LighgColor()	{ return LightColor_current; }
	PAN_TILT get_PanTilt()			{ return PanTilt; }
};

/**************************************************
**************************************************/
class DMX_SYSTEM{
public:
	/****************************************
	****************************************/
	enum class STATE{
		Test,
		InitialImmediate,
		InitialFade,
		Kime,
		wait_Kime,
		Figure8,
		Figure8_Color,
		Coloring,
	};
	
	DMX_SYSTEM();
	~DMX_SYSTEM();
	
private:
	/****************************************
	****************************************/
	enum{
		DMX_SIZE = 512,
	};
	enum class DEVICE{
		Main,
		Back,
		Moving_L,
		Moving_R,
		
		NUM_DEVICES,
	};
	
	/****************************************
	****************************************/
	ofxArtnetSender artnet;
	
	unsigned char DmxData[DMX_SIZE];
	
	STATE State = STATE::Test;
	LIGHT_PARAM LightParam[ int(DEVICE::NUM_DEVICES) ];
	
	const LIGHT_COLOR col_Black;
	const LIGHT_COLOR col_White;
	const LIGHT_COLOR col_SimpleWhite;
	const LIGHT_COLOR col_StrongWhite;
	
	LIGHT_COLOR col_Performance[4];
	int Order_ColPerformance[4];
	int t_Coloring_LastUpdate = 0;
	int c_Coloring_update = 0;
	
	/****************************************
	****************************************/
	void ClearDmxData();
	void update_Param(int now);
	void update_DmxDataArray();
	
	void update_Param__Test();
	void update_Param__InitialImmediate();
	void update_Param__InitialFade();
	void update_Param__Kime();
	void update_Param__waitKime();
	void update_Param__Figure8(int now);
	void update_Param__Figure8_color(int now);
	void update_Param__Coloring(int now);
	void Coloring_invert_order();
	void Coloring_rotate_order();
	void swap(int& val_0, int& val_1);
	
	PAN_TILT cal_PanTilt_Figure8(int now, float Pan_min, float Pan_max, float Tilt_min, float Tilt_max, float theta_ofs, float sgn_p, float sgn_t);
	
public:
	/****************************************
	****************************************/
	void setup();
	void update(int now);
	void SendDmx();
	void set_State(int now, STATE _State);
};

