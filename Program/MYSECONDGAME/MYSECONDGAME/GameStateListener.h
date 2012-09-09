/*
	Filename: GameStateListener.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Event listener for Game state manager
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _GAMESTATELISTENER
#define _GAMESTATELISTENER
//Library dependencies

//Class dependencies
#include "GameEventManager.h"

//Forward declarations
class GameStateManager;
class EventData;

class GameStateListener : public IEventListener
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameStateListener(GameStateManager* statemgrptr)
		:mName("GameStateListener"),
		mGameStateMgr(statemgrptr)
	{
		//Register events to process
		SingletonGameEventMgr::Instance()->AddListener(this,Event_ExitGame);  //Exit game event
	}
	~GameStateListener()
	{
		//Register events to process
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
	GameStateManager* mGameStateMgr;
	//----- INTERNAL FUNCTIONS -----
};

#endif
