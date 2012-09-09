/*
	Filename: GameStateManager.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to states inside game (credits, intro, maingame...)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "GameStateManager.h"
#include "GameStateListener.h"
#include "GameScreen.h"
#include "IntroScreen.h"
#include "MainGameScreen.h"
#include "EditorScreen.h"
#include "MainMenuScreen.h"
#include "CreditsScreen.h"
#include "ControlsScreen.h"

//Init state
void GameStateManager::Init()		
{	
	GameScreen* screen (NULL);
	
	//IF - Normal game mode
	if(!mEditMode)
	{
		mState = INTRO;  //Initial state
		//Setup state - INTRO
		screen = new IntroScreen();
	}
	else //ELSE - Editor mode
	{
		mState = EDITOR;	//Editor state
		//Setup state -	EDITOR
		screen = new EditorScreen();
	}
	
	//Update variables
	mScreensMap[mState] = screen;
	mScreensMap[mState]->ExecuteEnter();
	mPreviousState = mState;
}
//Render screen contents depending on state
void GameStateManager::Render()		
{
	//Only render if a state is fixed
	if(mState == mPreviousState)	
	{
		//Render corresponding screen
		mScreensMap[mState]->ExecuteRender();
	}	
}

//Update game logic depending on state
void GameStateManager::UpdateLogic(float dt)
{	
	//Update Logic
	switch(mState)
	{
	case(EDITOR):
		{
			//-------Enter action of state---------
			//In Editor Mode no other states are allowed
			assert(mState == mPreviousState);
			//-------Logic for this state----------		
			//Execute screen
			mScreensMap[EDITOR]->ExecuteLogicUpdate(dt);

			//Check exit conditions from screen
			GameState nextstate = mScreensMap[EDITOR]->IsExit();
			//Change of state: BY SCREEN
			if(
				nextstate != NOSTATE
				)
			{
				mScreensMap[mState]->ExecuteExit();
				//Delete game screen (and game)
				delete 	mScreensMap[mState];
				mScreensMap.erase(mState);
				mState = nextstate;
				mPreviousState = EDITOR;
			}
		}
		break;
	case(INTRO): 
		{
			//-------Enter action of state---------
			//Done in init of class
			//-------Logic for this state----------
			//Execute screen
			mScreensMap[INTRO]->ExecuteLogicUpdate(dt);
			GameState nextstate = mScreensMap[INTRO]->IsExit();
			//-------Change of state:--------------
			if(
				nextstate != NOSTATE
				)
			{
				mScreensMap[mState]->ExecuteExit();
				delete 	mScreensMap[mState];
				mScreensMap.erase(mState);
				mState = nextstate;
				mPreviousState = INTRO;
			}
		}
		break;
	case(MAINMENU):
		{
			//-------Enter action of state---------
			if(mState != mPreviousState)
			{
				//Setup state - MAINMENU
				//Create main menu screen
				GameScreen* screen = new MainMenuScreen();
				mScreensMap[MAINMENU] = screen;
				mScreensMap[MAINMENU]->ExecuteEnter();
				mPreviousState = mState;
			}
			//-------Logic for this state----------
			//Execute screen
			mScreensMap[MAINMENU]->ExecuteLogicUpdate(dt);
			GameState nextstate = mScreensMap[MAINMENU]->IsExit();
			//-------Change of state:--------------
			if(
				nextstate != NOSTATE
				)
			{
				//Delete screen
				mScreensMap[mState]->ExecuteExit();
				delete 	mScreensMap[mState];
				mScreensMap.erase(mState);
				mState = nextstate;
				mPreviousState = MAINMENU;
			}
		}
			break;
	case(NORMALGAME):
		{
			//-------Enter action of state---------
			if(mState != mPreviousState)
			{
				//Setup state - NORMAL GAME
				//IF - Screen wasnt created previously
				//Create game screen (and game)
				GameScreen* screen = new MainGameScreen();
				mScreensMap[NORMALGAME] = screen;
				mScreensMap[NORMALGAME]->ExecuteEnter();
				mPreviousState = mState;
			}
			//-------Logic for this state----------		
			//Execute screen
			mScreensMap[NORMALGAME]->ExecuteLogicUpdate(dt);

			//Check exit conditions from screen
			GameState nextstate = mScreensMap[NORMALGAME]->IsExit();
			//Change of state: BY SCREEN
			if(
				nextstate != NOSTATE
				)
			{
				mScreensMap[mState]->ExecuteExit();
				//Delete game screen (and game)
				delete 	mScreensMap[mState];
				mScreensMap.erase(mState);
				mState = nextstate;
				mPreviousState = NORMALGAME;
			}
		}
		break;
	case(CREDITS):
		{
			//-------Enter action of state---------
			if(mState != mPreviousState)
			{
				//Setup state - CREDITS
				//Create main menu screen
				GameScreen* screen = new CreditsScreen();
				mScreensMap[CREDITS] = screen;
				mScreensMap[CREDITS]->ExecuteEnter();
				mPreviousState = mState;
			}
			//-------Logic for this state----------
			//Execute screen
			mScreensMap[CREDITS]->ExecuteLogicUpdate(dt);
			GameState nextstate = mScreensMap[CREDITS]->IsExit();
			//-------Change of state:--------------
			if(
				nextstate != NOSTATE
				)
			{
				//Delete screen
				mScreensMap[mState]->ExecuteExit();
				delete 	mScreensMap[mState];
				mScreensMap.erase(mState);
				mState = nextstate;
				mPreviousState = CREDITS;
			}
		}
		break;
	case(CONTROLS):
		{
			//-------Enter action of state---------
			if(mState != mPreviousState)
			{
				//Setup state - CREDITS
				//Create main menu screen
				GameScreen* screen = new ControlsScreen();
				mScreensMap[CONTROLS] = screen;
				mScreensMap[CONTROLS]->ExecuteEnter();
				mPreviousState = mState;
			}
			//-------Logic for this state----------
			//Execute screen
			mScreensMap[CONTROLS]->ExecuteLogicUpdate(dt);
			GameState nextstate = mScreensMap[CONTROLS]->IsExit();
			//-------Change of state:--------------
			if(
				nextstate != NOSTATE
				)
			{
				//Delete screen
				mScreensMap[mState]->ExecuteExit();
				delete 	mScreensMap[mState];
				mScreensMap.erase(mState);
				mState = nextstate;
				mPreviousState = CONTROLS;
			}
		}
		break;
	case(EXIT):
		{
			//Signal application end
			mExit = true;
			SingletonLogMgr::Instance()->AddNewLine("GameStateManager::UpdateLogic","User quits game...",LOGNORMAL);
			//Just wait for app to end
		}
	default:
		break;
	}
}

//Init resources
void GameStateManager::_init()
{
	//Register event listener
	if(!mListener)
		mListener = new GameStateListener(this);
}

//Release resources
void GameStateManager::_release()
{
		GameScreenMap::iterator it = mScreensMap.begin();
		while(it != mScreensMap.end())
		{
			if((*it).second)
				delete (*it).second;
			it++;
		}
		mScreensMap.clear();

		//Delete event listener
		if(mListener)
		{
			delete mListener;
			mListener = NULL;
		}
}

//Event Handling
bool GameStateManager::_handleEvents(const EventData& theevent)
{
	//IF - Some screen executing
	if(mState != EXIT)
	{
		//Forward event to actual active screen
		if(mScreensMap.find(mState) != mScreensMap.end())
			return (mScreensMap[mState]->HandleEvent(theevent));	 //Process by screen
		else
			return false;  //Processed, no active screen
	}//ELSE - No screen executing
	else
	{
		return false;
	}//IF
}