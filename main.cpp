/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofApp.h"
#include "Monitor.h"
#include <ofAppGLFWWindow.h>

/************************************************************
************************************************************/

/******************************
******************************/
int main( int argc, char** argv ){
	/********************
	comment out for multi-window
	********************/
	// ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context
	// ofRunApp(new ofApp());
	
	/********************
	C++11スマートポインタ入門
		https://qiita.com/hmito/items/db3b14917120b285112f
	********************/
	ofGLFWWindowSettings settings;
	
    settings.setSize(600, 600); // temporary.
	settings.setPosition(glm::vec2(0,0));
	settings.resizable = true;
	// settings.shareContextWith = secondWindow;
	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);
	
    settings.setSize(600, 600); // temporary
	settings.setPosition(glm::vec2(400,200));
	settings.resizable = true;
	settings.shareContextWith = mainWindow; // texure(Graphic memory)領域は、mainWindowと共有する : この設定がないと、fboなど、texture関連memoryの やり取りができない。
	shared_ptr<ofAppBaseWindow> secondWindow = ofCreateWindow(settings);
	
	shared_ptr<ofApp> mainApp( new ofApp() );
	shared_ptr<MONITOR> MonitorApp( new MONITOR );
	MonitorApp->main_obj = mainApp;
	
	ofRunApp(mainWindow, mainApp);
	ofRunApp(secondWindow, MonitorApp);
	
	ofRunMainLoop();
}
