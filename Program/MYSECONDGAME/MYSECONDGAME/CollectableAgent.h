/*
	Filename: CollectableAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: An agent which can be collected and sends event when collected
	Comments: In Hydro, are fellow water drops! ;)
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*	TODO:
		- ADD SAVE AND LOAD CAPABILITIES; NOW IT IS NOT TOP PRIORITY FOR MY GAMES...
*/

#ifndef _COLLECTABLEAGENT
#define _COLLECTABLEAGENT

//Library dependencies

//Classes dependencies
#include "IAgent.h"
#include "Shared_Resources.h"
#include "AnimationController.h"
#include "GFXDefs.h"

//Forwar declarations
class b2Body;

//Definitions
//Properties to contain from agent - Inherited
struct CollectableAgentPar : public GameAgentPar
{
		CollectableAgentPar():
		GameAgentPar()
		{
			//Determine type
			type = COLLECTABLE;
		}
	
		ContainedSprite sprite;  //The graphics entity related
		b2Body* physicbody;				//Physics body related
		//Assignment operator for fast copying
		CollectableAgentPar& operator=(const CollectableAgentPar &newparams)
		{
			//Deep copy of elements in struct
			sprite = newparams.sprite;
			physicbody = newparams.physicbody;
			return (*this);
		}
};


class CollectableAgent : public IAgent
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	CollectableAgent(PhysicsManagerPointer physicsptr):
	  mPhysicsManager(physicsptr),
	  mGlobalScale(1.0f),
	  mResY(800.0f),
	  mActive(false),
	  mAlive(true),
	  mCollected(false),
	  mOutOfLimits(false)
	{
	}
	virtual ~CollectableAgent(){ _release(); }
	//----- VALUES GET/SET ---------------
	virtual AgentType GetType() { return mParams.type; }			//Get type of agent
	virtual bool IsAlive()  { return (mActive && mAlive); }             //Get if agent was destroyed
	//----- OTHER FUNCTIONS --------------
	//Interface implementations
	virtual void UpdateState(float dt);								//Update object status
	virtual bool HandleCollision(const CollisionEventData& data);	//Process possible collisions
	virtual bool HandleEvent(const EventData& data);				//Process possible events
	virtual void HandleOutOfLimits(const OutOfLimitsEventData& data);//Handle out of limits
	virtual void Create( const GameAgentPar *params);				//Create from params
	virtual void Destroy();											//Destroy body

protected:
	//---- INTERNAL VARIABLES ----
	CollectableAgentPar mParams;				//All parameters needed to create the agent
	bool mActive;							//Internal "active" tracking
	bool mAlive;							//Delayed "alive" signal, it is active delayed 1 logic tick
	float mGlobalScale;						//Global scale to draw entities
	float mResY;							//Resolution of screen in Y axis to draw entities
	bool mOutOfLimits;						//"Out of Limits" tracking

	PhysicsManagerPointer mPhysicsManager;			//Physics manager
	AnimationControllerPointer mAnimController;	//GFX entity animation controller
	bool mCollected;				//Tracking of collision with player
	//---- INTERNAL FUNCTIONS ----
	void _init();							//Init
	void _release();						//Release internal resources
};

#endif
