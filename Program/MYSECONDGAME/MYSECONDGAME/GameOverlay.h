/*
	Filename: GameOverlay.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to overlay presentation (UI)
	Comments: It depends of IndieLib
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*TODO:
	- Make mouse and keyboard part of this class, not the game logic
*/


#ifndef _GAMEOVERLAY
#define _GAMEOVERLAY

//Library dependencies
#include <string>
#include <sstream>
#include <list>
//Class dependencies
#include "Vector2.h"
#include "Shared_Resources.h"
#include "GFXDefs.h"
#include "LogManager.h"
#include "GenericException.h"
#include "AnimationController.h"

//Forward declarations
class EventData;
class GameOverlayListener;
class GameMouse;
class GameKeyBoard;

class GameOverlay
{
	friend class GameOverlayListener; //Listener is friend to this, as it has access to internals... (just a handle really)

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameOverlay():
	  mGameMouse(NULL), 
	  mGameKeyBoard(NULL),
	  mScaleFactor(0),
	  mResX(0),
	  mResY(0),
	  mCounter(0.0f),
	  mPositionUpdateDelay(0.0f),
	  mListener(NULL),
	  mFreeCameraMode(false),
	  mDisplayNextMessages(true),
	  mDebugLines(0),
	  mLastHealth(0.0f),
	  mLevelCompleted(false),
	  mGameOver(false)
	{	
		_init();
		SingletonLogMgr::Instance()->AddNewLine("GameOverlay","Overlay created and ready",LOGNORMAL);
	}
	virtual ~GameOverlay()
	{
		_release();
		SingletonLogMgr::Instance()->AddNewLine("GameOverlay","Overlay destroyed",LOGNORMAL);
	}

	//----- GET/SET FUNCTIONS -----
	
	//----- OTHER FUNCTIONS -----
	void Update(float dt);		//Update method
	void Render();				//Render necessary elements
	void ToggleCameraMode() { mFreeCameraMode = !mFreeCameraMode; }; //Toggle camera mode
	bool IsFreeCamera() { return mFreeCameraMode; }
	//----- PUBLIC VARIABLES ------

protected:
	//----- INTERNAL VARIABLES -----
	float mScaleFactor;			//Transform scale to get pixel units x
	float mResX;				//Screen X resolution
	float mResY;				//Screen Y resolution
	float mCounter;				//Timing variable
	float mPositionUpdateDelay;	//Timing variable

	GameMouse* mGameMouse;			//Input controllers pointers
	GameKeyBoard* mGameKeyBoard;

	static const std::string mOverlayAssetsFileName;  //Filename hard-coded
	GameLevelPointer mOverlayAssets;	//The entities loaded from xml file
	std::list<AnimationControllerPointer> mDynamicAnimations;	//Dynamic entities created in-game

	GameOverlayListener* mListener;			//Events listener
	
	bool mFreeCameraMode;			//Free camera mode /yes-no
	bool mDisplayNextMessages;
	float mLastHealth;				//To update health and with camera modes correctly
	bool mLevelCompleted;			//Tracking of level completed
	bool mGameOver;					//Tracking of game over
	
	//Text for font entities in overlay
	std::string mDebugText;
	std::string mMessagesText;
	std::string mCollectedText;
	std::string mHealthText;

	//Debug messages
	int mDebugLines;
	std::stringstream mDebugMSGstream;

	//----- INTERNAL FUNCTIONS -----
	void _init();
	void _resetVariables();
	void _release();
	//Event handling
	bool _handleEvents(const EventData& theevent);
};

#endif