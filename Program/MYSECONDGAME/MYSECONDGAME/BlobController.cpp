/*
	Filename: BlobController.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Blob physical controller to abstract general actions in the character
	Comments: Related strictly to Box2D!
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "BlobController.h"
#include "PhysicsManager.h"
#include "PhysicsEvents.h"
#include "GameEvents.h"
#include "Math.h"
#include <sstream>

//Constant variables definitions
const float BlobController::BLOBBROKENTOLERANCE = 0.4f;

//Update bodies control
void BlobController::Update(float dt)
{
	//IF - Controller active
	if(mActive)
	{
		//IF  - Move command and physics stepped
		if(mPhysicsMgr->IsPhysicsStepped()
		   &&
		   mMoveCommand)
		{
			assert(mCenterBody);
			//IF - Blob "Not flying"
			if(mBoundingCollisions > 3)  //3 Small bodies touching as minimum!
			{
				//IF - Speed not maximum
				if(mCurrentSpeed.Length() < mMaxSpeed)
				{
					//Get direction in b2Vec
					b2Vec2 movforce(static_cast<float32>(mMoveDirection.x),
									static_cast<float32>(mMoveDirection.y));
					movforce*=mMaxControlForce; //Scale to control force
					mCenterBody->ApplyImpulse(movforce,mCenterBody->GetPosition());
				}//IF	
			}//IF
			mMoveCommand = false;
		}//IF

		//Update speed tracking
		b2Vec2 speed = mCenterBody->GetLinearVelocity();
		mCurrentSpeed = Vector2(static_cast<double>(speed.x),
								  static_cast<double>(speed.y));


		//Check "blob broken"
		if(_blobBroken())
		{
			mIntegrity = 0.0f;
			#ifdef _DEBUGGING
			std::stringstream ss;
			ss<<"\nBLOB IS BROKEN!!!";
			DebugStringInfo themessage(ss.str());
			SingletonGameEventMgr::Instance()->QueueEvent(
													EventDataPointer(new DebugMessageEvent(Event_DebugString,themessage))
													);
			#endif
		}//IF

		//Collision damage filtering update
		//IF - A collision occured which damaged blob
		if(mDamaged)
		{
			mDamageFilterCounter += dt;
		}//IF
		//IF - Damage filtering passed (no more tracking of "Damaged")
		if(mDamageFilterCounter >= mInitialParams.damagefiltertime)
			mDamaged = false;
		//Health limits checking
		//IF - Health not in limits
		if(mIntegrity < 0.0f)
			mIntegrity = 0.0f;	
		else if(mIntegrity > 100.0f)
			mIntegrity = 100.0f;

	}//IF
}
//Call to start logic of controller (finished creation)
void  BlobController::StartControlling(bool ismainblob) 
{
	//Ready to start controlling
	if(!mDestroyed)
	{
		mIsMainBlob =  ismainblob;
		mActive = true;
		//Update variables needed to operate correctly
		mDamageFilterCounter = mInitialParams.damagefiltertime;
	}
} 

//Call to stop controlling
void BlobController::StopControlling()
{
	mActive = false;	
}

//Call to sleep bodies
void BlobController::Sleep()
{
	BodiesVectorIterator it;
	//LOOP - Get maximum joints length
	for(it = mBodiesVector.begin(); it != mBodiesVector.end(); ++it)
	{	
		//Sleep body
		(*it)->PutToSleep();
	}//LOOP END

	mCenterBody->PutToSleep();
}
// Set the important center body and its related bounding sensor
void BlobController::SetCenterBody(b2Body* body) 
{ 
	assert(body); 
	//Copy body pointer
	mCenterBody = body;
} 

//Add a body to blob definition
void BlobController::AddBodyToControl(b2Body* body)
{
	assert(body);
	//Add to dynamic vector
	mBodiesVector.push_back(body);
}

//Add a joint created (used in creation step)
void BlobController::AddJointToList(b2DistanceJoint* joint)
{
	assert(joint);
	//Add to dynamic vector
	mJointsVector.push_back(joint);
}

//Called to finish control and destroy related bodies and joints
void BlobController::Destroy()
{
	//IF - Not already destroyed
	if(!mDestroyed)
	{	
		//Store position of blob before destroying
		b2Vec2 position = mCenterBody->GetPosition();

		//---Call destroy for every body---
		BodiesVectorIterator it;
		//LOOP - Destroy bodies
		for(it = mBodiesVector.begin(); it != mBodiesVector.end(); ++it)
		{
			mPhysicsMgr->DestroyBody((*it));
		}
		//LOOP END
		//Destroy center body
		if(mCenterBody)
		{
			mPhysicsMgr->DestroyBody(mCenterBody);
		}
		//------

		//Clear references
		mCenterBody = NULL;
		mBodiesVector.clear();	//Clear bodies vector (all destroyed by manager)
		mJointsVector.clear(); //Clear joints vector (implicitly destroyed)
		
		//Update controller state to destroyed
		mDestroyed = true;
		mIntegrity = 0.0f;
		mActive =  false;

		//Send "Blob dead" event
		BlobDeathInfo eventinfo(Vector2(position.x,position.y),mIsMainBlob,mCurrentRadius);

		if(mAffectWhenDying)
		{
			b2AABB bb;
			bb.lowerBound = b2Vec2(position.x - mCurrentRadius,position.y - mCurrentRadius);
			bb.upperBound = b2Vec2(position.x + mCurrentRadius,position.y + mCurrentRadius);
			assert(bb.IsValid());

			//Query for bodies affected by destroying (make them wet)
			eventinfo.affectedbodies = mPhysicsMgr->QueryforBodies(bb,false);  //Dont include static bodies
		}
		
		SingletonGameEventMgr::Instance()->QueueEvent(
														EventDataPointer(new BlobDeathEvent(Event_BlobDeath,eventinfo))
													);
	}//IF
}

//Call to know if the blob is about to "die"
bool BlobController::IsIntegrityVeryLow()
{
	return (mIntegrity>25.0f?false:true);
}

//Command to move blob
void BlobController::MoveBlob(const Vector2& direction)
{
	mMoveDirection = direction;
	mMoveCommand = true;
}

//Command to move by force the blob
void BlobController::ApplyImpulseToBlob(const Vector2& force)
{
	//IF  - Controller active
	if(mActive)
	{
		assert(mCenterBody);

		//Get force
		b2Vec2 movforce(static_cast<float32>(force.x),
						static_cast<float32>(force.y));
		//Apply to center body
		mCenterBody->ApplyImpulse(movforce,mCenterBody->GetPosition());
		
		//Apply to outer bodies
		BodiesVectorIterator it;
		//LOOP - Apply force to outer bodies (scaled)
		for(it = mBodiesVector.begin(); it != mBodiesVector.end(); ++it)
		{
			b2Body* body = (*it);
			body->ApplyImpulse(movforce,body->GetPosition());
		}//LOOP END

	}//IF
}

//Collision handling
bool BlobController::HandleCollision(const CollisionEventData& data)
{
	//IF - Controller active
	if(mActive)
	{
		//Get the collision data
		const ContactInfo& collisioninfo = data.GetCollisionData();

		//Receiving of physics events:
		//Check type of event
		//IF - Events types
		if(data.GetEventType() == Event_NewCollision)
		{		
			//Update bounding box collision detection tracking
			//IF - Separation is zero or penetrated
			if(collisioninfo.separation <= 0)
			{	
				//IF - Collision with external bodies
				if(//Collision with external bodies only
					 collisioninfo.agent1 != collisioninfo.agent2
					&&//Collision with shapes not being sensors (physical)
					!collisioninfo.collidedshape1->IsSensor()
					&&
					!collisioninfo.collidedshape2->IsSensor()
					)
				{
					mBoundingCollisions++;
					//IF - Collision damage is to be applied
					if(mApplyCollisionDamage)
					{
						//IF - Max speed surpassed
						if(collisioninfo.relvelocity.Length() >= 3.5f*mMaxSpeed)
						{
							//Destroy directly
							mIntegrity = 0.0f;
							//Collision damage filtering variables reset
							mDamaged = true;  
							mDamageFilterCounter = 0.0f;
						}//IF
					}//IF
				}//IF
			}//IF
		}
		else if(data.GetEventType() == Event_PersistantCollision)
		{
		}
		else if(data.GetEventType() == Event_DeletedCollision)
		{
			//Update bounding box collision detection tracking
			if(collisioninfo.separation <= 0)
			{
				//IF - Collision with external bodies
				if( //Collision with external bodies only
					collisioninfo.agent1 != collisioninfo.agent2
					&&//Collision with shapes not being sensors (physical)
					!collisioninfo.collidedshape1->IsSensor()
					&&
					!collisioninfo.collidedshape2->IsSensor()
					)
				{
					mBoundingCollisions--;
				}//IF
			}//IF
		}
		else if((data.GetEventType() == Event_CollisionResult) 
			    && mApplyCollisionDamage)
		{
			//Debug info
			//Collision result -> Update damage (if necessary)
			//IF - External collision
			if(collisioninfo.agent1 != collisioninfo.agent2
			   )
			{	
				//Compute value of collision force
				float collisionforce(collisioninfo.normalimpulse);
			
				/*if(collisionforce >= mDamageForce * 0.2)
				{
					#ifdef _DEBUGGING
					std::stringstream ss;
					ss<<"CollisionForce"<<collisionforce;
					DebugStringInfo themessage(ss.str());
					SingletonGameEventMgr::Instance()->QueueEvent(
													EventDataPointer(new DebugMessageEvent(Event_DebugString,themessage))
													);
					#endif
				}*/


				//IF - Max impulse - And collision filtering time passed
				if(collisionforce >= mDamageForce
					&&
					mDamageFilterCounter >= mInitialParams.damagefiltertime)
				{
					//IF - Collision is in blob boby
					if(IsCollisionInBlobBody(collisioninfo))
					{
						//Call internal function
						_handleDamage(collisionforce);
						//Collision damage filtering variables reset
						mDamaged = true;  
						mDamageFilterCounter = 0.0f;
					}//if
				}//IF
			}//IF
		}//IF
	}//IF

	return true;  //Processed
}

