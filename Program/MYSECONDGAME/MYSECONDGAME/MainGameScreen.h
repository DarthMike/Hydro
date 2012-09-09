/*
	Filename: MainGameScreen.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Implementation of a screen in game - MAINGAMESCREEN
	Comments: Dependant of IndieLib Graphics library - derived from abtract class "GameScreen"
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _MAINGAMESCREEN
#define _MAINGAMESCREEN

//Library dependencies

//Class dependencies
#include "General_Resources.h"
#include "GameScreen.h"

//Forward Declarations
class PhysicsSim;	//Game Logic class
class GameOverlay;	//Game UI class

class MainGameScreen : public GameScreen
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	MainGameScreen()
		:GameScreen(),
		mMainGame(NULL),
		mOverlay(NULL)
	{
		_init();
		SingletonLogMgr::Instance()->AddNewLine("MainGameScreen","MainGameScreen scren loaded",LOGDEBUG);
	}
	virtual ~MainGameScreen()
	{
		_release();
		SingletonLogMgr::Instance()->AddNewLine("MainGameScreen","MainGameScreen screen released",LOGDEBUG);
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
	PhysicsSim* mMainGame;
	GameOverlay* mOverlay;
	//----- INTERNAL FUNCTIONS -----
	void _init();//Internal init
	void _release(); //Internal release
};

#endif