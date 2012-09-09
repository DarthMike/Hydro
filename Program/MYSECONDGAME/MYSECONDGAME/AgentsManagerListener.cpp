/*
	Filename: AgentsManagerListener.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Listener for an agents manager
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "AgentsManagerListener.h"
#include "AgentsManager.h"
#include "PhysicsEvents.h"

bool AgentsManagerListener::HandleEvent(const EventData& theevent)
{
	//Forward request to Agent Manager
	return(mAgentsManager->_handleEvents(theevent));
}