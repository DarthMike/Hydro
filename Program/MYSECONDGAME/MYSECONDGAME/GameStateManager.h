/*
	Filename: GameStateManager.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to states inside game (credits, intro, maingame...)
	Comments: A Game state is associated to one screen, but many game states can have the same screen. 
	          In this class, the state machine of the main game app is defined and executed. its up to the
			  container screens to tell this manager which state will be next.
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
#ifndef _GAMESTATEMANAGER
#define _GAMESTATEMANAGER

//Library dependencies

//Class dependencies
#include "General_Resources.h"

//Forward declarations
class GameScreen;
class EventData;
class GameStateListener;

class GameStateManager
{
	//Event listener class is friend to this (internal callbacks)
	friend class GameStateListener;
protected:
	// A map of screen object with name of screen
	typedef std::map<GameState,GameScreen*> GameScreenMap;
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameStateManager(bool editmode):
	mExit(false),
	mListener(NULL),
	mEditMode(editmode),
	mState(NOSTATE),
	mPreviousState(NOSTATE)
	{
		_init();
	}
	~GameStateManager()
	{		
		_release();
	}
	//----- GET/SET FUNCTIONS -----

	//----- OTHER FUNCTIONS -----
	void Init();						//Init state
	void Render();						//Render screen contents depending on state
	void UpdateLogic(float dt);			//Update game logic depending on state
	bool Exit() {return mExit;};		//Is game finished?
	//----- PUBLIC VARIABLES ------
protected:
	//----- INTERNAL VARIABLES -----
	GameStateListener* mListener;			//Event Listener class

	GameState mState;						//The game state
	GameState mPreviousState;				//The previous game state
	bool mEditMode;							//Tracking if in edit mode
	bool mExit;								//Is necessary to exit?
	GameScreenMap mScreensMap;	//Screens dynamic container
	//----- INTERNAL FUNCTIONS -----
	void _init();			//Init resources
	void _release();		//Release resources
	bool _handleEvents(const EventData& theevent); //Event Handling
};

#endif