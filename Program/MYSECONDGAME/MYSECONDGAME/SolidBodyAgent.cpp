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

#include "SolidBodyAgent.h"
#include "Math.h"
#include "PhysicsManager.h"
#include "PhysicsEvents.h"
#include "GameEvents.h"
#include "GameEventManager.h"
#include "IndieLibManager.h"

//Definition of constants
const float SolidBodyAgent::mTriggerCollisionVel = 2.0f;	//Trigger force to play events (sounds etc)
const float SolidBodyAgent::mTriggerDragForce = 5.0f;		//Trigger force to play events (sounds etc)
const float SolidBodyAgent::mCollisionFilter = 50.0f;		//Filtering time to send events (sounds etc)
const ColorHSLA SolidBodyAgent::mWetTintColor = ColorHSLA(0,0.0f,0.71f,255);		//Color to tint to when body gets wet

//Update object status
void SolidBodyAgent::UpdateState(float dt)
{
	//IF - Agent is active
	if(!mActive)
		return;

	//Update internal timer
	mCounter += dt;

	//Read position of body
	b2XForm bodytransform (mParams.physicbody->GetXForm());
	
	std::list<ContainedSprite>::iterator itr;
	//LOOP - All sprites created
	for(itr = mParams.gfxentities.begin(); itr != mParams.gfxentities.end(); ++itr)
	{
		//IF - Related sprite exists
		if((*itr).gfxentity)
		{
			//Transform its position inside body
			//Get local transform offset
			b2Vec2 translation(static_cast<float>((*itr).posoffset.x),
								static_cast<float>((*itr).posoffset.y));
			b2Mat22 rotation(SingletonMath::Instance()->AngleToRadians((*itr).rotoffset));

			b2XForm spritetransform(translation,rotation);
			b2Vec2 finalpos (b2Mul(spritetransform,b2Vec2(0,0)));  //Apply sprite transform
			finalpos = b2Mul(bodytransform,finalpos);  //Apply body transform
			b2Mat22 finalrot = b2Mul(spritetransform.R,bodytransform.R);
			(*itr).gfxentity->SetAngleXYZ(0.0f,0.0f,SingletonMath::Instance()->RadiansToAngle<float>(finalrot.GetAngle(),true));
			Vector2 positionpix = SingletonIndieLib::Instance()->FromCoordToPix(Vector2(finalpos.x,finalpos.y));
			(*itr).gfxentity->SetPosition(static_cast<float>(positionpix.x),static_cast<float>(positionpix.y),0);
		}//IF
	}//LOOP END

	if(mOutOfLimits)
		Destroy();	
}

//Process possible collisions
bool SolidBodyAgent::HandleCollision(const CollisionEventData& data)
{
	if(!mActive)
		return false;

	//IF - New collision
	if(data.GetEventType() == Event_NewCollision)
	{
		const ContactInfo& collinfo (data.GetCollisionData());
			
		//First check if collision is between solid bodies - Sound playing
		//IF - "Between solid bodies" collision
		if(collinfo.agent1->GetType() == PHYSICBODY 
		   &&
		   collinfo.agent2->GetType() == PHYSICBODY
		   &&
		   mCollisionFilter <  mCounter
		   &&
		   mParams.physicbody->IsDynamic())
		{
			//IF - Velocity is fast enough to trigger collision
			if(collinfo.relvelocity.Length() > mTriggerCollisionVel)
			{
				//Send event
				SolidCollisionInfo data(mParams.material);
				SingletonGameEventMgr::Instance()->QueueEvent(
																EventDataPointer(new SolidCollisionEvent(Event_SolidCollision,data))
															  );
				mCounter = 0.0f;  //Reset timing
			}
		}//ELSE - Collision with player agent
		else if(
			collinfo.agent1->GetType() == PLAYER
			||
			collinfo.agent2->GetType() == PLAYER
			)
		{
			mPlayerCollisions ++;
		}//IF
	}//ELSE - Deleted collision
	else if(data.GetEventType() == Event_DeletedCollision)
	{
		const ContactInfo& collinfo (data.GetCollisionData());

		//IF - Collision with player agent
		if(
			collinfo.agent1->GetType() == PLAYER
			||
			collinfo.agent2->GetType() == PLAYER
			)
		{
			mPlayerCollisions --;
		}//IF
	}//IF

	return true;
}

