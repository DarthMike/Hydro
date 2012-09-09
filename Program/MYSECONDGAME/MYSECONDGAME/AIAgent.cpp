/*
	Filename: AIAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Agent controlled by AI
	Comments: Internally, it holds a state machine, and a solid body agent
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "AIAgent.h"
#include "PhysicsEvents.h"
#include "GameEvents.h"
#include "Creatable_StateMachines.h"
#include "PhysicsManager.h"
//TODO: ONLY INCLUDED FOR DEBUG PURPOSES
#include "IndieLibManager.h"


//Update object status
void AIAgent::UpdateState(float dt)
{
	//IF - Agent is active
	if(mActive)
	{
		//Update state machine
		if(mStateMachine)
		{
			mStateMachine->Update(dt);
		}
		//Call internal solid body agent update
		mSolidBodyAgent.UpdateState(dt);

	_updateBodyData();

	_Seek(dt);
	//_Wander(dt);
	}//IF
}

//Process possible collisions
bool AIAgent::HandleCollision(const CollisionEventData&)
{
	return true;
}

//Handle possible events
bool AIAgent::HandleEvent(const EventData& data)
{
	bool eventprocessed =  false;

	if(data.GetEventType() == Event_NewTarget)
	{
		const NewTargetEvent& newtargetdata = static_cast<const NewTargetEvent&>(data);
		Vector2 newposition = newtargetdata.GetTargetPosition();
		mTarget.Set(static_cast<float32>(newposition.x),
					static_cast<float32>(newposition.y));

		printf("\nRECEIVED COMMAND TO MOVE FLY!");
		eventprocessed = true;
	}

	return eventprocessed;
}

//Handle out of limits
void AIAgent::HandleOutOfLimits(const OutOfLimitsEventData&) 
{ 
	//Just Destroy agent
	Destroy(); 
}	

//Create from params
void AIAgent::Create( const GameAgentPar *params)	
{
	
	//Agent params should be of type
	//Copy parameters from creation 
	assert(params->type == AI);
	
	//Cast down params hierarchy to this type of params
	const AIAgentPar *aiparams = static_cast<const AIAgentPar*>(params);
	//Copy all parameters
	mParams = *aiparams;

	//Manual creation of parameters for solid body contained
	SolidBodyPar bodyparams;
	bodyparams.gfxentities = aiparams->gfxentities;
	bodyparams.physicbody = aiparams->physicbody;
	bodyparams.position = aiparams->position;
	bodyparams.rotation = aiparams->rotation;
	
	//Creation of body agent contained
	mSolidBodyAgent.Create(&bodyparams);

	//Finally, update internal tracking
	mActive = true;
}

//Destroy agent
void AIAgent::Destroy()
{
	//Stop state machine
	if(mStateMachine)
	{
		mStateMachine->Stop();
	}
	
	//Destroy agents
	mSolidBodyAgent.Destroy();
	mParams.physicbody = NULL;  //Nullify reference to b2Body (destroyed by solidbody agent)

	//Finally, update internal tracking
	mActive = false;
}

//State machine class instantiation
void AIAgent::_createStateMachine( const std::string& agentAI )
{
	//Association of text property and AI state machine instantiation
	if(agentAI == "Fly")
	{
		mStateMachine = new StateMachine_Fly(this);	
	}
	else
	{
		SingletonLogMgr::Instance()->AddNewLine("AIAgent::_createStateMachine","Error: Bad Agent AI state machine name! Agent will not make anything!",LOGEXCEPTION);
	}
}

//Memory cleanup
void AIAgent::_release()
{
	//Cleanup memory
	if(mStateMachine)
	{
		delete	mStateMachine;
		mStateMachine = NULL;
	}
}

//Updates all body info needed for calculations
void AIAgent::_updateBodyData()
{
	//Update internal coordinates tracking of body
	//Local orientation axis
	mDirectionAxisX = mParams.physicbody->GetWorldPoint(LOCALXAXIS);
	mDirectionAxisY = mParams.physicbody->GetWorldPoint(LOCALYAXIS);
}

void AIAgent::_Seek(float)
{
	static bool seekcalculated = false;

	//Just calculate seek actions after a physics step
	if(
		 mPhysicsMgr->IsPhysicsStepped()
		 &&
		 !seekcalculated
		 )
	{
		//Get body position - Position is steering thrust position
		b2Vec2 bodypos = mParams.physicbody->GetPosition();
		
		//Calculate desired velocity to reach target
		b2Vec2 linearvel = mParams.physicbody->GetLinearVelocity();
		b2Vec2 DesiredVel = mTarget - bodypos;
		DesiredVel.Normalize();  //Normalize vel
		DesiredVel*= mParams.maxlinearvelocity;//Scale it to max speed
		//Steering force in center of mass
		b2Vec2 steeringforce = DesiredVel - linearvel;
		Vector2 steerforce(steeringforce.x,steeringforce.y);
		//Truncate steering force to max value
		Vector2 truncatedforce = SingletonMath::Instance()->ClampVector2(steerforce,mParams.maxsteerforce);
		mParams.physicbody->ApplyForce(b2Vec2(static_cast<float32>(truncatedforce.x),static_cast<float32>(truncatedforce.y)),bodypos);
		mParams.physicbody->ApplyForce(b2Vec2(0.0,10.0),bodypos); //Apply gravity force TODO: HACKED!
		
		seekcalculated = true;
	}
	else
		seekcalculated = false;
}
void AIAgent::_Wander(float dt)
{
	//this behavior is dependent on the update rate, so this line must
	//be included when using time independent framerate.
	double JitterThisTimeSlice = WanderJitterPerSec * dt;

	//first, add a small random vector to the target's position
	mWanderTarget += Vector2(SingletonMath::Instance()->ClampedRandom() * JitterThisTimeSlice,
					   SingletonMath::Instance()->ClampedRandom() * JitterThisTimeSlice);

	//reproject this new vector back on to a unit circle
	mWanderTarget.Normalise();

	//increase the length of the vector to the same as the radius
	//of the wander circle
	mWanderTarget *= WanderRad;

	//move the target into a position WanderDist in front of the agent
	Vector2 target = mWanderTarget + Vector2(WanderDist, 0);

	//project the target into world space
	b2Vec2 localpoint(static_cast<float32>(target.x),
					  static_cast<float32>(target.y));
	b2Vec2 worldpoint = mParams.physicbody->GetWorldPoint(localpoint);

	//Calculate steering force
	b2Vec2 bodypos = mParams.physicbody->GetPosition();	
	b2Vec2 steeringforce = worldpoint - bodypos;
	mParams.physicbody->ApplyForce(steeringforce,bodypos);
}
