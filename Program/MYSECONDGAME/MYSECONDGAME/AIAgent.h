/*
	Filename: AIAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Agent controlled by AI
	Comments: Internally, it holds a state machine, and a solid body agent
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*	TODO:
		- ADD SAVE AND LOAD CAPABILITIES; NOW IT IS NOT TOP PRIORITY FOR MY GAMES...
// TODO: ADDED TEMP REFERENCE TO PHYSICS MANAGER BECAUSE OF HACKS IN FORCES!! NEED TO ADD A FORCE CONTROLLER INSIDE
*/

#ifndef _AIAGENT
#define _AIAGENT

//Library dependencies
#include <assert.h>
//Classes dependencies
#include "IAgent.h"
#include "SolidBodyAgent.h"
#include "Math.h"

//Definitions
//Properties to contain from agent - Inherited
struct AIAgentPar : public SolidBodyPar
{
		AIAgentPar():
		SolidBodyPar(),
		agentAI("")
		{
			//Determine type
			type = AI;
		}
		
		//Agent AI name - for creation of state machine
		std::string agentAI;
		//Maximum parameters for moving
		float maxlinearvelocity;
		float maxsteerforce;
		//Assignment operator for fast copying
		AIAgentPar& operator=(const AIAgentPar &newparams)
		{
			//Deep copy of elements in struct
			position = newparams.position;
			rotation = newparams.rotation;
			gfxentities = newparams.gfxentities;
			physicbody = newparams.physicbody;	
			agentAI = newparams.agentAI;
			maxlinearvelocity = newparams.maxlinearvelocity;
			maxsteerforce = newparams.maxsteerforce;
			return (*this);
		}
};

//Definitions

//the radius of the constraining circle for the wander behavior
const float32 WanderRad    = 1.0f;
//distance the wander circle is projected in front of the agent
const float32 WanderDist   = 1.5f;
//the maximum amount of displacement along the circle each frame
const float32 WanderJitterPerSec = 40.0f;
//Axis definition
const b2Vec2 LOCALXAXIS(1,0);
const b2Vec2 LOCALYAXIS(0,1);

//Forward declarations
template <class ActorType> class StateMachine;  //The state machine
class PhysicsManager;

class AIAgent : public IAgent
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	AIAgent(PhysicsManagerPointer physicsptr):
    mPhysicsMgr(physicsptr),
	mSolidBodyAgent(physicsptr),
	mStateMachine(NULL),
	mLastCollisionInfo(NULL),
	mActive(false),
	mIsCollided(false),
	mDirectionAxisX(LOCALXAXIS),
	mDirectionAxisY(LOCALYAXIS),
	mTarget(10.0,7.0)   //TODO: TAKE TEST HACKS OUT OF THE WAY!
	{
		//stuff for the wander behavior
		double theta = static_cast<double>(SingletonMath::Instance()->NewRandom(0,180)) 
					   * SingletonMath::Instance()->Two_Pi;

		//create a vector to a target position on the wander circle
		mWanderTarget = Vector2(WanderRad * cos(theta),
									WanderRad * sin(theta));
	}
	virtual ~AIAgent()
	{
		//Release
		_release();
	}
	//----- VALUES GET/SET ---------------
	virtual AgentType GetType() { return mParams.type; }						//Get type of agent
	virtual bool IsAlive()  { return mActive; }             //Get if agent was destroyed
	StateMachine<AIAgent>* GetStateMachine() { return mStateMachine;} 
	//----- OTHER FUNCTIONS --------------
	virtual void UpdateState(float dt);								//Update object status
	virtual bool HandleCollision(const CollisionEventData& data);	//Process possible collisions
	virtual bool HandleEvent(const EventData& data);				//Process possible events
	virtual void HandleOutOfLimits(const OutOfLimitsEventData& data);	//Handle out of limits
	virtual void Create( const GameAgentPar *params);				//Create from params
	virtual void Destroy();											//Destroy

protected:
	//---- INTERNAL VARIABLES ----
	AIAgentPar mParams;						//All parameters needed to create the agent
	SolidBodyAgent mSolidBodyAgent;			//Internal solid body
	StateMachine<AIAgent>* mStateMachine;	//Internal state machine (depends on type of agent)
	bool mActive;							//Internal "active" tracking
	
	PhysicsManagerPointer mPhysicsMgr;

	ContactInfo* mLastCollisionInfo;		//Storing of collision data for last collision
	bool mIsCollided;						//Checks if agent is currently collided

	b2Vec2 mTarget;							//Target destination in global coords

	b2Vec2 mDirectionAxisX;					//Local direction axis for this agent
	b2Vec2 mDirectionAxisY;
	
	
	//---- INTERNAL FUNCTIONS ----
	void _createStateMachine( const std::string& agentAI );
	void _release();
	//Internal data updating
	void _updateBodyData();

	//Steering algorithms
	void _Wander(float dt);
	void _Seek(float dt);
	Vector2 mWanderTarget;
};

#endif
