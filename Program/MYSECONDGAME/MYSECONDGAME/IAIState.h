/*
	Filename: IAIState.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which is interface class to states of an AI state machine
	Comments: This class is templated to serve as base for any kind of object Agent implementation
	Attribution: It is loosely based in two books implementation of a flexible state machine:
				 "Game coding complete"  http://www.mcshaffry.com/GameCode/
				 "Programming Game AI by Example" http://www.ai-junkie.com/books/toc_pgaibe.html
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _IAISTATE
#define _IAISTATE

//Library dependecies

//Class dependencies

template <class ActorType>
class IAIState
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	IAIState(ActorType* actor):
	  mActor(actor),
	  mIsDead(false),
	  mIsInitial(true)
	{
	}
	~IAIState()
	{}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	//Interface for updating of states
	virtual void Update(float dt)=0;   
	virtual void EnterAction()=0;
	virtual void ExitAction()=0;
	//----- PUBLIC VARIABLES ------
protected:
	//----- INTERNAL VARIABLES -----
	ActorType* mActor;				//The actor Id for controlled actor
	bool mIsDead;					//Tracking if state is dead
	bool mIsInitial;				//Tracking if it is the first time to enter state
	//----- INTERNAL FUNCTIONS -----

};

#endif
