/*
	Filename: MainMenuScreen.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Implementation of a screen in game - MAINMENUSCREEN
	Comments: Dependant of IndieLib Graphics library - derived from abtract class "GameScreen"
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _MAINMENUSCREEN
#define _MAINMENUSCREEN

//Library dependencies

//Class dependencies
#include "General_Resources.h"
#include "Shared_Resources.h"
#include "GameScreen.h"

class MainMenuScreen : public GameScreen
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	MainMenuScreen()
		:GameScreen(),
		 mStart(false),
		 mCredits(false),
		 mControls(false),
		 mFirstTransition(false),
		 mSecondTransition(false),
		 mCounter(0.0f)
	{
		_init();
		SingletonLogMgr::Instance()->AddNewLine("MainMenuScreen","MainMenuScreen scren loaded",LOGDEBUG);
	}
	virtual ~MainMenuScreen()
	{
		_release();
		SingletonLogMgr::Instance()->AddNewLine("MainMenuScreen","MainMenuScreen screen released",LOGDEBUG);
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
	bool mStart;	//Start game memory
	bool mCredits;	//Credits memory
	bool mControls;  //Controls selection memory
	float mCounter;	//Temporization memory
	bool mFirstTransition;	//Logic memories
	bool mSecondTransition;
	//----- INTERNAL FUNCTIONS -----
	void _init();//Internal init
	void _release(); //Internal release
	void _processInputs();//Process inputs in scren
};

#endif