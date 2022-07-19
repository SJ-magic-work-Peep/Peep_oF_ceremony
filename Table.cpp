/************************************************************
************************************************************/
#include "Table.h"

/************************************************************
************************************************************/

/******************************
******************************/
TABLE::TABLE(){
}

/******************************
******************************/
TABLE::~TABLE(){
}

/******************************
******************************/
void TABLE::setup(){
	setup_udp();
}

/******************************
******************************/
void TABLE::setup_udp(){
	/********************
	********************/
	{
		ofxUDPSettings settings;
		// settings.sendTo("127.0.0.1", 12345);
		settings.sendTo("10.0.0.7", 12345);
		settings.blocking = false;
		
		udp_Send.Setup(settings);
	}
	
	/********************
	********************/
	{
		ofxUDPSettings settings;
		settings.receiveOn(12346);
		settings.blocking = false;
		
		udp_Receive.Setup(settings);
	}
}

/******************************
******************************/
void TABLE::update(int now){
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
			
			if(strMessages.size() == 2){
				if( (strMessages[0] == "Table") && (strMessages[1] == "Beat") )	BeatMonitor.beat(now);
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
bool TABLE::isAlive(int now){
	return BeatMonitor.isAlive(now);
}

/******************************
******************************/
void TABLE::sendUdpTo_Table__PushUp(){
	if(!Gui_Global->Table_ReverseDirection)	sendUdpTo_Table__set_dAngle(-int(Gui_Global->Table_dAngle_Up), int(Gui_Global->Table_PushTime));
	else									sendUdpTo_Table__set_dAngle( int(Gui_Global->Table_dAngle_Up), int(Gui_Global->Table_PushTime));
}

/******************************
******************************/
void TABLE::sendUdpTo_Table__PushDown(){
	if(!Gui_Global->Table_ReverseDirection)	sendUdpTo_Table__set_dAngle( int(Gui_Global->Table_dAngle_Down), int(Gui_Global->Table_PushTime));
	else									sendUdpTo_Table__set_dAngle(-int(Gui_Global->Table_dAngle_Down), int(Gui_Global->Table_PushTime));
}

/******************************
******************************/
void TABLE::sendUdpTo_Table__set_dAngle(int dAngle, int PushTime_ms){
	/********************
	********************/
	char buf[100];
	sprintf(buf, "%d|%d", dAngle, PushTime_ms);
	
	/********************
	********************/
	string message="";
	message += "oF|set_dAngle|";
	message += buf;
	
	udp_Send.Send(message.c_str(),message.length());
}
