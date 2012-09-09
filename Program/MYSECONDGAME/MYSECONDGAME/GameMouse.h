/*
	Filename: GameMouse.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to mouse input in game screen
	Comments: It depends of IndieLib, as it will be library used for input
	          The game mouse, can take objects and move them using the physics engine
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*TODO:
	- Merge both input controllers classes by creating an interface class, deriving both gamemouse and gamekeyboard
	  from it... but to make it work hastily is good as-is. (no 2 players expected, neither change of controller so..)
	- Generally speaking, input should be decoupled from which library is used. Throughout the game code it is not 
	  done as I dont expect to change things and libraries in short time. but... it is TODO.
*/


#ifndef _GAMEMOUSE
#define _GAMEMOUSE

//Library dependencies

//Class dependencies
#include "Vector2.h"
#include "IndieLibManager.h"
#include "Shared_Resources.h"

//Forward declarations
class PhysicsSim;
class GameOverlay;

class GameMouse
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	//GameMouse(SpritePointer themouseentity, PhysicsSim* thegame, GameOverlay* overlayptr):
	  GameMouse(SpritePointer themouseentity, GameOverlay* overlayptr):
	  mMouseGFX(themouseentity),
	 // mGame(thegame),
	  mOverlay(overlayptr),
	  mDragging(false),
	  mPosition(0.0f,0.0f),
	  mPositionPix(0.0f,0.0f),
	  mScaleFactor(0),
	  mResX(0),
	  mResY(0),
	  mForceCommand(0.0f)
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
	virtual ~GameMouse()
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
	bool mDragging;				//Tracks if it is dragging some object
	Vector2 mPosition;			//Position of mouse cursor in "units" absolute to world
	Vector2 mPositionPix;		//Position of mouse cursor in "pixels" units
	float mScaleFactor;			//Transform scale to get pixel units x
	float mResX;				//Screen X resolution
	float mResY;				//Screen Y resolution
	SpritePointer mMouseGFX;	//Mouse IndieLib GFX

	GameOverlay* mOverlay;		//Overlay pointer
	//PhysicsSim* mGame;			//Game pointer
	float mForceCommand;		//Force command counter to throw BLOB
	//----- INTERNAL FUNCTIONS -----
	void _updatePosition(float newx,float newy);
};

#endif