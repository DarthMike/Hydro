/*
	Filename: OverlayCamera2D.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate a 2d camera management for overlays this camera doesnt allow zooms neither movements
	Comments: Based directly in IndieLib
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "OverlayCamera2D.h"
#include "IndieLibManager.h"

//Periodically called to make movements/zooms... etc internally
void OverlayCamera2D::Update(float)
{
	//Camera does not move
}

//Sets this camera as current in IndieLib
void OverlayCamera2D::SetAsCurrent()
{
	//Call manager method for setting current camera
	SingletonIndieLib::Instance()->Render->SetCamera2d(&mIndCamera);
}

//Make an increment in position
void OverlayCamera2D::Translate(const Vector2&)
{
	//EMPTY, NOT MOVABLE DIRECTLY
}

//Command to move to a new position
void OverlayCamera2D::MoveTo(const Vector2&)
{
	//EMPTY, NOT MOVABLE DIRECTLY
}

//Make an increment in angle
void OverlayCamera2D::Rotate(float)
{
	//EMPTY, NOT MOVABLE DIRECTLY
}

//Apply a multiplication of zoom
void OverlayCamera2D::Zoom(bool)
{

	//EMPTY, NOT MOVABLE DIRECTLY
}