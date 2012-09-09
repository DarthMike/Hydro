/*
	Filename: GameOverlayListener.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Event listener for overlay
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _GAMEOVERLAYLISTENER
#define _GAMEOVERLAYLISTENER
//Library dependencies

//Class dependencies
#include "GameEventManager.h"

//Forward declarations
class GameOverlay;
class EventData;

class GameOverlayListener : public IEventListener
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameOverlayListener(GameOverlay* overlayptr)
		:mName("GameOverlayListener"),
		mOverlay(overlayptr)
	{
		//Register events to process
		SingletonGameEventMgr::Instance()->AddListener(this,Event_DebugString);  //Debug strings
		SingletonGameEventMgr::Instance()->AddListener(this,Event_GameOver);  //Game Over
		SingletonGameEventMgr::Instance()->AddListener(this,Event_BlobHealth);  //Blob Health Update
		SingletonGameEventMgr::Instance()->AddListener(this,Event_BlobDeath);  //Blob died
		SingletonGameEventMgr::Instance()->AddListener(this,Event_BlobPosition);  //Blob position
		SingletonGameEventMgr::Instance()->AddListener(this,Event_DropCollected);   //Drop Collected
		SingletonGameEventMgr::Instance()->AddListener(this,Event_SolidCollision);    //Solid bodies collision
		//Level events
		SingletonGameEventMgr::Instance()->AddListener(this,Event_NewCollectedValues);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_LevelCompleted);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_ExitGame);  //Exit game event
	}
	~GameOverlayListener()
	{
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_DebugString);  //Debug strings
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_GameOver); //Game Over
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_BlobHealth);  //Blob Health Update
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_BlobDeath);  //Blob died
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_BlobPosition);  //Blob position
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_DropCollected);   //Drop Collected
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_SolidCollision);    //Solid bodies collision
		//Level events
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_NewCollectedValues);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_LevelCompleted);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_ExitGame);  //Exit game event
	}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	//Implementation of handling
	virtual bool HandleEvent(const EventData& theevent);
	//Implementation of name
	virtual const std::string& GetName()
	{
		return mName;
	}
	//----- PUBLIC VARIABLES ------	

protected:
	//----- INTERNAL VARIABLES -----
	std::string mName;
	GameOverlay* mOverlay;
	//----- INTERNAL FUNCTIONS -----
};

#endif
