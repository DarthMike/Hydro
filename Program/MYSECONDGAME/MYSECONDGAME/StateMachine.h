/*
	Filename: StateMachine.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Base class for states machines of AI agents
	Comments: A class implementing a state machine, has to derive from this one, and implement 
			  an internal function which will associate states with names. Each state will be a different class, and
			  this class serves as interface and framework for state machines.
			  This class is templated to serve as base for any kind of object Agent implementation
	Attribution: It is loosely based in two books implementation of a flexible state machine:
				 "Game coding complete"  http://www.mcshaffry.com/GameCode/
				 "Programming Game AI by Example" http://www.ai-junkie.com/books/toc_pgaibe.html
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _STATEMACHINE
#define _STATEMACHINE

//Library dependencies
#include <string>
//Class dependencies
#include "IAIState.h"
#include "LogManager.h"

template <class ActorType>
class StateMachine
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	StateMachine(ActorType* actor):
	  mActor(actor),
	  mCurrentState(NULL)
	{
	}
	~StateMachine()
	{
		//Cleanup
		if(mCurrentState)
		{
			delete mCurrentState;
			mCurrentState = false;
		}
	}
	//----- GET/SET FUNCTIONS -----
	void SetState(const std::string& statename);   //Sets a new state for machine
	//----- OTHER FUNCTIONS -----
	//Interface for updating of states
	void Update(float dt);					//Update call 
	void Stop();							//Stop state machine (calling exit state of current state)
	//----- PUBLIC VARIABLES ------
protected:
	//----- INTERNAL VARIABLES -----
	ActorType* mActor;				//The actor pointer
	IAIState<ActorType>* mCurrentState;		//The current state
	//----- INTERNAL FUNCTIONS -----
	//Needs to be implemented
	virtual void _createState(const std::string& statename)=0;  //Creates a state given a state name (string) - it is specific of every state machine
};


//----------------Implementations----------------------------------------------------
//Sets a new state for machine
template <class ActorType>
void StateMachine<ActorType>::SetState(const std::string& statename)
{
	//Stop current state
	Stop();

	//Create a new state (it is a call with operator new to a new state class)
	//This is virtual method - will call corresponding method in class
	_createState(statename);

	//Debug info only
	SingletonLogMgr::Instance()->AddNewLine("StateMachine::SetState","New state for statemachine is '" + statename + "'",LOGDEBUG);
}

//Update call 
template <class ActorType>
void StateMachine<ActorType>::Update(float dt)
{
	//IF - Exists state
	if(mCurrentState)
	{
		//Call its logic
		mCurrentState->Update(dt);
	}
}

//Stop calls to current state 
template <class ActorType>
void StateMachine<ActorType>::Stop()
{
	//IF - Exists state
	if(mCurrentState)
	{
		mCurrentState->ExitAction();
		delete mCurrentState;
		mCurrentState = NULL;
	}
}

#endif