//Process possible events
bool SolidBodyAgent::HandleEvent(const EventData& theevent)
{
	bool eventprocessed(false);

	if(!mActive)
		return false;

	//IF - Blob death event
	if(theevent.GetEventType() == Event_BlobDeath)
	{
		bool change(false);

		//Body was affected when blob died
		if(mParams.material == GENERIC)
		{
			//Hardcoded material params :( Just for rapid development and small mats properties for this game
			/* //Playtests: It is better to obtain max wetness in one blob death! Simpler mechanics
			mParams.wetness += 0.2f;   
			
			//Check limits
			if(mParams.wetness > 0.8f)
				mParams.wetness = 0.8f;
			*/
			mParams.wetness = 0.8f;
			change = true;
		}
		else if(mParams.material == WOODISH)
		{
			//Hardcoded material params :( Just for rapid development and small mats properties for this game
			/* //Playtests: It is better to obtain max wetness in one blob death! Simpler mechanics
			mParams.wetness += 0.3f;
			//Check limits
			if(mParams.wetness > 0.9f)
				mParams.wetness = 0.9f;
			*/
			mParams.wetness = 0.9f;
			change = true;
		}

		//IF - Atts changed
		if(change)
		{
			//Change friction of body according to wetness param
			mPhysicsManager->ChangeFrictionofBody(mParams.physicbody,mInitialFriction * (1 - mParams.wetness));

			//Tint body according to new wetness
			ColorHSLA newcolor = mWetTintColor;
			newcolor.lightness = ((1 - mWetTintColor.lightness) * (1 - mParams.wetness)) + mWetTintColor.lightness;
			ColorRGBA drawcolor = SingletonIndieLib::Instance()->FromHSLToRGB(newcolor);
		
			//Change all shapes attributes
			std::list<ContainedSprite>::iterator itr;
			//LOOP - All sprites created
			for(itr = mParams.gfxentities.begin(); itr != mParams.gfxentities.end(); ++itr)
			{
				//IF - Related sprite exists
				if((*itr).gfxentity)
				{
					(*itr).gfxentity->SetTint(static_cast<byte>(drawcolor.red),
											  static_cast<byte>(drawcolor.green),
											  static_cast<byte>(drawcolor.blue)
											  );
				}//IF
			}//LOOP END
		}//IF
	}
	return eventprocessed;
}

//Handle out of limits
void SolidBodyAgent::HandleOutOfLimits(const OutOfLimitsEventData&) 
{ 
	//Memorize event
	mOutOfLimits = true;
}	

//Create from params
void SolidBodyAgent::Create( const GameAgentPar *params)	
{
	//Agent params should be of type
	//Copy parameters from creation 
	assert(params->type == PHYSICBODY);
	
	//Cast down params hierarchy to this type of params
	const SolidBodyPar *pagentparams = static_cast<const SolidBodyPar*>(params);
	//Copy all parameters
	mParams = *pagentparams;
	
	//Set initial friction from FIRST SHAPE (we suppose simple bodies with one shape or with same friction for all shapes)
	mInitialFriction = mParams.physicbody->GetShapeList()->GetFriction();

	//Finally, update internal tracking
	mActive = true;
}

//Destroy agent
void SolidBodyAgent::Destroy()
{
	if (!mActive)
		return;

	//Destroy related body
	mPhysicsManager->DestroyBody(mParams.physicbody);	
	mParams.physicbody = NULL;
	//Finally, update internal tracking
	mActive = false;
}
//Init internal resources
void SolidBodyAgent::_init()
{
	//Init internal variables
	mGlobalScale = SingletonIndieLib::Instance()->GetGeneralScale();
	mResY = static_cast<float>(SingletonIndieLib::Instance()->Window->GetHeight());
}
//Release internal resources
void SolidBodyAgent::_release()
{
	//Release all sprites from IndieLib manager
	std::list<ContainedSprite>::iterator itr;

	//LOOP - All sprites created
	for(itr = mParams.gfxentities.begin(); itr != mParams.gfxentities.end(); ++itr)
	{
		//IF - Related sprite exists
		if((*itr).gfxentity)
		{
			//Deregister from indielib entity drawing
			SingletonIndieLib::Instance()->Entity2dManager->Delete((*itr).gfxentity.get());
		}//IF
	}//LOOP END
}