/************************************************************
************************************************************/
#include "RotStage.h"

/************************************************************
************************************************************/

/******************************
******************************/
ROT_STAGE::ROT_STAGE(){
}

/******************************
******************************/
ROT_STAGE::~ROT_STAGE(){
	LogResult();
}

/******************************
******************************/
void ROT_STAGE::LogResult(){
	FILE* fp;
	{
		string str_FileName = "../../../data/Log_RotStage/" + get_str_date() + "__Success.csv";
		fp = fopen(str_FileName.c_str(), "w");
	}
	
	if(fp){
		fprintf(fp, "(c_Succeeded, c_OverRun, c_timeout) = (%d, %d, %d)\n", c_Succeeded, c_OverRun, c_timeout);
		fflush(stdout);
		
		fclose(fp);
	}
}

/******************************
******************************/
void ROT_STAGE::setup(){
	/********************
	********************/
	setup_udp();
}

/******************************
C初級：日付と時刻を取得する
	https://dianxnao.com/c%E5%88%9D%E7%B4%9A%EF%BC%9A%E6%97%A5%E4%BB%98%E3%81%A8%E6%99%82%E5%88%BB%E3%82%92%E5%8F%96%E5%BE%97%E3%81%99%E3%82%8B/
******************************/
string ROT_STAGE::get_str_date(){
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
	
	char buf[100];
	sprintf(buf, "%d_%d_%d__%d_%d", year, month, day, hour, minute);
	string str_date = buf;
	
	return str_date;
}

/******************************
******************************/
void ROT_STAGE::setup_udp(){
	/********************
	********************/
	{
		ofxUDPSettings settings;
		// settings.sendTo("127.0.0.1", 12345);
		settings.sendTo("10.0.0.6", 12345);
		settings.blocking = false;
		
		udp_Send.Setup(settings);
	}
	
	/********************
	********************/
	{
		ofxUDPSettings settings;
		settings.receiveOn(12345);
		settings.blocking = false;
		
		udp_Receive.Setup(settings);
	}
}

/******************************
******************************/
void ROT_STAGE::update(int now){
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
				if(strMessages[0] == "RotStage"){
					if(strMessages[1] == "Beat"){
						BeatMonitor.beat(now);
					}else if( strMessages[1] == "Meas" ){
						if(3 <= strMessages.size()){
							printf("MeasLev = %d\n", atoi(strMessages[2].c_str()));
							fflush(stdout);
						}
					}else if( strMessages[1] == "Succeeded" ){
						if(4 <= strMessages.size()){
							printf( "Succeeded : %d [ms], LastBlackLevs = %s\n", atoi(strMessages[2].c_str()), strMessages[3].c_str() );
							fflush(stdout);
						}
						if(State == STATE::wait_1R){
							State = STATE::StandBy;
							LastResult = RESULT::Succeed;
							c_Succeeded++;
						}
					}else if( strMessages[1] == "OverRun" ){
						if(3 <= strMessages.size()){
							printf("OverRun : %d [ms]\n", atoi(strMessages[2].c_str()));
							fflush(stdout);
						}
						if(State == STATE::wait_1R){
							State = STATE::StandBy;
							LastResult = RESULT::OverRun;
							c_OverRun++;
						}
					}
				}
			}
			
			/********************
			********************/
			udp_Receive.Receive(udpMessage, UDP_BUF_SIZE);
			message=udpMessage;
		}while(message!="");
	}
	
	/********************
	********************/
	check_timeout(now);
}

/******************************
******************************/
void ROT_STAGE::check_timeout(int now){
	if(State == STATE::wait_1R){
		if(60000 < now - t_from_1R){
			State = STATE::StandBy;
			LastResult = RESULT::Timeout;
			c_timeout++;
		}
	}
}

/******************************
******************************/
bool ROT_STAGE::isAlive(int now){
	return BeatMonitor.isAlive(now);
}

/******************************
******************************/
void ROT_STAGE::clearCounter_Succeed(){
	c_Succeeded = 0;
	c_OverRun = 0;
	c_timeout = 0;
}

/******************************
******************************/
void ROT_STAGE::sendUdpTo_RotStage__action(int now, int t_SafetyGuard){
	/********************
	********************/
	if(State == STATE::wait_1R)	return;
	
	/********************
	********************/
	char buf[100];
	sprintf( buf, "%d|%d", int(Gui_Global->RotStage_threshToDetectBlack), t_SafetyGuard );
	
	string message="";
	message += "oF|action|";
	message += buf;
	
	udp_Send.Send(message.c_str(),message.length());
	
	/********************
	********************/
	State = STATE::wait_1R;
	t_from_1R = now;
	LastResult = RESULT::None;
}

/******************************
******************************/
void ROT_STAGE::sendUdpTo_RotStage__AC_on(){
	sendUdpTo_RotStage("oF|AC_ON|0|0");
	
	State = STATE::StandBy;
	LastResult = RESULT::None;
}

/******************************
******************************/
void ROT_STAGE::sendUdpTo_RotStage__AC_off(){
	sendUdpTo_RotStage("oF|AC_OFF|0|0");
	
	State = STATE::StandBy;
	LastResult = RESULT::None;
}

/******************************
******************************/
void ROT_STAGE::sendUdpTo_RotStage__Meas(){
	sendUdpTo_RotStage("oF|Meas|0|0");
}

/******************************
******************************/
void ROT_STAGE::sendUdpTo_RotStage(string message){
	udp_Send.Send(message.c_str(),message.length());
}

/******************************
******************************/
ROT_STAGE::RESULT ROT_STAGE::get_LastResult(){
	return LastResult;
}

