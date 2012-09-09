/*
	Filename: EditorScreen.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Implementation of a screen in game - EDITORSCREEN
	Comments: Dependant of IndieLib Graphics library - derived from abtract class "GameScreen"
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "EditorScreen.h"
#include "EditorLogic.h"
#include "IndieLibManager.h"

//To execute on start
void EditorScreen::ExecuteEnter()
{
	//Create game object
	mEditorLogic = new EditorLogic();
}
void EditorScreen::ExecuteRender()
{
	//Rendering is done by entities -> IndieLib

	//Call rendering routine from game
	mEditorLogic->RenderScene();
}

//To execute while rendering
void EditorScreen::ExecuteLogicUpdate(float dt)
{
	mEditorLogic->UpdateState(dt);
}

//To execute at exit
void EditorScreen::ExecuteExit()
{
	//TODO: SAVE CURRENT EDITED LEVEL
}

//Process possible events
bool EditorScreen::HandleEvent(const EventData& theevent) 
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
GameState EditorScreen::IsExit()
{
	//Check if it is needed to change screen
	if(mExit)
		return (EXIT); //To main menu 
	else
		return(NOSTATE); //No change
}

//Internal init
void EditorScreen::_init()
{
}
//Internal release
void EditorScreen::_release()
{
	//Dynamically allocated memory...
	if(mEditorLogic)
		delete mEditorLogic;
	
	//Release from memory and graphics system is done by base class
}