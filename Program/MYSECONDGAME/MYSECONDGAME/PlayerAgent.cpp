/*
	Filename: PlayerAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: An agent for the game character the player controls
	Comments: In Hydro, it is a squishy water-ball ;)
	          It is a bit hacky in some parts, as well as a bit dirty for my liking, but I 
			  dont have time to make it better and I want to make something nice in a game,
			  not nice code only! ;)
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "PlayerAgent.h"
#include "IndieLibManager.h"
#include "PhysicsManager.h"
#include "GameEventManager.h"
#include "GameEvents.h"
#include "BlobBuilder.h"
#include "Camera2D.h"
#include <sstream>

//Update object status
void PlayerAgent::UpdateState(float dt)
{
	//Internal control variable
	static float aftershoottime = 0.0f;
	static bool singlecontrol = false;

	//IF - Agent is active
	if(!mActive)
	{
		mAlive = false;
		return;
	}

	//Update control delay variable
	if(mControlDelay >= 0.0f)
		mControlDelay -= dt;


	//Update graphics control
	b2Vec2 position;
	//IF - Second blob controlled
	if(mSecondControl && mSecondBlobController)
	{
		position = mSecondBlobController->GetCenterBody()->GetPosition();
		mParams.position = Vector2(position.x,position.y);
		b2Vec2 linvel = mSecondBlobController->GetCenterBody()->GetLinearVelocity();
		mLinearVel = Vector2(linvel.x,linvel.y);
		
	}
	else //ELSE - First blob controlled
	{
		 position = mBlobController->GetCenterBody()->GetPosition();
		 mParams.position = Vector2(position.x,position.y);
		 b2Vec2 linvel = mBlobController->GetCenterBody()->GetLinearVelocity();
		 mLinearVel = Vector2(linvel.x,linvel.y);
	}//IF
	
	//Call internal function to update IndieLib GFX
	_updateBlobGFX(dt);

	//Update animation controller
	mAnimController->Update(dt);

	//Events to change position
	BlobPositionInfo data(mParams.position, mParams.maxspeed, mLinearVel);

	SingletonGameEventMgr::Instance()->QueueEvent(
		EventDataPointer (new BlobPositionEvent(Event_BlobPosition,data))
		);
	
	//Update state of controllers
	//Allways update first blob
	mBlobController->Update(dt);
	
	//IF - Second blob controlled - Tracking of shooting and dying
	if(mSecondControl && mSecondBlobController)
	{
		//IF - Previous step was single control active (one blob)
		if(singlecontrol && aftershoottime < 2000.0f) 
		{
			aftershoottime += dt;
		}
		else if (!singlecontrol)//ELSE - Not shooting in process
		{
			//mBlobController->Sleep();
			singlecontrol = false;
			aftershoottime = 0.0f;

		}//IF

		//Update second blob controller
		mSecondBlobController->Update(dt);
		
		//IF - Blob destroyed
		if(mSecondBlobController->GetIntegrity() <= 0.0f)
		{
			mSecondControl = false;
			mSecondBlobController->Destroy(); //Call destroy for this controller
			mSecondBlobController.reset();
			mControlDelay = 1000.0f;  //Delay to response next control event
		}
	}// ELSE - First blob controlled
	else
	{
		singlecontrol = true;
		aftershoottime = 0.0f;
	}//IF
			
	//Update collisions with other blobs
	BlobCollisionList::iterator itr = mBlobCollisionsList.begin();

	//LOOP - Search all collisions with other blobs
	while(itr != mBlobCollisionsList.end())
	{
		//Update counters of collision if colliding
		//IF - Collided
		if((*itr).numcollisions > 0)
		{
			(*itr).timecollided += dt; 
		}//IF

		//IF - Check if blobs should merge
		if((*itr).timecollided > 500.0f)
		{
			(*itr).theotherblob->DisableAffectBodiesWhenDeath();  //Disable bodies affecting (merging, not dying in a "proper" way)
			(*itr).theotherblob->Destroy(); //Call destroy for this controller

			//Handle special case of second blob controller
			//IF - Second controlled was the collided one
			if((*itr).theotherblob == mSecondBlobController)
			{
				mSecondBlobController.reset(); //Delete shared data
				mSecondControl = false;
			}
			else //ELSE - Other blob than second controlled
			{
				//Delete from list of blobs
				BlobControllerList::iterator blobsitr;
				//LOOP - Find blob and delete it
				for(blobsitr = mBlobsList.begin(); blobsitr != mBlobsList.end();++blobsitr)
				{
					if((*blobsitr) == (*itr).theotherblob)
					{
						mBlobsList.erase(blobsitr);
						break;
					}
				}//LOOP END
			}//IF
		
			//Finally, delete this collision and continue
			itr = mBlobCollisionsList.erase(itr);
			mBlobController->ApplyHealth();	//Remerge with original causes increment in health
		}
		else//ELSE - Blobs shouldnt merge
		{
			++itr; //Increment iterator
		}//IF
	}//LOOP END

	//Send event to report change of health
	float integrity;
	if(mSecondControl && mSecondBlobController)
	{
		integrity = mSecondBlobController->GetIntegrity();
	}
	else
	{
		integrity = mBlobController->GetIntegrity();
	}
	//Report others of change of health
	BlobHealthInfo info(integrity,mBlobController->IsIntegrityVeryLow());
	SingletonGameEventMgr::Instance()->QueueEvent(
												  EventDataPointer(new BlobHealthEvent(Event_BlobHealth,info))
													);

	//Update and Check death of "scattered" blobs
	BlobControllerList::iterator blobitr = mBlobsList.begin();
	//LOOP - Search all blobs
	while(blobitr != mBlobsList.end())
	{
		//Update logic
		(*blobitr)->Update(dt);
		//IF - Blob died
		if((*blobitr)->GetIntegrity() <= 0.0f)
		{
			(*blobitr)->Destroy(); //Destroy
			blobitr = mBlobsList.erase(blobitr);
		}
		else //ELSE - Blob didnt die
		{
			 ++blobitr; //Search next
		}
	}//LOOP

	//Check if blob died (send game over if so)
	if(mBlobController->GetIntegrity() <= 0)
	{
		//Destroy player agent
		Destroy();
	}
}

//Process possible collisions
bool PlayerAgent::HandleCollision(const CollisionEventData& data)
{	
	//COLLISION HANDLING:
	/*Here the steps:
		- Check which blob collided (main blob, secondary controlled, other created and scattered)
		- Check and update the case where mainblob - minorblob collide (they should merge after a time)
		- Process collision logic with a collectable agent. If main blob is controlled, collectable is
		  "taken" by player
		- Forward collision processing to the affected blob controllers
	*/
	//When other blobs are active, they can merge together again
	//Local variables
	bool collsubblob1, collsubblob2, collblob;
	collsubblob1 = collsubblob2 =  collblob = false;
	BlobControllerPointer oneblob;
	BlobControllerPointer otherblob;

	//*****************CHECK WHICH BLOB WAS COLLIDED FOR THE PLAYER AGENT***************
	//IF - Main blob controlled
	if(mBlobController)
	{
		collblob = mBlobController->IsCollisionInBlobBody(data.GetCollisionData());
		if(collblob)
			oneblob = mBlobController;
	}//IF

	//IF - Second blob controlled
	if(mSecondControl && mSecondBlobController)
	{
		collsubblob1 = mSecondBlobController->IsCollisionInBlobBody(data.GetCollisionData());
		if(!oneblob && collsubblob1)
			oneblob = mSecondBlobController;
		else if(oneblob && collsubblob1)
			otherblob = mSecondBlobController;
	}//IF

	//IF - Not collided with second blob controlled
	if(!oneblob || !otherblob)
	{
		BlobControllerList::iterator blobitr;
		//LOOP - Search for collided blob
		for(blobitr = mBlobsList.begin(); blobitr != mBlobsList.end(); ++blobitr)
		{
			//IF - Collided with this blob
			if((*blobitr)->IsCollisionInBlobBody(data.GetCollisionData()))
			{
				//IF - No blob collided
				if(!oneblob)
				{
					oneblob = (*blobitr);
					collsubblob1 = true;
				}
				else//ELSE - There was collision with another blob already
				{
					otherblob = (*blobitr);
					collsubblob2 = true;
					break; //Break because collision is between 2 elements...
				}//IF
			}//IF
		}//LOOP END
	}
	//****************************************************************************************
	//*************************PROCESS SPECIAL CASES OF BLOB-BLOB COLLISION*******************
	//IF - New collision - Special processing
	if(data.GetEventType() == Event_NewCollision)
	{
		//Check if collision is between the two blobs (Main and thrown)-> They should merge
		if(collblob && (collsubblob1 || collsubblob2))
		{
			//Search for previous collision and update it!
			BlobCollisionList::iterator blclitr;
			bool found(false);
			//LOOP - Search for collided blob info and update it
			for(blclitr = mBlobCollisionsList.begin(); blclitr != mBlobCollisionsList.end(); ++blclitr)
			{
				//IF - Found
				if((*blclitr).theotherblob ==  otherblob)
				{
					found = true;
					break;
				}//IF
			}//LOOP END

			//IF - Found
			if(found)
			{
				++(*blclitr).numcollisions; //Just increment pointer
			}
			else
			{
				mBlobCollisionsList.push_back(BlobCollisionInfo(otherblob)); //Create with one collision
			}
		}//IF
	}//ELSE - Collision deleted - Special processing
	else if(data.GetEventType() == Event_DeletedCollision)
	{
		//Check if collision is between the two blobs (Main and thrown)-> They should merge
		if(collblob && (collsubblob1 || collsubblob2))
		{
			//Search for previous collision and update it!
			BlobCollisionList::iterator blclitr;
			bool found(false);
			//LOOP - Search for collided blob info and update it
			for(blclitr = mBlobCollisionsList.begin(); blclitr != mBlobCollisionsList.end(); ++blclitr)
			{
				//IF - Found
				if((*blclitr).theotherblob ==  otherblob)
				{
					found = true;
					break;
				}//IF
			}//LOOP END

			//IF - Found
			if(found)
			{	//Update values
				--(*blclitr).numcollisions;
				//Delete collision if necessary
				if((*blclitr).numcollisions <= 0)
				{
					mBlobCollisionsList.erase(blclitr);
				}
			}	
		}//IF
	}//IF
	//****************************************************************************************
	//************************PROCESS SPECIAL CASE OF BLOB-COLLECTABLE COLLISION**************
	//IF - New collision with main blob
	if(data.GetEventType() == Event_NewCollision 
	   &&
	   !mSecondControl && !mSecondBlobController
	   &&
	   collblob)
	{

		IAgent* agent1 = data.GetCollisionData().agent1;
		IAgent* agent2 = data.GetCollisionData().agent2;

		//IF - Agent of body 1 is a collectable
		if(agent1)
		{
			if(agent1->GetType() == COLLECTABLE)
			{
				//Send event of "collected"
				DropCollidedInfo info(agent1);
				SingletonGameEventMgr::Instance()->QueueEvent(
														EventDataPointer(new DropCollidedEvent(Event_DropCollision,info))
														);
				//Applies health to blob
				mBlobController->ApplyHealth();
			}
		}//IF
			
		//IF - Agent of body 2 is a collectable
		if(agent2)
		{
			if(agent2 ->GetType() == COLLECTABLE)
			{
				//Send event of "collected"
				DropCollidedInfo info(agent2);
				SingletonGameEventMgr::Instance()->QueueEvent(
														EventDataPointer(new DropCollidedEvent(Event_DropCollision,info))
														);
				//Applies health to blob
				mBlobController->ApplyHealth();
			}
		}
		
	}//IF
	//****************************************************************************************
	//************************FORWARD COLLISION TO RESPECTIVE BLOB CONTROLLERS****************
	//IF - Collission between two blobs
	if(oneblob && otherblob)
	{
		//Dont apply collision damage
		oneblob->IgnoreCollisionDamage();
		otherblob->IgnoreCollisionDamage();
	}//IF
	bool debugshit(false);
	b2Body* collisionbody = data.GetActiveBody();
	//Forward collision after processing
	//IF - First blob 
	if(oneblob)
	{
		//IF - Active body in collision event is from this blob
		if(oneblob->IsBodyInBlob(collisionbody))
			oneblob->HandleCollision(data);
		else
			debugshit = true;
	}
	if(otherblob)
	{
		//IF - Active body in collision event is from this blob
		if(otherblob->IsBodyInBlob(collisionbody))
			otherblob->HandleCollision(data);
		else
			debugshit = true;
	}

	//IF - Collission between two blobs
	if(oneblob && otherblob)
	{
		//Dont apply collision damage
		oneblob->DontIgnoreCollisionDamage();
		otherblob->DontIgnoreCollisionDamage();
	}//IF
	//******************************************************************************************
	return true;
}

