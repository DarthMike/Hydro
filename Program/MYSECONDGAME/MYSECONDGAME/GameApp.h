/*
	Filename: GameApp.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate Game application functionality (startup and cleanup)
	Comments: 
	Attribution: Based in part of tutorial at gametuto.com - Javier López López (javilop.com)
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _GAMEAPP
#define _GAMEAPP

//Library dependencies

//Class dependencies
#include "General_Resources.h"
#include "GameStateManager.h"
#include "PrecissionTimer.h"

class GameApp
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameApp(bool gameeditor):
    mEditMode(gameeditor),
	mInitialized(false),
	mStateManager(NULL)
	{
	}
	virtual ~GameApp()
	{
		_release();
	}
	//----- GET/SET FUNCTIONS -----
	void Load() {  if(!mInitialized) { _init(); } };
	void Run();
	//----- OTHER FUNCTIONS -----
	//----- PUBLIC VARIABLES ------

protected:
	//----- INTERNAL VARIABLES -----
	bool mInitialized;
	bool mEditMode;								//Game Editor mode
	GameStateManager* mStateManager;			//Game state manager
	PrecissionTimer mMainTimer;					    //Main app timer
	static const unsigned int mLogicUpdateTicks;			//How many times per second we want to update logic
	static const unsigned int mMaxLogicTicksPerFrame;		//How many times we can update as max, logic per game loop
	//----- INTERNAL FUNCTIONS -----
	void _init();
	void _release();
};

#endif