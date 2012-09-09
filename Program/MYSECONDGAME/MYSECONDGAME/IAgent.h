/*
	Filename: IAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Abstract class to interface of game agent
	Comments: A game agent is anything that reacts to game logic: Physic box, enemy, triggerpoint, coins, bombs...
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*	TODO:
		- ADD SAVE AND LOAD CAPABILITIES; NOW IT IS NOT TOP PRIORITY FOR MY GAMES...
		- MESSAGE HANDLING IN INTERNAL MESSAGE SYSTEM IN GAME
*/

#ifndef _IAGENT
#define _IAGENT

//Library dependencies

//Classes dependencies
#include "Vector2.h"
#include "PhysicsEvents.h"
#include "GameEvents.h"
#include "GameLogicDefs.h"

//Definitions
enum AgentType{
	UNKNOWN = -1,
	PLAYER,
	PHYSICBODY,
	AI,
	COLLECTABLE,
	COUNT
};
//Properties to contain from agents - Inheritable to extend it!
struct GameAgentPar{
		GameAgentPar():
		position(0,0),
		rotation(0),
		type(UNKNOWN)
		{}
		AgentType type;
		Vector2 position;
		float rotation;
	};

class IAgent
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	IAgent()
	{
	};
	virtual ~IAgent(){}
	//----- VALUES GET/SET ---------------
	virtual AgentType GetType() = 0;		//Get the agent type
	virtual bool IsAlive() = 0;             //Get if agent was destroyed
	//----- OTHER FUNCTIONS --------------
	virtual void UpdateState(float) = 0;								//Update object status
	virtual bool HandleCollision(const CollisionEventData&)=0;	//Process possible collisions
	virtual bool HandleEvent(const EventData&)=0;						//Process possible events	
	virtual void HandleOutOfLimits(const OutOfLimitsEventData&)=0;							//Process out of limits
	virtual void Create( const GameAgentPar*) = 0;				//Create from params
	virtual void Destroy() = 0;											//Destroy

protected:
	//---- INTERNAL VARIABLES ----
	//---- INTERNAL FUNCTIONS ----
};

#endif
