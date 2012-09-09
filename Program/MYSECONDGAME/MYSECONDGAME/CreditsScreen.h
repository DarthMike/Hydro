/*
	Filename: CreditsScreen.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Implementation of a screen in game - CREDITSSCREEN
	Comments: Dependant of IndieLib Graphics library - derived from abtract class "GameScreen"
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _CREDITSSCREEN
#define _CREDITSSCREEN

//Library dependencies

//Class dependencies
#include "General_Resources.h"
#include "Shared_Resources.h"
#include "GameScreen.h"

class CreditsScreen : public GameScreen
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	CreditsScreen()
		:GameScreen(),
		mCounter(0.0f),
		mTransition(false)
	{
		_init();
		SingletonLogMgr::Instance()->AddNewLine("CreditsScreen","CreditsScreen scren loaded",LOGDEBUG);
	}
	virtual ~CreditsScreen()
	{
		_release();
		SingletonLogMgr::Instance()->AddNewLine("CreditsScreen","CreditsScreen screen released",LOGDEBUG);
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
	float mCounter;
	bool mTransition;
	//----- INTERNAL FUNCTIONS -----
	void _init();//Internal init
	void _release(); //Internal release
	void _processInputs();
};

#endif