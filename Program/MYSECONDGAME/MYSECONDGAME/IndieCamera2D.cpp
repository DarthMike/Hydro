/*
	Filename: IndieIndieCamera2D.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate a 2d camera management
	Comments: Based directly in IndieLib
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "IndieCamera2D.h"
#include "IndieLibManager.h"
#include "Math.h"

const float IndieCamera2D::ZOOMINCREMENT = 0.01f;  //Max control variables	
const float IndieCamera2D::ZOOMCHANGESPEED = 0.04f;

//Periodically called to make movements/zooms... etc internally
void IndieCamera2D::Update(float dt)
{
	
	//Movement calculations 
	//IF - Automovement active
	if(mAutoMove)
	{
		Vector2 targetdiff = mTarget - mPosition;
		//IF - Significant difference
		if(abs(targetdiff.x) > 0.0001 || abs(targetdiff.y) > 0.0001)
		{
			mSpeed = targetdiff * 0.005;
			mSpeed.Clamp(mMaxSpeed);
		}
		else // ELSE - Position reached
		{
			mAutoMove = false;
			mSpeed = Vector2(0,0);
		}//IF
	} //IF

	//Zoom calculations
	float zoomdiference = mDesiredZoom - mZoom;
	float zoomspeed = 0.0f;
	//IF - Significant difference of zoom vs desired
	if(abs(zoomdiference) > ZOOMINCREMENT)
	{
		zoomspeed = zoomdiference * 0.001f;
		if(abs(zoomspeed) > ZOOMCHANGESPEED)
			zoomspeed = ZOOMCHANGESPEED;
	}//IF

	//Move as requested
	//Update internal tracking
	mPosition += mSpeed * dt; //Increment position (Units)
	//Update internal tracking
	mRotation += mRotationSpeed * dt;
	//Update internal zoom
	mZoom += zoomspeed * dt;
	//Update position in pixels
	mPositionPix = SingletonIndieLib::Instance()->FromCoordToPix(mPosition);
	//Correct by boundaries
	_checkBoundariesOfPosition();
	
	//Update camera values (in pixels)
	mIndCamera.SetPosition(static_cast<int>(mPositionPix.x),
						   static_cast<int>(mPositionPix.y));
	//Update camera values
	mIndCamera.SetAngle(mRotation);
	mIndCamera.SetZoom(mZoom);

	//IF - Not automovement
	if(!mAutoMove)
	{
		mSpeed = Vector2(0,0);
	}
	//DEBUG VARIABLES
	/*#ifdef _DEBUGGING
	printf("\nCAMERA %s:PositionX = %.4f PositionY = %.4f\nPixPosX = %.4f PixPosY = %.4f\n Zoom = %.4f",mName.c_str(),mPosition.x,mPosition.y, mPosition.x, mPosition.y,mZoom);
	#endif*/
}

void IndieCamera2D::SetPosition(const Vector2& newposition)
{ 
	//Update internal values
	mPosition = newposition;
	mPositionPix = SingletonIndieLib::Instance()->FromCoordToPix(mPosition);
	_checkBoundariesOfPosition();
	//Make changes to real camera
	mIndCamera.SetPosition(static_cast<int>(mPositionPix.x),
						   static_cast<int>(mPositionPix.y)
						   ); 
}

void IndieCamera2D::SetPositionPix(const Vector2& newpositionpix) 
{ 
	//Update internal values
	mPositionPix = newpositionpix;
	mPosition = SingletonIndieLib::Instance()->FromPixToCoord(mPositionPix);
	_checkBoundariesOfPosition();
	//Make changes to real camera
	mIndCamera.SetPosition(static_cast<int>(mPositionPix.x),
						   static_cast<int>(mPositionPix.y)
						   ); 
}

void IndieCamera2D::SetZoom(float newzoom)
{
	mZoom = newzoom;
	//IF - Out of limits (upper and lower)
	if(mZoom > mMaxZoom) mZoom = mMaxZoom;
	if(mZoom < mMinZoom) mZoom = mMinZoom;
	
	mIndCamera.SetZoom(mZoom);

	mDesiredZoom = mZoom; //No difference (instant change in uptate fcn)
}


//Sets this camera as current in IndieLib
void IndieCamera2D::SetAsCurrent()
{
	//Call manager method for setting current camera
	SingletonIndieLib::Instance()->Render->SetCamera2d(&mIndCamera);
}

//Make an increment in position
void IndieCamera2D::Translate(const Vector2& speed)
{
	//Memorize speed
	mSpeed = speed;
	mTarget = Vector2(0,0); //Reset target vector
	mAutoMove = false;		//Reset memory to automove
}

//Command to move to a new position
void IndieCamera2D::MoveTo(const Vector2& newposition)
{
	//Memorize speed
	mTarget = newposition;
	mAutoMove = true;
}

//Make an increment in angle
void IndieCamera2D::Rotate(float speed)
{
	//Memorize speed
	mRotationSpeed = speed;
}

//Apply a multiplication of zoom
void IndieCamera2D::Zoom(bool positive)
{
	//Update internal tracking
	if(positive)
		mDesiredZoom += ZOOMINCREMENT;
	else
		mDesiredZoom -= ZOOMINCREMENT;

	//Check limits
	if(mDesiredZoom > mMaxZoom)
		mDesiredZoom = mMaxZoom;
	if(mDesiredZoom < mMinZoom)
		mDesiredZoom =  mMinZoom;
}

//Check the boundaries of current position
void IndieCamera2D::_checkBoundariesOfPosition()
{
	if(mMovsLimited)
	{
		//Local control variables
		bool outoflimits(false);

		//Calculate in pixels width and height of viewport of camera
		float viewwidth (mScreenWidth / mZoom);
		float viewheight (mScreenHeight / mZoom);

		//Calculate AABB of viewport (note as in pixels coords go inversed in sign)
		Vector2 maxcoord(viewwidth / 2, -viewheight / 2);
		Vector2 mincoord(-viewwidth / 2, +viewheight / 2);
		maxcoord += mPositionPix;
		mincoord += mPositionPix;

		//Compare with limits
		Vector2 maxaabbpix = SingletonIndieLib::Instance()->FromCoordToPix(mWorldAABBMax);
		Vector2 minaabbpix = SingletonIndieLib::Instance()->FromCoordToPix(mWorldAABBMin);

		Vector2 maxdifference (maxaabbpix - maxcoord);
		Vector2 mindifference (minaabbpix - mincoord);

		//IF - Difference exceeded in upper limits (X)
		if(maxdifference.x < 0)
		{
			mPositionPix.x += maxdifference.x;
			outoflimits = true;
		}//IF
		//IF - Difference exceeded in upper limits (Y)
		if(maxdifference.y > 0)
		{
			mPositionPix.y += maxdifference.y;
			outoflimits = true;
		}//IF
		//IF - Difference exceeded in lower limits (X)
		if(mindifference.x > 0)
		{
			mPositionPix.x += mindifference.x;
			outoflimits = true;
		}//IF
		//IF - Difference exceeded in lower limits (Y)
		if(mindifference.y < 0)
		{
			mPositionPix.y += mindifference.y;
			outoflimits = true;
		}//IF
		
		//Change position in coords if limits exceeded
		//IF- Limits exceeded
		if(outoflimits)
		{
			mPosition = SingletonIndieLib::Instance()->FromPixToCoord(mPositionPix);
		}//IF
	}
}