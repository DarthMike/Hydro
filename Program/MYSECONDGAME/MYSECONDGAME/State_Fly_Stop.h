/*
	Filename: State_Fly_Stop.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for STOP (Fly State Machine)
	Comments: In this state, the fly stops "sticked" to the element which made it stop. It plays an Idle animation also,
			  and in case the object it is sitting moves, or case a random time is elapsed, it changes state
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _STATE_FLY_STOP
#define _STATE_FLY_STOP

//Library dependecies

//Class dependencies
#include "IAIState.h"
#include "AIAgent.h"
#include "Math.h"

class State_Fly_Stop : public IAIState<AIAgent>
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	State_Fly_Stop(AIAgent* actor):
    IAIState(actor)
	{
	}
	~State_Fly_Stop()
	{}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	//Interface implementation
	virtual void Update(float dt);   
	virtual void EnterAction();
	virtual void ExitAction();
	//----- PUBLIC VARIABLES ------
protected:
	//----- INTERNAL VARIABLES -----
	//----- INTERNAL FUNCTIONS -----

};

#endif
