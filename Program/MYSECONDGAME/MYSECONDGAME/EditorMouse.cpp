/*
	Filename: EditorMouse.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to mouse input in editor screen
	Comments: It depends of IndieLib, as it will be library used for input
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "EditorMouse.h"
#include "EditorLogic.h"
#include "Camera2D.h"

//Update method
void EditorMouse::Update(float)
{
	//Get input and physics manager
	IND_Input* input = SingletonIndieLib::Instance()->Input;
	
	//Update mouse position
	float newx = static_cast<float>(input->GetMouseX());
	float newy = static_cast<float>(input->GetMouseY());
	_updatePosition(newx,newy);
	
	//Left-Dragging tracking
	if(input->OnMouseButtonPress(IND_MBUTTON_LEFT) && !mRightDragging)
	{	
		mDragging = true;
	}
	else if(input->OnMouseButtonRelease(IND_MBUTTON_LEFT))
	{
		mDragging = false;
	}
	//Right-Dragging tracking
	if(input->OnMouseButtonPress(IND_MBUTTON_RIGHT) && !mDragging)
	{	
		mRightDragging = true;
	}
	else if(input->OnMouseButtonRelease(IND_MBUTTON_RIGHT))
	{
		mRightDragging = false;
	}

	//Move and define bodies (dragging commands) commands
	//Move tile command (dragging and left-button)
	mEditor->MoveTileCommand(input->IsMouseButtonPressed(IND_MBUTTON_LEFT) && mDragging);
	//Define body command (dragging and right-button)
	mEditor->DefineBodyCommand(input->IsMouseButtonPressed(IND_MBUTTON_RIGHT) && mRightDragging);

	//Zoom commands
	if(input->OnMouseButtonPress(IND_MBUTTON_WHEELUP ))
	{
		mEditor->ZoomInCommand();
	}
	else if(input->OnMouseButtonPress(IND_MBUTTON_WHEELDOWN))
	{
		mEditor->ZoomOutCommand();
	}
	
	//Other commands

	//Mix Keyboard and mouse (change add-to body)
	if((input->IsKeyPressed(IND_LCTRL) 
	   ||
	   input->IsKeyPressed(IND_RCTRL))
	   &&
	   input->OnMouseButtonPress(IND_MBUTTON_LEFT))
	{
		mEditor->ChangeBodyToAddShapesTo();
	}
}
	
//Render necessary elements
void EditorMouse::Render()
{
	//Render mouse is made by entities...
}

void EditorMouse::_updatePosition(float newx,float newy)
{
	//-----Put in position of mouse (pixels) to render in overlay camera-----------
	//relative to overlay position in screen center
	mCursorPositionPix.x = newx - mResX/2;
	mCursorPositionPix.y = newy - mResY/2;

	//-------Update position of brush (pixels) to render in current camera---------
	//Get necessary values from editing camera
	Camera2DPointer currentcamera = mEditor->GetCurrentRenderCamera();

	float zoom = currentcamera->GetZoom();   //Zoom of current render camera
	Vector2 camerapospix = currentcamera->GetPositionPix();		//Position of current render camera
		
	//Scale position with current camera zoom
	mBrushPositionPix = (mCursorPositionPix / zoom);
	//Offset position with current camera position
	mBrushPositionPix += camerapospix;	
	
	//--------------Update mouse display position---------------------------------
	if(mMouseGFX)
	{
		//Change scaling to zoom, not to change cursor size with zoom
		mMouseGFX->SetScale(1/zoom,1/zoom);
		mMouseGFX->SetPosition(static_cast<float>(mBrushPositionPix.x),
							   static_cast<float>(mBrushPositionPix.y),
							   0);
	}
	//--------------Update brush display position---------------------------------
	SpritePointer editingbrush = mEditor->GetEditBrush();
	if(editingbrush)
	{
		editingbrush->SetPosition(static_cast<float>(mBrushPositionPix.x),
							   static_cast<float>(mBrushPositionPix.y),
							   0);		
	}

	SpritePointer collidedinfo = mEditor->GetCollidedInfo();
	if(collidedinfo)
	{
		collidedinfo->SetPosition(static_cast<float>(mBrushPositionPix.x),
							   static_cast<float>(mBrushPositionPix.y),
							   0);		
	}

}