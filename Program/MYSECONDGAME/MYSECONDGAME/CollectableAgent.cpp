/*
	Filename: CollectableAgent.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: An agent which can be collected and sends event when collected
	Comments: In Hydro, are fellow water drops! ;)
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "CollectableAgent.h"
#include "IndieLibManager.h"
#include "PhysicsManager.h"
#include <sstream>

//Update object status
void CollectableAgent::UpdateState(float dt)
{
	//IF - Agent active
	if(!mActive)
	{
		mAlive = false;
		return;
	}

	//Update animations
	mAnimController->Update(dt);
	//In case of collision with player
	//IF - Collected (to drestroy)
	if(mCollected)
	{
		//Play collection animation
		mAnimController->SetNextAnimation(false,1,true);
		bool animstopped = mAnimController->IsAnimationStoppedAtEnd();
		bool currentanimok =  mAnimController->IsCurrentAnim(1);
		if(animstopped
			&&
			currentanimok
		   )
		{	
			Destroy();  //Self-Destroy (agent manager will delete memory)
		}
	}

	if(mOutOfLimits)
		Destroy();
}

//Process possible collisions
bool CollectableAgent::HandleCollision(const CollisionEventData&)
{
	return true;
}

//Process possible events
bool CollectableAgent::HandleEvent(const EventData& data)
{
	bool eventprocessed = false;
	
	//Collectable receives event of being collected
	//IF - "Drop Collected event"
	if(data.GetEventType() == Event_DropCollision)
	{
		//Check if this was collected
		const DropCollidedEvent& theevent = static_cast<const DropCollidedEvent&>(data);
		//IF - Pointers match
		if(theevent.GetCollectedAgent() == this && !mCollected)
		{
			//Drop collected!
			//Destroy body
			mPhysicsManager->DestroyBody(mParams.physicbody);
			mCollected = true; //Memorize collected
			//Send event as drop was collected
			SingletonGameEventMgr::Instance()->QueueEvent(
												EventDataPointer(new EventData(Event_DropCollected))
											);
		}	
	}//IF
	
	return eventprocessed;
}

//Handle out of limits
void CollectableAgent::HandleOutOfLimits(const OutOfLimitsEventData&) 
{ 
	mOutOfLimits = true;
}	

//Create from params
void CollectableAgent::Create( const GameAgentPar *params)	
{
	//Agent params should be of type
	//Copy parameters from creation 
	assert(params->type == COLLECTABLE);
	
	//Cast down params hierarchy to this type of params
	const CollectableAgentPar *pagentparams = static_cast<const CollectableAgentPar*>(params);
	//Copy all parameters
	mParams = *pagentparams;
	
	//Init internal variables
	mAnimController = AnimationControllerPointer(new AnimationController(mParams.sprite.gfxentity,0,true));
	//Finally, update internal tracking
	mActive = true;
}

//Destroy agent
void CollectableAgent::Destroy()
{
	//Finally, update internal tracking
	mActive = false;
}

//Init variables
void CollectableAgent::_init()
{
	//Init internal variables
	mGlobalScale = SingletonIndieLib::Instance()->GetGeneralScale();
	mResY = static_cast<float>(SingletonIndieLib::Instance()->Window->GetHeight());
}
//Release internal resources
void CollectableAgent::_release()
{
	//Deregister from indielib entity drawing is done by animation controller
}	
