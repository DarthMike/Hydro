/*
	Filename: TestEventListener.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Test event listener / only for debugging
	Comments: I
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _TESTEVENTLISTENER
#define _TESTEVENTLISTENER
//Library dependencies
#include <stdio.h>
//Class dependencies
//#include "PhysicsManager.h"
#include "GameEventsDef.h"
#include "PhysicsEvents.h"
#include "GameEventManager.h"

class TestEventListener : public IEventListener
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	TestEventListener()
		:mName("Hello")
	{
		//Register events to process
		/*SingletonGameEventMgr::Instance()->AddListener(this,Event_NewCollision);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_PersistantCollision);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_DeletedCollision);*/
	}
	~TestEventListener()
	{
		/*SingletonGameEventMgr::Instance()->RemoveListener(this,Event_NewCollision);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_PersistantCollision);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_DeletedCollision);*/
	}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	//Implementation of handling
	virtual bool HandleEvent(const EventData& theevent)
	{
		//Receiving of physics events:
		//Check type of event
		if(theevent.GetEventType() == Event_NewCollision)
		{
			//Retrieve data from event
		}
		else if(theevent.GetEventType() == Event_PersistantCollision)
		{
			//Retrieve data from event
		}
		else if(theevent.GetEventType() == Event_DeletedCollision)
		{
			//Retrieve data from event
		}
		return true;
	}
	//Implementation of name
	virtual const std::string& GetName()
	{
		return mName;
	}
	//----- PUBLIC VARIABLES ------	

protected:
	//----- INTERNAL VARIABLES -----
	std::string mName;
	//----- INTERNAL FUNCTIONS -----
};

#endif