//Internal handle damage function
void BlobController::_handleDamage(float amount)
{
	if(mIntegrity <= 0)
		return;

	//Apply damage
	//IF - "Big" collision
	if(amount >= 3*mDamageForce)
	{
		mIntegrity = 0.0f;
	}
	else if(amount >= 1.5*mDamageForce)
	{
		mIntegrity -= 20.0f;
	}
	else//ELSE - Not so "big" collision
	{
		mIntegrity -= 10.0f;
	}

	//Reduce size of blob
	JointsVectorIterator it;
	float totallength = mInitialParams.radius;
	float toreduce = (amount>1.5*mDamageForce) ? mInitialParams.radius/10.0f : mInitialParams.radius/30.0f;
	//LOOP - Set tocenter joints length
	for(it = mJointsVector.begin(); it != mJointsVector.end(); ++it)
	{	
		//IF - Joint is to center body (radius)
		if((*it)->GetBody1() == mCenterBody 
			||
		   (*it)->GetBody2() == mCenterBody )
		{
			(*it)->m_length -= toreduce;
			
			//Check length is in a minimum
			//IF - Length is smaller than a minimum
			if((*it)->m_length < 0.10f * totallength)
			{
				//Just keep the minimum value
				(*it)->m_length = 0.10f * totallength;
			}

			//Store radius tracking
			mCurrentRadius = (*it)->m_length;
		}
		assert((*it)->m_length > 0.0f);
	}//LOOP END

	//Deactivate controller if dead
	if(mIntegrity <=0.0f)
	{
		mIntegrity = 0.0f;
		mActive = false;
	}
}

