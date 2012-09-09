/*
	Filename: GameEventManager.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: In-game event manager
	Comments: IMPORTANT: Events are passed as an auto_ptr, that means that in any client using event manager, 
	          event data should be created in an auto pointer and passed to the manager by reference. That way
			  ownership and data integrity is satisfied for all lifespan of the event. There is a typedef 
			  just for this auto pointer declaration
	Attribution: "Game coding complete" was the inspiration , great book! http://www.mcshaffry.com/GameCode/
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/


#ifndef _GAMEEVENTMANAGER
#define _GAMEEVENTMANAGER
//Library dependencies
#include <string>
#include <map>
#include <list>
//Class dependencies
#include "Singleton_Template.h"
#include "GameEventsDef.h"
#include "LogManager.h"

//Definitions
const int NUMEVENTQUEUES = 2;		//Number of buffers to process queued events
					
class GameEventManager : public MeyersSingleton<GameEventManager>
{
	//Definitions
private:
	typedef std::list<IEventListener* const> EventListenerList;	//List of event listeners
	typedef std::map<GameEventType, EventListenerList> EventListenersMap;      //Mapping of event type to a list of listeners
	typedef std::list<EventDataPointer> EventsList;				//List of queued unprocessed events
	//Intermediate results typedefs to make program readable
	typedef std::pair<EventListenersMap::iterator,bool> EvListInsertResult;
	typedef std::pair<GameEventType, EventListenerList> EvListEntry;
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameEventManager():
	  mInitialized(false),
	  mActiveQueue(0)
	{
		mInitialized = true;
		SingletonLogMgr::Instance()->AddNewLine("GameEventManager","Event Manager initialized",LOGNORMAL);
	}
	~GameEventManager()
	{
		mActiveQueue = 0;
		mInitialized = false;
		SingletonLogMgr::Instance()->AddNewLine("GameEventManager","Event Manager destroyed",LOGNORMAL);
	}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----		
	//Adding and removing of listeners
	bool AddListener(IEventListener* const listenerptr, const GameEventType& eventtype);
	bool RemoveListener(IEventListener* const listenerptr, const GameEventType& eventtype);
	//Events triggering
	bool TriggerEvent(EventDataPointer const & newevent);
	bool QueueEvent(EventDataPointer const & newevent);
	bool AbortEvent(const GameEventType& typetoabort, bool alloftype);
	//Updating
	bool Update( float dt);
	//Special actions
	void EmptyEventQueues();
	//----- PUBLIC VARIABLES ------	

protected:
	//----- INTERNAL VARIABLES -----
	bool mInitialized;				//Tracking of initialization 
	EventListenersMap mEvTypeListenerMap;		//Dynamic map of event type - listener list
	EventsList mEventQueue[NUMEVENTQUEUES];			//Queues of events(more than one to prevent infinite generation of cycled events)
	int mActiveQueue;					//Which queue is active
	//----- INTERNAL FUNCTIONS -----
};

//Definitions - SINGLETON
typedef GameEventManager SingletonGameEventMgr;

#endif
