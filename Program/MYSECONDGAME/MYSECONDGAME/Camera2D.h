/*
	Filename: Camera2D.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Base abstract class for 2D cameras, implements base functionality of access
	Comments: Generally used to move Indie cameras in a nice way, and to render complex 
	          scenes (parallax) and overlays in an automated way
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _CAMERA2D
#define _CAMERA2D

//Library dependencies
#include <string>
//Class dependencies
#include "Vector2.h"
#include "LogManager.h"

class Camera2D
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	Camera2D(const Vector2 &position, float zoom,float globalscale, int layer, const std::string& name, const Vector2& maxaabb, const Vector2& minaabb)
		:mGlobalScale(globalscale),
		mRotation(0),
		mPosition(position.x,position.y),
		mPositionPix(position.x * mGlobalScale, position.y * mGlobalScale),
		mWorldAABBMin(minaabb),
		mWorldAABBMax(maxaabb),
		mMaxSpeed(0.1f),
		mZoom(zoom),
		mMaxZoom(5.0f),
		mMinZoom(0.1f),
		mLayer(layer),
		mName(name),
		mMovsLimited(false)
	{
		assert(mGlobalScale > 0.001 );
		assert(mLayer >= 0 && mLayer <64); 
		SingletonLogMgr::Instance()->AddNewLine("Camera2D","New camera added",LOGDEBUG);
	}
	~Camera2D()
	{
		SingletonLogMgr::Instance()->AddNewLine("Camera2D","Camera removed",LOGDEBUG);
	}
	//----- GET/SET FUNCTIONS -----
	Vector2 GetPosition() { return mPosition; }			//Position(in units)
	Vector2 GetPositionPix() {return mPositionPix;}		//Position(in pixels)
	Vector2 GetSpeed() { return mSpeed; }				//Speed of camera
	virtual void SetPosition(const Vector2& newposition) = 0; //Position(in units)
	virtual void SetPositionPix(const Vector2& newpositionpix) = 0; //Position(in pixels)
	virtual void SetZoom(float newzoom) = 0;		//Zoom in %
	virtual void SetLimitingMoves(bool limiting) = 0; //To set limit moves
	void SetMaxMinPositions(const Vector2& minaabb, const Vector2& maxaabb) { mWorldAABBMin = minaabb; mWorldAABBMax = maxaabb; }  //Max and min AABB coordinates for movements 	
	float SetMaxSpeed(float newspeed) { mMaxSpeed = newspeed; }
	float GetRotation() {return mRotation; }	//Angle
	float GetZoom() { return mZoom;}			//Zoom
	void SetMaxMinZoom(float max,float min) { assert(max>min); mMaxZoom = max;  mMinZoom = min; } //Max and min zoom
	void GetMaxMinZoom(float& max, float& min) { max = mMaxZoom; min = mMinZoom; }  //Max and min zoom
	int GetLayer() { return mLayer; }			//Get the camera layer
	const std::string& GetName() { return mName; } //Get the camera name 

	//----- OTHER FUNCTIONS -----
	virtual void Update (float dt)= 0;					//Periodically called to make movements/zooms... etc internally
	virtual void SetAsCurrent()= 0;						//Sets this camera as current
	virtual void Translate(const Vector2& speed) = 0;	//Make an increment in position
	virtual void MoveTo(const Vector2& newposition) = 0;//Command to move to a new position
	virtual void Rotate(float speed)= 0;;				//Make an increment in angle
	virtual void Zoom(bool positive)= 0;;				//Apply a multiplication of zoom
	//----- PUBLIC VARIABLES ------

protected:
	//----- INTERNAL VARIABLES -----
	Vector2 mPosition;			//The position of the camera (units)
	Vector2 mPositionPix;		//The position of the camera (pixels)
	Vector2 mSpeed;				//The speed of movement of camera
	float mMaxSpeed;			//Max speed of camera
	float mRotation;			//The rotation of camera (only in one axis - Z)
	float mZoom;				//Zoom 
	float mMaxZoom;				//Max and min zoom
	float mMinZoom;
	bool mMovsLimited;			//Limit moves of camera with AABB
	Vector2 mWorldAABBMax;		//The max position allowed
	Vector2 mWorldAABBMin;		//The min position allowed
	float mGlobalScale;		//Global scale for unit/pixels in X axis	
	int mLayer;					//Indielib Layer
	std::string mName;			//Camera name
	//----- INTERNAL FUNCTIONS -----
};

#endif