//Internal health function
void BlobController::_handleHealth()
{
	if(mIntegrity >= 100.0f)
		return;

	//IF - Not maximum health
	if(mIntegrity != 100.0f)
	{
		mIntegrity += 10.0f;
	}
	
	//IF - Maximum health exeeded
	if(mIntegrity> 100.0f)
		mIntegrity = 100.0f;

	//Increment size of blob
	JointsVectorIterator it;
	float totallength = mInitialParams.radius;
	float toincrement = mInitialParams.radius/30.0f;

	//LOOP - Set tocenter joints length
	for(it = mJointsVector.begin(); it != mJointsVector.end(); ++it)
	{	
		//IF - Joint is to center body (radius)
		if((*it)->GetBody1() == mCenterBody 
			||
		   (*it)->GetBody2() == mCenterBody )
		{
			//Set the length to wanted
			(*it)->m_length += toincrement;
			//Check length is where it should
			//IF - Length is bigger than start
			if((*it)->m_length > totallength)
			{
				//Just keep the minimum value
				(*it)->m_length = totallength;
			}

			//Store radius tracking
			mCurrentRadius = (*it)->m_length;
		}
		assert((*it)->m_length > 0.0f);
	}//LOOP END
}


//Internal check function
bool BlobController::IsCollisionInBlobBody(const ContactInfo& collisioninfo)
{
	//IF - Check center body first
	if(collisioninfo.collidedbody1 == mCenterBody || collisioninfo.collidedbody2 == mCenterBody)
		return true;

	BodiesVectorIterator it;
	//LOOP - Search bodies
	for(it = mBodiesVector.begin(); it != mBodiesVector.end(); ++it)
	{
		if((*it) == collisioninfo.collidedbody1 || (*it) == collisioninfo.collidedbody2)
			return true;
	}
	//LOOP END

	return false;
}

