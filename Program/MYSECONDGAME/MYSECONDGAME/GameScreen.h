/*
	Filename: GameScreen.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Abstract class to define a screen of game
	             A game screen only contains graphical data to portray and show. It also will execute the UI
				 code necessary in very game state (or screen in this case), as it can be different for every
				 screen. Pure game logic is done in another classes
	Comments: Dependant of IndieLib Graphics library
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
#ifndef _GAMESCREEN
#define _GAMESCREEN

//Library dependencies

//Class dependencies
#include "General_Resources.h"
#include "GameEvents.h"

class GameScreen
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameScreen():mExit(false)
	{
		SingletonLogMgr::Instance()->AddNewLine("GameScreen","New game screen created",LOGNORMAL);
	}
	virtual ~GameScreen()
	{
		SingletonLogMgr::Instance()->AddNewLine("GameScreen","Game Screen deleted",LOGNORMAL);
	}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	virtual void ExecuteEnter() = 0; //When entering screen
	virtual void ExecuteLogicUpdate(float dt) = 0; //Update screen logic
	virtual void ExecuteRender() = 0; //Render contents of screen (if necesary)
	virtual void ExecuteExit() = 0; //When exiting screen
	virtual bool HandleEvent(const EventData&)=0;						//Process possible events
	virtual GameState IsExit() { return NOSTATE; } //Exit conditions from UI - Exit to another game state
	//----- PUBLIC VARIABLES ------

protected:
	//----- INTERNAL VARIABLES -----
	//Tracking of exit condition
	bool mExit;
	//----- INTERNAL FUNCTIONS -----
};

#endif