//Process possible events
bool PlayerAgent::HandleEvent(const EventData& data)
{
	bool eventprocessed =  false;
	//Handle correct events:
	//In-Game Events
	if(!mActive)
		return true;

	// IF - Request to move blob
	if(data.GetEventType() == Event_BlobMove && mControlDelay < 0.0f)
	{	
		const BlobMovementEvent& newtargetdata = static_cast<const BlobMovementEvent&>(data);
		Vector2 movdirection = newtargetdata.GetMoveCommand();
		if(mSecondControl && mSecondBlobController)
		{
			mSecondBlobController->MoveBlob(movdirection);			
		}
		else
		{
			mBlobController->MoveBlob(movdirection);
		}
		eventprocessed = true;
	}//ELSE IF - Command to sacrifice blob
	else if(data.GetEventType() == Event_SacrificeBlobCommand && mControlDelay < 0.0f)
	{
		if(mSecondControl && mSecondBlobController)
		{
			//Destroy the blob
			mSecondBlobController->SetIntegrity(0);
		}

	}//ELSE IF - Request to shoot new blob
	else if(data.GetEventType() == Event_ShootBlobCommand && mControlDelay < 0.0f)
	{	
		//IF - Not already controlling second blob
		if(!mSecondControl)
		{
			//IF - Enough integrity left and stopped
			if(!mBlobController->IsIntegrityVeryLow())
			{

				const ShootBlobEvent& shootcommanddata = static_cast<const ShootBlobEvent&>(data); 
				//Get parameters 
				assert(shootcommanddata.GetForcePercent() > 0.0f && shootcommanddata.GetForcePercent() <1.1f);
				
				//Creation of a new blob, smaller than the other one in some percentage
				//The creation is an involved process, and a builder is used. The initial parameters are taken
				//from the controller, and scaled back to some degree.

				BlobParameters blobparams = mBlobController->GetInitialParameters();

								//Get shooting direction
				Vector2 target = shootcommanddata.GetTargetPosition() -  mParams.position;
				target.Normalise();

				blobparams.initialx = static_cast<float>(mParams.position.x) + (static_cast<float>(target.x) * blobparams.radius);
				blobparams.initialy = static_cast<float>(mParams.position.y) + (static_cast<float>(target.y) * blobparams.radius);

				//Scale down parameters (to make smaller blob)				
				//Multiply and update operator - To make conversions easier (hardcoded as I dont 
				blobparams.radius *= 0.4f;
				blobparams.bodies = static_cast<int>(0.7f * blobparams.bodies);
				blobparams.jointsdamping *= 1.5f;
				blobparams.jointsfrequency *= 1.6f;
				blobparams.skinjointsdamping *= 2.0f;
				blobparams.skinjointsfrequency *= 2.0f;
				blobparams.massesradius *= 0.5f;
				mSubBlobMassesRadius = blobparams.massesradius; //Store this parameter for drawing
				mSubBlobMassesRadius *= 1.2f; //Visual improvement ;)
				blobparams.massesdensity *= 0.5f;
				blobparams.innermassradius *= 0.4f;
				blobparams.innermassdensity *= 0.5f;
				blobparams.initialintegrity = 20.0f;
	       
				BlobBuilder thebuilder(mPhysicsMgr,this);
				thebuilder.LoadBlob(blobparams);  //Create using modified params
				
				mSecondBlobController = thebuilder.GetBlobController();
				mSecondBlobController->SetMaxControlForce(mParams.maxcontrolforce * 0.15f);
				mSecondBlobController->SetMaxSpeed(mParams.maxspeed * 1.0f);
				mSecondBlobController->SetDamageForce(mParams.damageforce * mParams.damageratio);
				mSecondBlobController->SetIntegrity(20.0f);				
				
				//Update controller and start it
				mSecondBlobController->StartControlling(false);
				//Make an impulse to blob unconditional to collisions and other internal data
				mSecondBlobController->ApplyImpulseToBlob(target * shootcommanddata.GetForcePercent() * mParams.throwingforce);
				
				//Apply damage to bigger blob
				mBlobController->ApplyDamage();

				//Send event to report change of health
				//Report others of change of health
				BlobHealthInfo info(mSecondBlobController->GetIntegrity(),true);
				SingletonGameEventMgr::Instance()->QueueEvent(
														  EventDataPointer(new BlobHealthEvent(Event_BlobHealth,info))
															);
				//Update internal tracking
				mSecondControl = true;
			}//IF
		}//IF

		eventprocessed = true; //Processed
	}//ELSE IF - CHANGE BLOB COMMAND
	else if(data.GetEventType() == Event_ChangeBlobCommand)
	{
		//IF - There is a second blob controlled
		if(mSecondControl && mSecondBlobController)
		{
			//Store blob as present in level
			mSecondBlobController->SetAsMainBlob(false);
			mBlobsList.push_front(mSecondBlobController);

			//Change back to main blob
			mSecondControl = false;
			mSecondBlobController.reset();
			mControlDelay = 1000.0f;  //Delay to response next control event
			//Send event to report change of health
			//Report others of change of health
			BlobHealthInfo info(mBlobController->GetIntegrity(),true);
			SingletonGameEventMgr::Instance()->QueueEvent(
														  EventDataPointer(new BlobHealthEvent(Event_BlobHealth,info))
															);
		}
	}//ELSE IF - RENDER IN CORRESPONDING LAYER
	else if(data.GetEventType() == Event_RenderInLayer)	
	{
		 
		//Convert event data
		const RenderInLayerXEvent& layerdata = static_cast<const RenderInLayerXEvent&>(data);
		
		//IF - The layer is the one where to draw TODO: GET LAYER FROM ENTITY!!!!
		if(layerdata.GetLayer() == 10) 
		{
			//Allways draw main blob
			if(mBlobController)
			{
				if(!mSecondControl)
					_drawBlob(mBlobController,mMassesRadius,SingletonIndieLib::Instance()->FromHSLToRGB(mParams.drawcolor));
				else
					_drawBlob(mBlobController,mMassesRadius,SingletonIndieLib::Instance()->FromHSLToRGB(mParams.drawcolor));
			}

			if(mSecondControl && mSecondBlobController)
			{
				//Draw it
				_drawBlob(mSecondBlobController,mSubBlobMassesRadius,SingletonIndieLib::Instance()->FromHSLToRGB(mParams.drawcolor));
			}

			//Draw additional blobs if used
			BlobControllerList::iterator blobitr;
			//LOOP - Draw all blobs
			for(blobitr = mBlobsList.begin(); blobitr != mBlobsList.end(); ++blobitr)
			{
				_drawBlob((*blobitr),mSubBlobMassesRadius,SingletonIndieLib::Instance()->FromHSLToRGB(mParams.originaldrawcolor));
			}//LOOP
		}//IF
	}//IF
	return eventprocessed;
}

