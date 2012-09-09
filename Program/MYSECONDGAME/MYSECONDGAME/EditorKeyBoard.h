/*
	Filename: EditorKeyBoard.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to keyboard input
	Comments: It depends of IndieLib, as it will be library used for input
	          The keyboard is used to control things by the user in editor mode
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*TODO:
	- Merge all input logic in a nice structure of classes, for now there are 4 separated classes which share some
	  code... this is a TODO for GameMouse and GameKeyboard too...
*/


#ifndef _EDITORKEYBOARD
#define _EDITORKEYBOARD

//Library dependencies

//Class dependencies
#include "Vector2.h"

//Forward declarations
class EditorLogic;

class EditorKeyBoard
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	EditorKeyBoard(EditorLogic* editor):
    mEditor(editor)
	{	
	}
	virtual ~EditorKeyBoard()
	{
	}

	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	void Update(float dt);		//Update method
	void Render();				//Render necessary elements
	//----- PUBLIC VARIABLES ------

protected:
	//----- INTERNAL VARIABLES -----
	EditorLogic* mEditor;		//Game pointer
	//----- INTERNAL FUNCTIONS -----
};

#endif