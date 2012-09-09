/*
	Filename: IndieCamera2D.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate a 2d camera management for IndieLib
	Comments: Based directly in IndieLib
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _INDIECAMERA2D
#define _INDIECAMERA2D

//Library dependencies
#include "IndieLib\Common\LibHeaders\Indie.h"
#include <assert.h>
//Class dependencies
#include "Camera2D.h"

class IndieCamera2D : public Camera2D
{
	//Definitions
protected:
	static const float ZOOMINCREMENT;  //Max control variables
	static const float ZOOMCHANGESPEED;
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	IndieCamera2D(const Vector2 &position, float zoom,float globalscale, int layer, float screenwidth, float screenheight, const std::string& name, const Vector2& maxaabb, const Vector2& minaabb):
	    Camera2D(position,zoom,globalscale,layer,name,maxaabb,minaabb),
		mIndCamera(static_cast<int>(mPositionPix.x),static_cast<int>(screenheight - mPositionPix.y)),
		mTarget(0.0f,0.0f),
		mDesiredZoom(zoom),
		mAutoMove(false),
		mRotationSpeed(0.0f),
		mScreenWidth(screenwidth),
		mScreenHeight(screenheight)
	{
		//Debug assert
		assert(zoom > 0);
		//Update coords in pixels
		mPositionPix = Vector2(mPosition.x * globalscale,
				   screenheight - mPosition.y * globalscale);
		//Setup indielib camera
		mIndCamera.SetZoom(zoom);

	}
	~IndieCamera2D()
	{
	}
	//----- GET/SET FUNCTIONS -----
	//Get of variables defined in base class!!
	//Implementation of interface
	virtual void SetPosition(const Vector2& newposition);
	virtual void SetPositionPix(const Vector2& newpositionpix);
	virtual void SetZoom(float newzoom);
	virtual void SetLimitingMoves(bool limiting) { mMovsLimited = limiting; _checkBoundariesOfPosition();}
	//----- OTHER FUNCTIONS -----
	//Interface implementation
	virtual void Update (float dt);					//Periodically called to make movements/zooms... etc internally
	virtual void SetAsCurrent();					//Sets this camera as current in IndieLib
	virtual void Translate(const Vector2& speed);	//Make an increment in position
	virtual void MoveTo(const Vector2& newposition);//Command to move to a new position		
	virtual void Rotate(float speed);				//Make an increment in angle
	virtual void Zoom(bool positive);				//Apply a multiplication of zoom
	//----- PUBLIC VARIABLES ------

private:
	//----- INTERNAL VARIABLES -----
	IND_Camera2d mIndCamera;	//The indielib camera object
	Vector2 mTarget;			//Target position (if moved automatically)
	float mDesiredZoom;			//Desired target zoom
	bool mAutoMove;				//Command to move automatically by positions
	float mRotationSpeed;		//Speed of rotation of camera
	float mScreenWidth;			//Screen width (to limits moves)
	float mScreenHeight;		//Screen height in pixels (for scaling reasons)
	//----- INTERNAL FUNCTIONS -----
	void _checkBoundariesOfPosition();

};

#endif