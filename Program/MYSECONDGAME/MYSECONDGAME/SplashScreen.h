/*
	Filename: SplashScreen.h
	Author: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Implementation of a screen in game - SPLASHSCREEN
	Comments: Dependant of IndieLib Graphics library - derived from abtract class "GameScreen"
	Attribution: 
		License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _SPLASHSCREEN
#define _SPLASHSCREEEN

//Library dependencies

//Class dependencies
#include "GameScreen.h"

//Definitions
const float SPLASHSCREENTIME = 5000.0f;	//Time splash screen will be showing (in ms)

class SplashScreen : public GameScreen
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	SplashScreen(IndieLibManager* pIlib):GameScreen(pIlib),mScreenTime(0)
	{
		_init();	
		SingletonLogMgr::Instance()->AddNewLine("SplashScreen","Splash screen loaded",DEBUG);
	}
	virtual ~SplashScreen()
	{
		_release();
		SingletonLogMgr::Instance()->AddNewLine("SplashScreen","Splash screen released",DEBUG);
	}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	//Implementations from base class
	virtual void ExecuteEnter();
	virtual void ExecuteLogicUpdate(float dt);
	virtual void ExecuteRender();
	virtual void ExecuteExit();
	virtual GameState IsExit();
	//----- PUBLIC VARIABLES ------

protected:
	//----- INTERNAL VARIABLES -----
	float mScreenTime;
	//----- INTERNAL FUNCTIONS -----
	void _init();//Internal init
	void _release(); //Internal release
};

#endif