//Handle out of limits
void PlayerAgent::HandleOutOfLimits(const OutOfLimitsEventData& data) 
{ 
	//Get the body which went out of limits
	b2Body* body = data.GetEventData().body;

	//IF - Main blob was out of limits
	if(mBlobController->IsBodyInBlob(body))
	{
		mBlobController->SetIntegrity(0);
	}//ELSE - Body is in second controlled blob
	else if(mSecondControl && mSecondBlobController)
	{
		if(mSecondBlobController->IsBodyInBlob(body))
		{
			mSecondBlobController->SetIntegrity(0);
		}
	}
	else
	{
		//Search in other blobs
		//Delete from list of blobs
		BlobControllerList::iterator blobsitr;
		//LOOP - Find blob and delete it
		for(blobsitr = mBlobsList.begin(); blobsitr != mBlobsList.end();++blobsitr)
		{
			if((*blobsitr)->IsBodyInBlob(body))
			{
				//Destroy the blob by setting to 0 integrity (destroy in update call...)
				(*blobsitr)->SetIntegrity(0);
			}
		}//LOOP END
	}
}					

//Create from params
void PlayerAgent::Create( const GameAgentPar *params)	
{
	//Agent params should be of type
	//Copy parameters from creation 
	assert(params->type == PLAYER);
	
	//Cast down params hierarchy to this type of params
	const PlayerAgentPar *pagentparams = static_cast<const PlayerAgentPar*>(params);
	//Copy all parameters
	mParams = *pagentparams;
	
	//Update internal variables
	mOriginalScale.x = mParams.sprite.gfxentity->GetScaleX();
	mOriginalScale.y = mParams.sprite.gfxentity->GetScaleY();

	//Init graphics
	mAnimController = AnimationControllerPointer(new AnimationController(mParams.sprite.gfxentity,0,true));
	//Finally, update internal tracking
	mActive = true;
}

