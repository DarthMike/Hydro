/*
	Filename: OverlayCamera2D.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate a 2d camera management for overlays this camera doesnt allow zooms neither movements
	Comments: Based directly in IndieLib
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _OVERLAYCAMERA2D
#define _OVERLAYCAMERA2D

//Library dependencies
#include "IndieLib\Common\LibHeaders\Indie.h"
#include <assert.h>
//Class dependencies
#include "Camera2D.h"
#include "Shared_Resources.h"

class OverlayCamera2D : public Camera2D
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----

	OverlayCamera2D(const Vector2 &position,float globalscale, int layer, const std::string& name,const Vector2& maxaabb, const Vector2& minaabb):
		Camera2D(position,1.0f,globalscale,layer,name,maxaabb,minaabb), //In construction, OVERLAY camera is on top layer
		mIndCamera(static_cast<int>(mPositionPix.x),static_cast<int>(mPositionPix.y)),
		mSpeed(0.0f,0.0f),
		mRotationSpeed(0.0f)
	{
		//Setup indielib camera
		mIndCamera.SetZoom(mZoom);
		//Moves or position fixed and not limited by AABB
		mMovsLimited = false;
	}
	~OverlayCamera2D()
	{
	}
	//----- GET/SET FUNCTIONS -----
	//Get of variables defined in base class!!
	//Implementation of interface
	virtual void SetPosition(const Vector2&){ } //Doesnt do anything - this camera doesnt move
	virtual void SetPositionPix(const Vector2&) { } //Doesnt do anything - this camera doesnt move
	virtual void SetZoom(float) {} //Doesnt do anything - this camera doesnt change zoom
	virtual void SetLimitingMoves(bool) { }  //Doesnt do anything - this camera doesnt change position
	//----- OTHER FUNCTIONS -----
	//Implementation from base class!!
	virtual void Update (float dt);					//Periodically called to make movements/zooms... etc internally
	virtual void SetAsCurrent();					//Sets this camera as current in IndieLib
	virtual void Translate(const Vector2& speed);	//Make an increment in position
	virtual void MoveTo(const Vector2& newposition);//Command to move to a new position
	virtual void Rotate(float speed);				//Make an increment in angle
	virtual void Zoom(bool positive);				//Apply a multiplication of zoom
	//----- PUBLIC VARIABLES ------

private:
	//----- INTERNAL VARIABLES -----
	IND_Camera2d mIndCamera;		//The indielib camera object
	Vector2 mSpeed;					//The speed of movement of camera
	float mRotationSpeed;			//Speed of rotation of camera
	bool mIsGeneral;				//Tracking to know if it is general camera (to apply scales for representation)
	//----- INTERNAL FUNCTIONS -----
};
#endif