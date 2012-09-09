/*
	Filename: IntroScreen.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Implementation of a screen in game - INTROSCREEN
	Comments: Dependant of IndieLib Graphics library - derived from abtract class "GameScreen"
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _INTROSCREEN
#define _INTROSCREEN

//Library dependencies

//Class dependencies
#include "General_Resources.h"
#include "Shared_Resources.h"
#include "GameScreen.h"

class IntroScreen : public GameScreen
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	IntroScreen()
		:GameScreen()
	{
		_init();
		SingletonLogMgr::Instance()->AddNewLine("IntroScreen","IntroScreen scren loaded",LOGDEBUG);
	}
	virtual ~IntroScreen()
	{
		_release();
		SingletonLogMgr::Instance()->AddNewLine("IntroScreen","IntroScreen screen released",LOGDEBUG);
	}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	//Implementations from base class
	virtual void ExecuteEnter();
	virtual void ExecuteLogicUpdate(float dt);
	virtual void ExecuteRender();
	virtual void ExecuteExit();
	bool HandleEvent(const EventData&);						//Process possible events
	virtual GameState IsExit();
	//----- PUBLIC VARIABLES ------

protected:
	//----- INTERNAL VARIABLES -----
	GameLevelPointer mScreenAssets;
	static const std::string mScreenAssetsFileName;
	//----- INTERNAL FUNCTIONS -----
	void _init();//Internal init
	void _release(); //Internal release
};

#endif