//Destroy agent
void PlayerAgent::Destroy()
{
	//Destroy blob and dont show graphics
	mParams.sprite.gfxentity->SetShow(false);
	mBlobController->Destroy();
	mBlobController.reset();
	//Destroy all other blobs
	if(mSecondBlobController)
	{
		mSecondBlobController->Destroy();
		mSecondBlobController.reset();
	}
	BlobControllerList::iterator blobitr;
	//LOOP - Search all blobs
	for(blobitr = mBlobsList.begin();blobitr != mBlobsList.end();++blobitr)
	{
		//Destroy Blob
		(*blobitr)->Destroy();
	}//LOOP
	mBlobsList.clear();

	//Send "Game Over" msg (player died!)
	SingletonGameEventMgr::Instance()->TriggerEvent(
			EventDataPointer (new EventData(Event_GameOver))
			);

	//Finally, update internal tracking
	mActive = false;
}

//Called to set the pointer to blob controller
void PlayerAgent::SetBlobController(BlobControllerPointer pointer) 
{ 
	//Set the controller parameters from the player
	mBlobController = pointer;  
	mBlobController->SetMaxControlForce(mParams.maxcontrolforce);
	mBlobController->SetMaxSpeed(mParams.maxspeed);
	mBlobController->SetDestructionSpeed(mParams.destructionspeed);
	mBlobController->SetDamageForce(mParams.damageforce);
	//Memorize how many bodies has this blob
	mMassesRadius = mBlobController->GetInitialParameters().massesradius;
	mMassesRadius *= 1.2f; //Visual improvement ;)
	//Update controller and start it
	mBlobController->StartControlling(true);

	//Send necessary events
	//Send event to report change of health
	//Report others of change of health
	BlobHealthInfo info(mBlobController->GetIntegrity(),mBlobController->IsIntegrityVeryLow());
	SingletonGameEventMgr::Instance()->QueueEvent(
											  EventDataPointer(new BlobHealthEvent(Event_BlobHealth,info))
												);

	//Before starting, update general camera position in player position
	mParams.position = Vector2 (mBlobController->GetInitialParameters().initialx,
						mBlobController->GetInitialParameters().initialy);
	SingletonIndieLib::Instance()->GetCamera("General")->SetPosition(mParams.position );
}

