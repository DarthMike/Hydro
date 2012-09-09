/*
	Filename: EditorMouse.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to mouse input in editor screen
	Comments: It depends of IndieLib, as it will be library used for input
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*TODO:
	- Merge all input logic in a nice structure of classes, for now there are 4 separated classes which share some
	  code... this is a TODO for GameMouse and GameKeyboard too...
*/


#ifndef _EDITORMOUSE
#define _EDITORMOUSE

//Library dependencies

//Class dependencies
#include "Vector2.h"
#include "IndieLibManager.h"
#include "Shared_Resources.h"

//Forward declarations
class EditorLogic;

class EditorMouse
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	EditorMouse(SpritePointer themouseentity, EditorLogic* editor):
	  mMouseGFX(themouseentity),
	  mEditor(editor),
	  mDragging(false),
	  mRightDragging(false),
	  mBrushPositionPix(0.0f,0.0f),
	  mCursorPositionPix(0.0f,0.0f),
	  mScaleFactor(0),
	  mResX(0),
	  mResY(0)
	{	
		//Get general values from display
		mScaleFactor = SingletonIndieLib::Instance()->GetGeneralScale();
		mResX = static_cast<float>(SingletonIndieLib::Instance()->Window->GetWidth());
		mResY = static_cast<float>(SingletonIndieLib::Instance()->Window->GetHeight());

		//When created, it shows mouse cursor and positions itself
		if(mMouseGFX)
			mMouseGFX->SetShow(true);

		float posx = static_cast<float>(SingletonIndieLib::Instance()->Input->GetMouseX());
		float posy = static_cast<float>(SingletonIndieLib::Instance()->Input->GetMouseY());

		_updatePosition(posx,posy);
	}
	virtual ~EditorMouse()
	{
		//Dont show mouse cursor
		mMouseGFX->SetShow(false);
	}

	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	void Update(float dt);		//Update method
	void Render();				//Render necessary elements
	//----- PUBLIC VARIABLES ------

protected:
	//----- INTERNAL VARIABLES -----
	bool mDragging;				//Tracks if it is dragging with left mouse button
	bool mRightDragging;		//Trak if it is dragging with right mouse button
	Vector2 mBrushPositionPix;		//Position of editing brush in "pixels" units (in current editing layer)
	Vector2 mCursorPositionPix;	//Position of mouse cursor in "pixels" units
	float mScaleFactor;			//Transform scale to get pixel units x
	float mResX;				//Screen X resolution
	float mResY;				//Screen Y resolution
	SpritePointer mMouseGFX;	//Mouse IndieLib GFX
	EditorLogic* mEditor;		//Game pointer
	//----- INTERNAL FUNCTIONS -----
	void _updatePosition(float newx,float newy);
};

#endif