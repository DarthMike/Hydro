/*
	Filename: GameStateListener.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Event listener for Game state manager
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "GameStateListener.h"
#include "GameStateManager.h"
#include "GameEvents.h"

bool GameStateListener::HandleEvent(const EventData& theevent)
{
	
	return mGameStateMgr->_handleEvents(theevent);
}