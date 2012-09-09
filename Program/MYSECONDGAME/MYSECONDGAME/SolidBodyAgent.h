/*
	Filename: SolidBodyAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Solid body agent class
	Comments: It is a game agent which is a solid body: Triangle, circle, or more complicated!
			  It follows a physical simulated body, and displays the position using an associated entity
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*	TODO:
		- ADD SAVE AND LOAD CAPABILITIES; NOW IT IS NOT TOP PRIORITY FOR MY GAMES...
*/

#ifndef _SOLIDBODYAGENT
#define _SOLIDBODYAGENT

//Library dependencies
#include <assert.h>
#include <list>
//Classes dependencies
#include "IAgent.h"
#include "Shared_Resources.h"
#include "GFXDefs.h"

//Properties to contain from agent - Inherited
struct SolidBodyPar : public GameAgentPar
{
		SolidBodyPar():
		GameAgentPar(),
		material(GENERIC),
		wetness(0.0f),
		physicbody(NULL)
		{
			//Determine type
			type = PHYSICBODY;
		}
		std::list<ContainedSprite> gfxentities;
		b2Body* physicbody;
		MaterialType material;
		float wetness;
		//Assignment operator for fast copying
		SolidBodyPar& operator=(const SolidBodyPar &newparams)
		{
			//Deep copy of elements in struct
			position = newparams.position;
			rotation = newparams.rotation;
			gfxentities = newparams.gfxentities;
			physicbody = newparams.physicbody;
			material = newparams.material;
			wetness = newparams.wetness;
			return (*this);
		}
};

class SolidBodyAgent : public IAgent
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	SolidBodyAgent(PhysicsManagerPointer physicsptr):
	  mPhysicsManager(physicsptr),
	  mGlobalScale(1.0f),
	  mResY(800.0f),
	  mActive(false),
	  mCounter(0.0f),
	  mPlayerCollisions(0),
	  mInitialFriction(0.0f),
	  mOutOfLimits(false)
	{
		_init();
	};
	virtual ~SolidBodyAgent(){ _release(); }
	//----- VALUES GET/SET ---------------
	virtual AgentType GetType() { return mParams.type; }						//Get type of agent
	virtual bool IsAlive()  { return mActive; }             //Get if agent was destroyed
	const SolidBodyPar& GetAgentInfo() { return mParams; }
	//----- OTHER FUNCTIONS --------------
	virtual void UpdateState(float dt);								//Update object status
	virtual bool HandleCollision(const CollisionEventData& data);	//Process possible collisions
	virtual bool HandleEvent(const EventData& data);				//Process possible events
	virtual void HandleOutOfLimits(const OutOfLimitsEventData& data);//Handle out of limits
	virtual void Create( const GameAgentPar *params);				//Create from params
	virtual void Destroy();											//Destroy body

protected:
	//---- INTERNAL VARIABLES ----
	PhysicsManagerPointer mPhysicsManager;	//PhysicsManager pointer
	SolidBodyPar mParams;					//All parameters needed to create the agent
	bool mActive;							//Internal "active" tracking
	float mGlobalScale;						//Global scale to draw entities
	float mResY;							//Resolution of screen in Y axis to draw entities
	bool mOutOfLimits;						//"Out of Limits" tracking

	float mCounter;								//Filtering counter
	int mPlayerCollisions;						//Counter to track player number of collisions
	float mInitialFriction;						//Initial friction tracking
	static const float mTriggerCollisionVel;	//Trigger force to play events (sounds etc)
	static const float mTriggerDragForce;		//Trigger force to play events (sounds etc)
	static const float mCollisionFilter;		//Filtering time to send events (sounds etc)
	static const ColorHSLA mWetTintColor;		//Color to tint to when body gets wet
	//---- INTERNAL FUNCTIONS ----
	void _init();							//Init internal resources
	void _release();						//Release internal resources
};

#endif