//Draw a blob
void PlayerAgent::_drawBlob(BlobControllerPointer thepointer,float radiusoffset, const ColorRGBA& drawcolor)
{

	IND_Point drawpoints[3];
	IND_Point firstbodydrawpoint;

	//Get initial and outer bodies coords
	BlobController::BodiesVector::const_reverse_iterator bodiesit = thepointer->GetOuterBodiesListStart();
	BlobController::BodiesVector::const_reverse_iterator bodieslistend = thepointer->GetOuterBodiesListEnd();

	//------Get center position-----
	b2Vec2 centerpos = thepointer->GetCenterBody()->GetPosition(); 
	//Write start vertex to draw
	drawpoints[0].x = static_cast<int>(mGlobalScale * centerpos.x);
	drawpoints[0].y = static_cast<int>(mResY - (mGlobalScale * centerpos.y));
	b2Vec2 bodypos = (*bodiesit)->GetPosition();
	b2Vec2 raddir = bodypos - centerpos; //Get vector from center to point
	float angle =  atan2(raddir.y,raddir.x); //Rotation angle	
	b2Vec2 addradius(radiusoffset,0.0f);  //We have to add radius of body, to render last point!
	b2Mat22 rotmatrix(angle); //Create a rotation matrix
	addradius = b2Mul(rotmatrix,addradius); //Rotate the adding radius
	bodypos+= addradius; //Now we can add, as they are in same direction
	firstbodydrawpoint.x = static_cast<int>(mGlobalScale * bodypos.x);
	firstbodydrawpoint.y = static_cast<int>(mResY - (mGlobalScale * bodypos.y));
	//Store coords of first body
	drawpoints[1].x  = firstbodydrawpoint.x;
	drawpoints[1].y  = firstbodydrawpoint.y;

	//------Get all bodies and write triangles-----
	//LOOP - Write all coords
	int trianglevertex = 2; //Start at last vertex of first triangle
	while(bodiesit != bodieslistend)
	{
		if(trianglevertex != 0)
		{
			//Write body vertex in triangle
			b2Vec2 bodypos = (*bodiesit)->GetPosition();
			b2Vec2 raddir = bodypos - centerpos; //Get vector from center to point
			float angle =  atan2(raddir.y,raddir.x); //Rotation angle	
			b2Vec2 addradius(radiusoffset,0.0f);  //We have to add radius of body, to render last point!
			b2Mat22 rotmatrix(angle); //Create a rotation matrix
			addradius = b2Mul(rotmatrix,addradius); //Rotate the adding radius
			bodypos+= addradius; //Now we can add, as they are in same direction
			drawpoints[trianglevertex].x = static_cast<int>(mGlobalScale * bodypos.x);
			drawpoints[trianglevertex].y = static_cast<int>(mResY - (mGlobalScale * bodypos.y));
			++bodiesit;  //Increment iterator
		}

		//IF - Triangle completed
		if(trianglevertex == 2)
		{	
			SingletonIndieLib::Instance()
			->Render->BlitColoredTriangle(drawpoints[0].x,
										  drawpoints[0].y,
										  drawpoints[1].x,
										  drawpoints[1].y,
										  drawpoints[2].x,
										  drawpoints[2].y,
										  255,255,255,
										  static_cast<byte>(drawcolor.red),
										  static_cast<byte>(drawcolor.green),
										  static_cast<byte>(drawcolor.blue),
										  static_cast<byte>(drawcolor.red),
										  static_cast<byte>(drawcolor.green),
										  static_cast<byte>(drawcolor.blue),
										  static_cast<byte>(drawcolor.alpha));							
			trianglevertex = 0;
			bodiesit--;
		}
		else
		{
			++trianglevertex;
		}
	}
	//LOOP END
	//Update last triangle (to close circle with start)
	//Last point drawn passes to be second vertex
	drawpoints[1].x = drawpoints[2].x;
	drawpoints[1].y = drawpoints[2].y;
	//First body position is last vertex of last body

	SingletonIndieLib::Instance()
	->Render->BlitColoredTriangle(drawpoints[0].x,
										  drawpoints[0].y,
										  drawpoints[1].x,
										  drawpoints[1].y,
										  firstbodydrawpoint.x,
										  firstbodydrawpoint.y,
										  255,255,255,
										  static_cast<byte>(drawcolor.red),
										  static_cast<byte>(drawcolor.green),
										  static_cast<byte>(drawcolor.blue),
										  static_cast<byte>(drawcolor.red),
										  static_cast<byte>(drawcolor.green),
										  static_cast<byte>(drawcolor.blue),
										  static_cast<byte>(drawcolor.alpha));	
}

