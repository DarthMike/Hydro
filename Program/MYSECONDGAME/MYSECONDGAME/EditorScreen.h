/*
	Filename: EditorScreen.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Implementation of a screen in game - EDITORSCREEN
	Comments: Dependant of IndieLib Graphics library - derived from abtract class "GameScreen"
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

/*
	TODO:
		- It should be here where input is processed, and sent to game. For now, game input and overlay
		  display is done in another class inside game logic. Not good architecture, but for now it works...
*/
#ifndef _EDITORSCREEN
#define _EDITORSCREEN

//Library dependencies

//Class dependencies
#include "General_Resources.h"
#include "GameScreen.h"

//Forward declarations
class EditorLogic;

class EditorScreen : public GameScreen
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	EditorScreen()
		:GameScreen(),
		mEditorLogic(NULL)
	{
		_init();
		SingletonLogMgr::Instance()->AddNewLine("EditorScreen","EditorScreen scren loaded",LOGDEBUG);
	}
	virtual ~EditorScreen()
	{
		_release();
		SingletonLogMgr::Instance()->AddNewLine("EditorScreen","EditorScreen screen released",LOGDEBUG);
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
	EditorLogic* mEditorLogic;
	//----- INTERNAL FUNCTIONS -----
	void _init();//Internal init
	void _release(); //Internal release
};

#endif