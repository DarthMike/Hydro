/*
	Filename: PhysicsSimListener.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Event listener for game
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _PHYSICSSIMLISTENER
#define _PHYSICSSIMLISTENER
//Library dependencies

//Class dependencies
#include "GameEventManager.h"

//Forward declarations
class PhysicsSim;
class EventData;

class PhysicsSimListener : public IEventListener
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	PhysicsSimListener(PhysicsSim* gameptr)
		:mName("PhysicsSimListener"),
		mGame(gameptr)
	{
		//Register events to process
		SingletonGameEventMgr::Instance()->AddListener(this,Event_GameOver);  //Game Over
		SingletonGameEventMgr::Instance()->AddListener(this,Event_RestartLevel);  //Restart the level
		SingletonGameEventMgr::Instance()->AddListener(this,Event_NextLevel);  //Load next level
		//Level events
		SingletonGameEventMgr::Instance()->AddListener(this,Event_DropCollected); //Drop Collected	
	}
	~PhysicsSimListener()
	{
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_GameOver); //Game Over
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_RestartLevel);  //Restart the level
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_NextLevel);  //Load next level
		//Level events
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_DropCollected);
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
	PhysicsSim* mGame;
	//----- INTERNAL FUNCTIONS -----
};

#endif
