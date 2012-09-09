/*
	Filename: PhysicsSimListener.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Event listener for game
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/	

#include "PhysicsSimListener.h"
#include "PhysicsSim.h"
#include "GameEvents.h"

bool PhysicsSimListener::HandleEvent(const EventData& theevent)
{
	
	return mGame->_handleEvents(theevent);
}