//Check if a body belongs to blob
bool BlobController::IsBodyInBlob(b2Body* thebody)
{
	if(thebody == mCenterBody)
		return true;

	BodiesVectorIterator it;
	//LOOP - Search bodies
	for(it = mBodiesVector.begin(); it != mBodiesVector.end(); ++it)
	{
		if((*it) == thebody)
			return true;
	}
	//LOOP END

	return false;
}

bool BlobController::_blobBroken()
{
	//Local variables
	int bodiescount(mInitialParams.bodies);
	int bodxbigger(0);
	int bodybigger(0);
	int bodxsmaller(0);
	int bodysmaller(0);
	
	//Check position of bodies to see if center body "broke through" skin of blob
	b2Vec2 centerpos (mCenterBody->GetPosition());
	BodiesVectorIterator itr;
	//LOOP - Check blob is not broken
	for(itr = mBodiesVector.begin(); itr != mBodiesVector.end();++itr)
	{
		b2Vec2 bodypos((*itr)->GetPosition());
		if(bodypos.x  > (centerpos.x + BLOBBROKENTOLERANCE))
			++bodxbigger;
		if(bodypos.x < (centerpos.x - BLOBBROKENTOLERANCE))
			++bodxsmaller;
		if(bodypos.y > (centerpos.y + BLOBBROKENTOLERANCE))
			++bodybigger;
		if(bodypos.y < (centerpos.y - BLOBBROKENTOLERANCE))
			++bodysmaller;
	}
	//LOOP

	bool broken (bodxbigger == bodiescount 
		     || 
			 bodybigger == bodiescount
			 ||
			 bodxsmaller == bodiescount
			 ||
			  bodysmaller == bodiescount);
	return(broken);
}