void PlayerAgent::_updateBlobGFX(float dt)
{
	//Update position of sprite (scaled to pixels)
	mParams.sprite.gfxentity->SetPosition(static_cast<float>(mParams.position.x * mGlobalScale),
									   static_cast<float>(mResY - (mParams.position.y * mGlobalScale)),
									   0);

	//Update scaling of sprite and track damage
	bool isnewdamage(false);
	float integritypercent(0.0f);
	
	//IF - Second blob controlled
	if(mSecondControl && mSecondBlobController)
	{
		mParams.sprite.gfxentity->SetScale(static_cast<float>(mOriginalScale.x * 0.6f),
									static_cast<float>(mOriginalScale.y * 0.6f));
		isnewdamage = mSecondBlobController->IsBlobDamaged();
		integritypercent = mSecondBlobController->GetIntegrityPercent();
	}
	else//ELSE  - First blob controlled
	{
		mParams.sprite.gfxentity->SetScale(static_cast<float>(mOriginalScale.x),
									static_cast<float>(mOriginalScale.y));
		isnewdamage = mBlobController->IsBlobDamaged();
		integritypercent = mBlobController->GetIntegrityPercent();
	}//IF

	//Update drawing color to match target
	//Get drawing color
	ColorHSLA originalcolor (mParams.originaldrawcolor);
	ColorHSLA newcolor (originalcolor);
	//Apply a change in saturation
	float satpercent (integritypercent);
	newcolor.saturation = originalcolor.saturation * satpercent;
	float lightpercent ( 0.2f - (integritypercent)*0.2f);
	newcolor.lightness = originalcolor.lightness + (originalcolor.lightness * lightpercent);
		
	//Check limits
	if(newcolor.saturation > 1.0)
		newcolor.saturation = 1.0;
	if(newcolor.saturation < 0.0)
		newcolor.saturation = 0.0;
	if(newcolor.lightness > 1.0)
		newcolor.lightness = 1.0;
	if(newcolor.lightness < 0.0)
		newcolor.lightness = 0.0;

	int huedifference = newcolor.hue - mParams.drawcolor.hue;
	float satdifference =  newcolor.saturation - mParams.drawcolor.saturation;
	float lightdifference = newcolor.lightness - mParams.drawcolor.lightness;
	
	if(abs(huedifference) > 1)
		mParams.drawcolor.hue += static_cast<int>(huedifference*0.001f*dt);
	else
		mParams.drawcolor.hue = newcolor.hue;

	if(abs(satdifference) > 0.01f) 
		mParams.drawcolor.saturation += satdifference*0.001f*dt;
	else
		mParams.drawcolor.saturation = newcolor.saturation;

	if(abs(lightdifference) > 0.01f)
		mParams.drawcolor.lightness += lightdifference*0.001f*dt;
	else
		mParams.drawcolor.lightness = newcolor.lightness;

	//Check limits
	if(mParams.drawcolor.saturation > 1.0f)
		mParams.drawcolor.saturation = 1.0f;
	if(mParams.drawcolor.saturation < 0.0f)
		mParams.drawcolor.saturation = 0.0f;
	if(mParams.drawcolor.lightness > 1.0f)
		mParams.drawcolor.lightness = 1.0f;
	if(mParams.drawcolor.lightness < 0.0f)
		mParams.drawcolor.lightness = 0.0f;
	if(mParams.drawcolor.hue > 360) 
		mParams.drawcolor.hue = 360;
	if(mParams.drawcolor.hue < 0)
		mParams.drawcolor.hue = 0;

	if(isnewdamage)
	{
		//Set animation of damage
		mAnimController->SetNextAnimation(false,5,false);
	}
	else if(
		mAnimController->IsCurrentAnimationFinished()
		)
	{
		if(mLinearVel.x < -2)
		{
			//Just "Look left"
			if(!mAnimController->IsAnimActive(1))
			{
				mAnimController->SetNextAnimation(true,1,true);
			}
			else if(!mAnimController->IsAnimationStoppedAtEnd())
			{
				mAnimController->SetNextAnimation(true,1,true);
			}
		}
		else if(mLinearVel.x > 2)
		{
			//Just "Look right"
			if(!mAnimController->IsAnimActive(3))
			{
				mAnimController->SetNextAnimation(true,3,true);
			}
			else if(!mAnimController->IsAnimationStoppedAtEnd())
			{
				mAnimController->SetNextAnimation(true,3,true);
			}
		}
		else
		{
			mAnimController->ForceDefaultAnim(true);
		}
	}
}

//Init
void PlayerAgent::_init()
{
	//Init internal variables
	mGlobalScale = SingletonIndieLib::Instance()->GetGeneralScale();
	mResY = static_cast<float>(SingletonIndieLib::Instance()->Window->GetHeight());
}


//Release internal resources
void PlayerAgent::_release()
{
	//Deregister from indielib entity drawing is done in animation controller
}