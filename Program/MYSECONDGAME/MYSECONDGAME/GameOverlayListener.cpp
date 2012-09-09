/*
	Filename: GameOverlayListener.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Event listener for overlay
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "GameOverlayListener.h"
#include "GameOverlay.h"
#include "GameEvents.h"

bool GameOverlayListener::HandleEvent(const EventData& theevent)
{
	
	return mOverlay->_handleEvents(theevent);
}