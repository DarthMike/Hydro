/*
	Filename: MainGameScreen.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Implementation of a screen in game - MAINGAMESCREEN
	Comments: Dependant of IndieLib Graphics library - derived from abtract class "GameScreen"
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/


#include "MainGameScreen.h"
#include "PhysicsSim.h"
#include "GameOverlay.h"
#include "IndieLibManager.h"
#include "Camera2D.h"

//To execute on start
void MainGameScreen::ExecuteEnter()
{
	//Create game object
	if(!mMainGame)	
		mMainGame = new PhysicsSim();

	if(!mOverlay)
		//mOverlay = new GameOverlay(mMainGame);
		mOverlay = new GameOverlay();

	//Load first level in game logic
	mMainGame->LoadFirstLevel();
}
void MainGameScreen::ExecuteRender()
{
	//Rendering is done by entities -> IndieLib
	//Setup good zooms for cameras
	//Call rendering routine from game
	mMainGame->RenderScene();
	//Call rendering routing from overlay
	mOverlay->Render();
}

//To execute while rendering
void MainGameScreen::ExecuteLogicUpdate(float dt)
{
	mMainGame->UpdateGameState(dt);
	mOverlay->Update(dt);
}

//To execute at exit
void MainGameScreen::ExecuteExit()
{
}

//Process possible events
bool MainGameScreen::HandleEvent(const EventData& theevent)
{
	bool eventprocessed(false);

	//IF - Event type is exit
	if(theevent.GetEventType() == Event_ExitGame)
	{
		//Just store request to exit on update
		mExit = true;
	}//IF

	return eventprocessed;
}


//Return a game state if it is necessary to exit
GameState MainGameScreen::IsExit()
{
	//Check if it is needed to change screen
	if(mExit)
		return (MAINMENU); //To main menu 
	else
		return(NOSTATE); //No change
}

//Internal init
void MainGameScreen::_init()
{
}
//Internal release
void MainGameScreen::_release()
{
	//Dynamically allocated memory...
	if(mOverlay)
	{
		delete mOverlay;
		mOverlay = NULL;
	}

	if(mMainGame)
	{
		delete mMainGame;
		mMainGame = NULL;
	}
	
	//Release from memory and graphics system is done by base class
}