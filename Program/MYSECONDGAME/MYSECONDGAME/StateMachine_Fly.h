/*
	Filename: FlyStateMachine.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Base class for states machines of AI agents
	Comments: Implementation of state machine class for a Fly (insect) AI agent
	Attribution: It is loosely based in two books implementation of a flexible state machine:
				 "Game coding complete"  http://www.mcshaffry.com/GameCode/
				 "Programming Game AI by Example" http://www.ai-junkie.com/books/toc_pgaibe.html
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _FLYSTATEMACHINE
#define _FLYSTATEMACHINE

//Library dependencies

//Class dependencies
#include "StateMachine.h"
#include "StateMachine_Fly_States.h"
#include "AIAgent.h"
#include "LogManager.h"


class StateMachine_Fly : public StateMachine<AIAgent>
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	StateMachine_Fly(AIAgent* actor):
	  StateMachine(actor)
	{
	}
	~StateMachine_Fly()
	{
	}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	//Interface for updating of states
	//----- PUBLIC VARIABLES ------
protected:
	//----- INTERNAL VARIABLES -----
	//----- INTERNAL FUNCTIONS -----
	//Implementation of "CreateState" Method
	virtual void _createState(const std::string& statename)  //Creates a state given a state name (string) - it is specific of every state machine
	{
		if(statename == "Stop")
		{
			mCurrentState = new State_Fly_Stop(mActor);
		}
		else if(statename == "Fly_Random")
		{

		}
		else
		{
			SingletonLogMgr::Instance()->AddNewLine("StateMachine_Fly::_createState","State name '" + statename + "' for a Fly is not correct! Program BUG!",LOGEXCEPTION);
		}
	}

};

#endif
