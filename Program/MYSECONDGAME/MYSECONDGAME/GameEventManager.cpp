/*
	Filename: GameEventManager.cpp
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

#include "GameEventManager.h"

//Periodic update
bool GameEventManager::Update( float)
{
	//Update all calls to handle events in events received before this update call.

	//Swap events queues. This is done not to enter an infinite loop where a handling of an 
	//event generates another, and another, and another... event ;) Great code by Mike McShaffry...
	int queuetoprocess = mActiveQueue;
	mActiveQueue = (mActiveQueue + 1) % NUMEVENTQUEUES;
	mEventQueue[mActiveQueue].clear();

	//Process all events in queue
	//TODO: It may be necessary to optimize this and limit the time spent processing queues updates.
	//Do it if it is necessary here,as it is an optimization
	
	//LOOP - Process all messages in queue
	while(mEventQueue[queuetoprocess].size() > 0)
	{
		//Get the event to process (pointers managed by smart pointer EventDataPointer)
		EventDataPointer toprocess = mEventQueue[queuetoprocess].front();
		mEventQueue[queuetoprocess].pop_front();

		//Be sure this event type has relationship of listeners
		EventListenersMap::iterator mapitr = mEvTypeListenerMap.find(toprocess->GetEventType());
		//IF - Event type not found
		if(mapitr == mEvTypeListenerMap.end())
		{
			//SingletonLogMgr::Instance()->AddNewLine("GameEventManager::Update","ERROR:Attempt to update event without being registered first!!!",LOGEXCEPTION);
			//return false;
			continue;
		}

		//So far so good - get REFERENCE of list related to this event type, and update all listeners
		EventListenerList& listenerlist = (*mapitr).second;  //Temp variable (REFERENCE!)
		
		//LOOP - Update all listeners related to this event type
		for(EventListenerList::iterator itr = listenerlist.begin();itr != listenerlist.end();++itr)
		{
			//Call handling function. Remember that the "handle" function returns true if it "eats" the message
			if((*itr)->HandleEvent(*toprocess))
				break;
		}//LOOP END


	}//LOOP END

	return true;
}

//Sometimes is necessary to waste all events in course (restart of a level, changing of state, out of game...)
void GameEventManager::EmptyEventQueues()
{
	//LOOP - Search all event queues
	for(int i = 0; i <NUMEVENTQUEUES; i++)
	{
		mEventQueue[i].clear();
	}//LOOP END
}

//Add a listener to listener list related to event type. Create event type if not created before
bool GameEventManager::AddListener(IEventListener* const listenerptr, const GameEventType& eventtype)
{
	//Be sure this listener-eventtype mapping doesnt exist
	EventListenersMap::iterator mapitr = mEvTypeListenerMap.find(eventtype);
	//IF - Type not registered to any event
	if(mapitr == mEvTypeListenerMap.end())
	{
		EvListInsertResult insertresult = mEvTypeListenerMap.insert(
												EvListEntry(eventtype,EventListenerList())
													  );		

		//Double check insertion was correct
		if(insertresult.second == false)
		{
			SingletonLogMgr::Instance()->AddNewLine("GameEventManager::AddListener","Insertion of new event and related list failure!!!!!!",LOGEXCEPTION);
			return false;
		}
		//Store iterator for this position of event-listenerlist
		mapitr = insertresult.first;
	}

	//Be sure listener wasnt registered
	EventListenerList::iterator listeneritr;
	EventListenerList& listenerlist = (*mapitr).second;  //Temp variable (REFERENCE!)
	//LOOP - Search for this listener register
	for(listeneritr = listenerlist.begin(); listeneritr != listenerlist.end(); listeneritr++)
	{
		if(*listeneritr == listenerptr)
		{
			SingletonLogMgr::Instance()->AddNewLine("GameEventManager::AddListener","Attempt to add the same listener to same event type twice!",LOGEXCEPTION);
			return false;
		}
	}//LOOP END


	//Event type added (or registered previously) and it doesnt have this listener related to it:
	//Add listener to list
	(*mapitr).second.push_back(listenerptr);

	return true;
}

//Remove a listener from listener list related to an event type
bool GameEventManager::RemoveListener(IEventListener* const listenerptr, const GameEventType& eventtype)
{

	EventListenersMap::iterator mapitr;
	//LOOP - Search for eventtype requested and its listeners related to it
	for(mapitr = mEvTypeListenerMap.begin();mapitr != mEvTypeListenerMap.end();mapitr++)
	{
		//IF - Event type match
		if((*mapitr).first == eventtype)
		{
			//Search listeners for this event type
			EventListenerList::iterator listeneritr;
			EventListenerList& listenerlist = (*mapitr).second;  //Temp variable (REFERENCE!)
			//LOOP - Search the listener inside this relationship
			for(listeneritr = listenerlist.begin();listeneritr != listenerlist.end();listeneritr++)
			{
				//IF - Listener is the one asked to remove
				if(*listeneritr == listenerptr)
				{
					listenerlist.erase(listeneritr);
					return true;
				}
			}//LOOP END
		}
	}//LOOP END

	//If this point is reached, function didnt find what was asked
	SingletonLogMgr::Instance()->AddNewLine("GameEventManager::RemoveListener","Listener could not be removed, not found eventtype or listener!",LOGEXCEPTION);

	return false;
}

//Call handler for event directly when event is triggered, instead of waiting till next update
bool GameEventManager::TriggerEvent(EventDataPointer const & newevent)
{
	//Initialize iterator
	EventListenersMap::iterator mapitr = mEvTypeListenerMap.find(newevent->GetEventType());

	//IF - Is there something registered for this trigger?
	if(mapitr == mEvTypeListenerMap.end())
	{
		//SingletonLogMgr::Instance()->AddNewLine("GameEventManager::TriggerEvent","Intent to trigger non-registered event type",LOGEXCEPTION);
		return false;
	}
	
	//Call hander to all associated listeners
	EventListenerList::iterator listeneritr;
	EventListenerList& listenerlist = (*mapitr).second;  //Temp variable (REFERENCE!)
	bool processed = false;			//Processed tracking
	//LOOP - Process all listeners in list
	for(listeneritr = listenerlist.begin();listeneritr != listenerlist.end();listeneritr++)
	{
		//Call handler function - if processed is true, store it
		if((*listeneritr)->HandleEvent(*newevent))
			processed = true;
	}//LOOP END
	
	return processed;
}

//Queue an event to process in timed update
bool GameEventManager::QueueEvent(EventDataPointer const & newevent)
{
	//Assertions
	assert (mActiveQueue>=0 && mActiveQueue < NUMEVENTQUEUES);

	//Check event type exists
	EventListenersMap::iterator mapitr = mEvTypeListenerMap.find(newevent->GetEventType());
	//IF - Event type not found
	if(mapitr == mEvTypeListenerMap.end())
	{
		//SingletonLogMgr::Instance()->AddNewLine("GameEventManager::QueueEvent","There are no listeners for an event type",LOGEXCEPTION);
		return false;
	}

	//Queue event in current active queue
	mEventQueue[mActiveQueue].push_back(newevent);

	return true;
}

//Aborts an event of a given type of the current processing queue. It can delete all events of a given
//type, optionally
bool GameEventManager::AbortEvent(const GameEventType& typetoabort, bool alloftype)
{	
	//Assertions
	assert (mActiveQueue>=0 && mActiveQueue < NUMEVENTQUEUES);
	
	//Local variables
	bool eventaborted = false;

	//Check event type exists
	EventListenersMap::iterator mapitr = mEvTypeListenerMap.find(typetoabort);
	//IF - Event type not found
	if(mapitr == mEvTypeListenerMap.end())
	{
		SingletonLogMgr::Instance()->AddNewLine("GameEventManager::AbortEvent","Intent to abort non-registered event type",LOGEXCEPTION);
		return false;
	}

	//LOOP - Search for the event and cancel (delete) it
	
	for(EventsList::iterator itr = mEventQueue[mActiveQueue].begin();
		itr != mEventQueue[mActiveQueue].end(); 
		itr++)
	{
		//IF - Event asked for is this
		if((*itr)->GetEventType() == typetoabort)
		{
			//Erase it
			itr = mEventQueue[mActiveQueue].erase(itr);
			eventaborted = true;
			//IF - Delete all elements of this type
			if(!alloftype)
				break;
		}
	}//LOOP END

	return eventaborted;
}


