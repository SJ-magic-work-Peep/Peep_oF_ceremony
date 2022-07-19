/************************************************************
************************************************************/
#include "Shutter.h"

/************************************************************
************************************************************/

/******************************
******************************/
SHUTTER::SHUTTER(){
}

/******************************
******************************/
SHUTTER::~SHUTTER(){
}

/******************************
******************************/
void SHUTTER::setup(){
	setup_udp();
}

/******************************
******************************/
void SHUTTER::setup_udp(){
	/********************
	********************/
	{
		ofxUDPSettings settings;
		// settings.sendTo("127.0.0.1", 12345);
		settings.sendTo("10.0.0.8", 12345);
		settings.blocking = false;
		
		udp_Send.Setup(settings);
	}
	
	/********************
	********************/
	{
		ofxUDPSettings settings;
		settings.receiveOn(12347);
		settings.blocking = false;
		
		udp_Receive.Setup(settings);
	}
}

/******************************
******************************/
void SHUTTER::update(int now){
	/********************
	********************/
	char udpMessage[UDP_BUF_SIZE];
	udp_Receive.Receive(udpMessage, UDP_BUF_SIZE);
	string message=udpMessage;
	
	if(message!=""){
		int counter = 0;
		do{
			/********************
			********************/
			// printf("%s\n", message.c_str());
			// printf("> %d th message\n", counter);
			counter++;
			
			vector<string> strMessages = ofSplitString(message, "[p]");
			/*
			for(int i = 0; i < strMessages.size(); i++){
				printf("%s\n", strMessages[i].c_str());
			}
			*/
			
			if(2 <= strMessages.size()){
				if( (strMessages[0] == "PrjShutter") && (strMessages[1] == "Beat") )	BeatMonitor.beat(now);
			}
			
			/********************
			********************/
			udp_Receive.Receive(udpMessage, UDP_BUF_SIZE);
			message=udpMessage;
		}while(message!="");
	}
}

/******************************
******************************/
bool SHUTTER::isAlive(int now){
	return BeatMonitor.isAlive(now);
}

/******************************
******************************/
void SHUTTER::sendUdpTo_PrjShutter__open(){
	sendUdpTo_PrjShutter__setAngle( int(Gui_Global->Shutter_Angle_Open) );
}

/******************************
******************************/
void SHUTTER::sendUdpTo_PrjShutter__close(){
	sendUdpTo_PrjShutter__setAngle( int(Gui_Global->Shutter_Angle_Close) );
}

/******************************
******************************/
void SHUTTER::sendUdpTo_PrjShutter__setAngle(int angle){
	char buf[100];
	sprintf(buf, "%d", int(angle));
	
	string message="";
	message += "oF|set_Angle|";
	message += buf;
	
	udp_Send.Send(message.c_str(),message.length());
}


