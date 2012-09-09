/*
	Filename: PhysicsManager.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Physics Managing encapsulation / provides single entry point through this class to physics
				 Class for contact managing (collision) is also created
	Comments: In construction, creates a parametrized physics simulation world.
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "PhysicsManager.h"
#include "PhysicsEvents.h"
#include "GameEvents.h"

//Definition of static members
const std::string PhysicsManager::MouseJointName = "TheMouseJoint";
int GameContactListener::debugcollisionsnum = 0;
//******************************CONTACT LISTENER IMPLEMENTATION***********************************
void GameContactListener::Add(const b2ContactPoint* point)
{
	//IF - Listen to collisions
	if(!mIgnoreCollisions)
	{
		 //Custom handle - a new contact point was added
		//When handling, as recommended by Erin Catto, it is better to buffer all contact points, and then
		//deal with them in an update function. All points are stored/created inside a container in physics manager.
		//Here we add new points, or modify existing ones

		//Create new collision point info
		ContactInfoKey id(point->shape1,point->id.key,point->shape2,ADDED);

		//Create new point
		ContactInfo pointinfo(*point);	 
		pointinfo.state = ADDED;		//Created with ADDED STATE
		PhysicsManager::ContactPointMapPair insertionpair(id,pointinfo);
		mPhysicsMgr->mContactPoints.insert(insertionpair);

	}//IF
}
    
void GameContactListener::Persist(const b2ContactPoint* point)
{
	//IF - Listen to collisions
	if(!mIgnoreCollisions)
	{
		 //Custom handle - a contact point is persisting
		//When handling, as recommended by Erin Catto, it is better to buffer all contact points, and then
		//deal with them in an update function. All points are stored/created inside a container in physics manager.
		//Here we add new points, or modify existing ones
		
		//Create new collision point info
		ContactInfoKey id(point->shape1,point->id.key,point->shape2,PERSISTED);
		
		//Update/create new point
		ContactInfo pointinfo(*point);		
		pointinfo.state = PERSISTED;		//Created with PERSISTED STATE
		PhysicsManager::ContactPointMapPair insertionpair(id,pointinfo);
		mPhysicsMgr->mContactPoints.insert(insertionpair);
			
	}//IF
}
    
void GameContactListener::Remove(const b2ContactPoint* point)
{
	//IF - Listen to collisions
	if(!mIgnoreCollisions)
	{
		 //Custom handle - a contact point was removed
		//When handling, as recommended by Erin Catto, it is better to buffer all contact points, and then
		//deal with them in an update function. All points are stored/created inside a container in physics manager.
		//Here we add new points, or modify existing ones

		//Create new collision point info
		ContactInfoKey id(point->shape1,point->id.key,point->shape2,REMOVED);
		
		//Create new point
		ContactInfo pointinfo(*point);		//Created with ADDED STATE
		pointinfo.state = REMOVED;			//Change to deleted state
		PhysicsManager::ContactPointMapPair insertionpair(id,pointinfo);
		mPhysicsMgr->mContactPoints.insert(insertionpair);
	}//IF

}
    
void GameContactListener::Result(const b2ContactResult* point)
{
	//Custom handle - a new result for a collision
	//When handling, as recommended by Erin Catto, it is better to buffer all contact points, and then
	//deal with them in an update function. All COLLISION RESULTS are stored in different container, 
	//as it has different meaning in BOX2D

	//Build contact info key data to map in container
	ContactInfoKey id(point->shape1,point->id.key,point->shape2,RESULT);
	
	//Update/create new point
	ContactInfo pointinfo(*point);	 
	pointinfo.state = RESULT;		//Created with RESULT STATE
	PhysicsManager::ContactResultsMapPair insertionpair(id,pointinfo);
	mPhysicsMgr->mContactResults.insert(insertionpair);
}

//******************************BOUNDARY LISTENER IMPLMEMENTATION*********************************
//The only callback!
void GameBoundaryListener::Violation(b2Body* body)
{
	IAgent* relatedagent = static_cast<IAgent*>(body->GetUserData());
	//Check agent was not registered before
	PhysicsManager::OutofBoundsVecIterator itr;
	//LOOP - Check out of bounds for 
	for(itr = mPhysicsMgr->mOutofBoundsBodies.begin(); itr != mPhysicsMgr->mOutofBoundsBodies.end(); ++itr)
	{
		if((*itr).second == relatedagent)
			return;
	}//LOOP END

	//Just store in Physics manager this body pointer
	mPhysicsMgr->mOutofBoundsBodies.push_back(
												PhysicsManager::OutofBoundsData(body,relatedagent)
											  );
}

//******************************PHYSICS MANAGER IMPLEMENTATION************************************
//Update simulation
void PhysicsManager::Update(float dt)
{
	/*NOT GOOD FOCUS: BOX2D INTERNALLY CACHES TO-DELETE AND ACTIVE BODIES. JUST BE SURE THEY ARE
	QUERIED FOR DESTROY AFTER A BOX2D STEP (PHYSICS STEP MUST ALLWAYS BE FIRST IN UPDATING LOGIC LOOP!!!!)
	//---------------------Delete requested bodies-------------------------------
	//As contacts and collisions cause events, and events cause response from the system, 
	//before updating next step of physics, requested to-delete bodies need to be destroyed
	//When a request to destroy a body is made, they are stored in a special container
	PhysBodiesVecIterator boditr;
	//mpContactListener->IgnoreCollisions(); //Ignore to collisions, as "CONTACT DELETED" is sent if body is destroyed when collided!!!
	//LOOP - Delete all pending bodies
	for(boditr = mBodiesToDestroyVec.begin(); boditr != mBodiesToDestroyVec.end(); boditr++)
	{
		//Delete body in BOX2D
		mpTheWorld->DestroyBody((*boditr));
	}//LOOP END

	//Clear container
	mBodiesToDestroyVec.clear();
	//mpContactListener->ListenCollisions(); //Liste to collisions again*/


	//---------------------Make advance in the world simulation--------------------
	//Timestep sync with FPS using fixed timestep for physics engine
	//taken from:http://gafferongames.com/game-physics/fix-your-timestep/
	//This will keep physics in time with frame rate and keep speeds to real
	
	//Update time elapsed in accumulator
	mTimeAccumulator += dt;

	//Clamp max time accumulated
	if(mTimeAccumulator > MAXACCUMTIME)
		mTimeAccumulator = MAXACCUMTIME;

	mPhysicsStepped = false;
	mTimeStepped = 0.0f;
	//LOOP - Step physics any time as needed using fixed timestep
	while(mTimeAccumulator >= mTimestepms)
	{
		//Recalculate new values for time-stepping
		mTimeStepped += mTimestepms;
		mTimeAccumulator -= mTimestepms;
		
		//Perform a step of simulation of physics world
		mpTheWorld->Step(mTimeStep,  //Timestep
			             mIterations, //Velocity solver iterations
						 mIterations,   //Position solver iterations
						 mTimeAccumulator<mTimestepms //Is it necessary to reset forces after step? (No more steps in this update)
						 );

		mPhysicsStepped = true;
	}//LOOP END

	mpTheWorld->Validate();
	//---------------------Send collision events-------------------------------
	//As creator of Box2D suggests, contact points in step of physics simulation are 
	//buffered for processing now. Points are stored in a custom structure "ContactInfo"
	//and its state reflects the result of the processing in successive callbacks: new, persistent, deleted
	//Points in state deleted have to be deleted after processing.
	ContactPointsMapIterator it;

	//LOOP - Pass all contact points and generate corresponding event
	for (it = mContactPoints.begin(); it != mContactPoints.end(); it++)
	{
		//Check in which state is the point
		switch((*it).second.state)
		{
		case ADDED: //The point was added - new collision event
			{
				//Call method to send event
				_sendNewContactEvent((*it).second);		
			}
			break;
		case PERSISTED: //The point was persisting more than one step - persisted collision event
			{
				//Call method to send event
				_sendPersitedContactEvent((*it).second);
			}
			break;
		case REMOVED: //The point was removed - deleted collision event
			{
				//Call method to send event
				_sendDeleteContactEvent((*it).second);
			}
			break;
		}
	}//LOOP END
	
	//After processing contact points, delete all data for this step
	mContactPoints.clear();

	//Process contact results
	ContactResultsMapIterator itr;
	//LOOP - Process contact results callbacks
	for (itr = mContactResults.begin(); itr != mContactResults.end(); itr++)
	{
		//Assert good state
		assert((*itr).second.state == RESULT);
		
		//Call method to send event
		_sendContactResultEvent((*itr).second);		
	}//LOOP END

	//After processing contact results, delete all data for this step
	mContactResults.clear();

	//---------------------Send "out of limits" events----------------------------
	OutofBoundsVecIterator boditr;
	//LOOP - Delete all pending bodies
	for(boditr = mOutofBoundsBodies.begin(); boditr != mOutofBoundsBodies.end(); boditr++)
	{
		//Call method to send event
		_sendOutOfLimitsEvent((*boditr));
	}//LOOP END

	//Clear container
	mOutofBoundsBodies.clear();
}

//Debug render call
void PhysicsManager::DebugRender()
{
	mpTheWorld->DrawDebugData();
}

//Get a body and return its pointer
b2Body* PhysicsManager::GetBody(const std::string &name)
{
	//find it
	PhysBodiesMapIterator itr = mBodiesMap.find(name);

	if(itr != mBodiesMap.end())
	{
		return (*itr).second;
	}
	else
	{
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::GetBody","Error: intent to acces nonexistent body: " + name ,LOGEXCEPTION);
		return NULL;
	}

}

//Get a body name by pointer
std::string PhysicsManager::GetBodyName(b2Body* body)
{
	PhysBodiesMapIterator itr;
	//LOOP - Find requested body
	for(itr = mBodiesMap.begin();itr != mBodiesMap.end();++itr)
	{
		if((*itr).second == body)
			return ((*itr).first);
	}//LOOP END

	//Not found
	return (std::string(""));
}

//Get a joint and return its pointer
b2Joint* PhysicsManager::GetJoint(const std::string &name)
{
	//Be sure body doesnt exist already
	JointsMapIterator itr = mJointsMap.find(name);

	if(itr != mJointsMap.end())
	{
		return (*itr).second;
	}
	else
	{
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::GetJoint","Error: intent to acces nonexistent joint: " + name ,LOGEXCEPTION);
		return NULL;
	}	
}
//Create a body given some parameters preconstructed
b2Body *PhysicsManager::CreateBody(const b2BodyDef* definition,const std::string& name)
{
	//Be sure body doesnt exist already
	PhysBodiesMapIterator itr = mBodiesMap.find(name);

	if(itr == mBodiesMap.end())
	{
		//Create body requested
		b2Body* newbody = mpTheWorld->CreateBody(definition);
		//Add it to maps
		mBodiesMap[name] = newbody;
		return newbody;
	}
	else
	{
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::CreateBody","Error: intent to create the same body twice: " + name ,LOGEXCEPTION);
		return NULL;
	}
	
}

//Destroy a body by name
void PhysicsManager::DestroyBody(const std::string& name)
{
	//First find requested body
	PhysBodiesMapIterator itr = mBodiesMap.find(name);
	
	//IF - Body found
	if(itr != mBodiesMap.end())
	{
		//mBodiesToDestroyVec.push_back((*itr).second);//Push pointer to container to-delete
		mpTheWorld->DestroyBody((*itr).second); //Destroy directly a body, box2d stores active and to-delete bodies internally
		mBodiesMap.erase(itr); //Delete reference in active bodies
	}
	else //ELSE - Body not found
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::DestroyBody","Error: intent to destroy non-existent body: " + name,LOGEXCEPTION);

	//TODO: Send body deleted event!
}

//Destroy body by pointer
void PhysicsManager::DestroyBody(b2Body* bodypointer)
{
	//First be sure requested body pointer exists
	PhysBodiesMapIterator itr;
	bool found = false;
	//LOOP - Look for body pointer requested
	for(itr = mBodiesMap.begin();itr != mBodiesMap.end(); itr++)
	{
		//Delete it and clear reference
		if((*itr).second == bodypointer)
		{
			found = true;
			//mBodiesToDestroyVec.push_back((*itr).second);//Push pointer to container to-delete
			mpTheWorld->DestroyBody((*itr).second);  //Destroy directly a body, box2d stores active and to-delete bodies internally
			mBodiesMap.erase(itr); //Delete reference in active bodies
			break;
		}
	}//LOOP END

	//IF - Body found
	if(!found)
	{
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::DestroyBody","Error: intent to destroy non-existent body through POINTER!",LOGEXCEPTION);
	}//IF
	//TODO: Send body deleted event!
}

//Create a circular shape
void PhysicsManager::CreateCircleShape(b2CircleDef* definition, const std::string &bodyname)
{
	PhysBodiesMapIterator itr = mBodiesMap.find(bodyname);

	if(itr != mBodiesMap.end())
	{
		//Add shape to body
		(*itr).second->CreateShape(definition);
	}
	else
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::CreateCircleShape","Error: intent to create shape to non-existent body: " + bodyname,LOGEXCEPTION);
}

//Create a polygonal shape
void PhysicsManager::CreatePolygonShape(b2PolygonDef* definition, const std::string &bodyname)
{
	if(definition->vertexCount > b2_maxPolygonVertices)
		throw GenericException("Encountered a polygon definition with too many vertexs",GenericException::INVALIDPARAMS);
	
	//TODO: BE SURE IT IS A CONVEX POLYGON OR TRANSFORM IT!

	PhysBodiesMapIterator itr = mBodiesMap.find(bodyname);

	if(itr != mBodiesMap.end())
	{
		//Add shape to body
		(*itr).second->CreateShape(definition);
	}
	else
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::CreatePolygonShape","Error: intent to create shape to non-existent body: " + bodyname,LOGEXCEPTION);

}

//Destroy a given shape from its parent body
void PhysicsManager::DestroyShape(b2Shape* theshape, const std::string& parentbodyname)
{
	//First find requested body
	PhysBodiesMapIterator itr = mBodiesMap.find(parentbodyname);
	
	if(itr != mBodiesMap.end())
	{
		//Delete it
		(*itr).second->DestroyShape(theshape);
	}
	else
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::DestroyShape","Error: intent to destroy shape in non-existent body: " + parentbodyname,LOGEXCEPTION);
}

//Create a mouse joint; only one can exist, attached to a body
void PhysicsManager::CreateMouseJoint(b2MouseJointDef* jointdef)
{	
	//Add first anchor body to mousejoint
	jointdef->body1 = mpTheWorld->GetGroundBody();
	mJointsMap[MouseJointName] = mpTheWorld->CreateJoint(jointdef);
}

//Destroy the only mouse joint
void PhysicsManager::DestroyMouseJoint()
{
	//Check mouse joint really exists
	JointsMapIterator itr = mJointsMap.find(MouseJointName);
	if(itr != mJointsMap.end())
	{
		mpTheWorld->DestroyJoint((*itr).second);
		mJointsMap.erase(itr);
	}
	else
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::DestroyMouseJoint","Error: intent to destroy mouse joint without creating it first!",LOGEXCEPTION);
}

//Create a distance joint
bool PhysicsManager::CreateDistanceJoint(b2DistanceJointDef* definition, const std::string &jointname, const std::string& body1, const std::string& body2, const b2Vec2& worldpoint1, const b2Vec2& worldpoint2)
{
	//Name of joint has to be coherent
	JointsMapIterator jointitr = mJointsMap.find(jointname);
	//IF - Name of joint is not correct
	if(jointitr != mJointsMap.end())
	{
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::CreateDistanceJoint","Error: intent to create the same joint twice: " + jointname ,LOGEXCEPTION);
		return false;
	}
	else //ELSE - Name of joint is correct
	{
		//Find bodies to attach joint
		PhysBodiesMapIterator itrbody1, itrbody2;
		itrbody1 = mBodiesMap.find(body1);
		itrbody2 = mBodiesMap.find(body2);
		//IF - Bodies are correct
		if(itrbody1 != mBodiesMap.end() && itrbody2 != mBodiesMap.end())
		{
			//Modify params and create body
			definition->Initialize((*itrbody1).second,(*itrbody2).second,worldpoint1,worldpoint2);
			b2Joint* newjoint = mpTheWorld->CreateJoint(definition);
			if(newjoint)
				mJointsMap[jointname] = newjoint;	
			else 
				return false;
		}//ELSE - bodies not correct
		else
			return false;
	}//END IF

	return true;
}

//Create a revolute joint
bool PhysicsManager::CreateRevoluteJoint(b2RevoluteJointDef* definition, const std::string &jointname, const std::string& body1, const std::string& body2, const b2Vec2& worldpoint)
{
	//Name of joint has to be coherent
	JointsMapIterator jointitr = mJointsMap.find(jointname);
	//IF - Name of joint is not correct
	if(jointitr != mJointsMap.end())
	{
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::CreateRevoluteJoint","Error: intent to create the same joint twice: " + jointname ,LOGEXCEPTION);
		return false;
	}
	else //ELSE - Name of joint is correct
	{
		//Find bodies to attach joint
		PhysBodiesMapIterator itrbody1, itrbody2;
		itrbody1 = mBodiesMap.find(body1);
		itrbody2 = mBodiesMap.find(body2);
		
		//IF - Bodies are correct
		if(itrbody1 != mBodiesMap.end() && itrbody2 != mBodiesMap.end()
		   ||
		   (body1 == "" && itrbody2 != mBodiesMap.end())
		   ||
		   (body2 == "" && itrbody1 != mBodiesMap.end()))
		{
			//Modify params and create body
			b2Body* body1ptr(NULL);
			b2Body* body2ptr(NULL);
			if(body1 !=  "")
				body1ptr = (*itrbody1).second;
			else
				body1ptr = mpTheWorld->GetGroundBody();

			if(body2 !=  "")
				body2ptr = (*itrbody2).second;
			else
				body2ptr = mpTheWorld->GetGroundBody();

			definition->Initialize(body1ptr,body2ptr,worldpoint);
			b2Joint* newjoint = mpTheWorld->CreateJoint(definition);
			if(newjoint)
				mJointsMap[jointname] = newjoint;
			else 
				return false;
		}//ELSE - bodies not correct
		else
			return false;
	}//END IF

	return true;
}

//Create a prismatic joint
bool PhysicsManager::CreatePrismaticJoint(b2PrismaticJointDef* definition, const std::string &jointname, const std::string& body1, const std::string& body2, const b2Vec2& worldpoint, const b2Vec2& axis)
{
	//Name of joint has to be coherent
	JointsMapIterator jointitr = mJointsMap.find(jointname);
	//IF - Name of joint is not correct
	if(jointitr != mJointsMap.end())
	{
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::CreatePrismaticJoint","Error: intent to create the same joint twice: " + jointname ,LOGEXCEPTION);
		return false;
	}
	else //ELSE - Name of joint is correct
	{
		//Find bodies to attach joint
		PhysBodiesMapIterator itrbody1, itrbody2;
		itrbody1 = mBodiesMap.find(body1);
		itrbody2 = mBodiesMap.find(body2);
		//IF - Bodies are correct
		if(itrbody1 != mBodiesMap.end() && itrbody2 != mBodiesMap.end())
		{
			//Modify params and create body
			definition->Initialize((*itrbody1).second,(*itrbody2).second,worldpoint,axis);
			b2Joint* newjoint = mpTheWorld->CreateJoint(definition);

			if(newjoint)
				mJointsMap[jointname] = newjoint;
			else 
				return false;
		}//ELSE - bodies not correct
		else
			return false;
	}//END IF

	return true;
}

//Create a Pulley joint
bool PhysicsManager::CreatePulleyJoint(b2PulleyJointDef* definition, const std::string &jointname, const std::string& body1, const std::string& body2)
{
	//Name of joint has to be coherent
	JointsMapIterator jointitr = mJointsMap.find(jointname);
	//IF - Name of joint is not correct
	if(jointitr != mJointsMap.end())
	{
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::CreatePulleyJoint","Error: intent to create the same joint twice: " + jointname ,LOGEXCEPTION);
		return false;
	}
	else //ELSE - Name of joint is correct
	{
		//Find bodies to attach joint
		PhysBodiesMapIterator itrbody1, itrbody2;
		itrbody1 = mBodiesMap.find(body1);
		itrbody2 = mBodiesMap.find(body2);
		//IF - Bodies are correct
		if(itrbody1 != mBodiesMap.end() && itrbody2 != mBodiesMap.end())
		{
			//Modify params and create body
			definition->body1 = (*itrbody1).second;
			definition->body2 = (*itrbody2).second;
			b2Joint* newjoint = mpTheWorld->CreateJoint(definition);
			if(newjoint)
				mJointsMap[jointname] = newjoint;
			else
				return false;
		}//ELSE - bodies not correct
		else
			return false;
	}//END IF

	return true;
}

//Destroy any joint by name
void PhysicsManager::DestroyJoint(const std::string& jointname)
{
	//Check mouse joint really exists
	JointsMapIterator itr = mJointsMap.find(jointname);
	if(itr != mJointsMap.end())
	{
		mpTheWorld->DestroyJoint((*itr).second);
		mJointsMap.erase(itr);
	}
	else
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::DestroyMouseJoint","Error: intent to destroy non-existent joint!: " + jointname,LOGEXCEPTION);
}

//Query for bodies in a point (through AABB)
b2Body* PhysicsManager::QueryforBodies(const b2Vec2 &thepoint, bool includestatic)
{
	// Query the world for overlapping shapes with a point (very small aabb)
	
	// Make a small box to query
	b2AABB aabb;
	b2Vec2 d(0.001f, 0.001f);
	aabb.lowerBound = thepoint - d;
	aabb.upperBound = thepoint + d;

	//Create a list of maximum number of shapes to find
	b2Shape* shapes[MAXFOUNDSHAPES];
	int32 count = mpTheWorld->Query(aabb, shapes, MAXFOUNDSHAPES);
	b2Body* body = NULL;
	//LOOP - Search every result
	for (int i = 0; i < count; ++i)
	{
		b2Body* shapeBody = shapes[i]->GetBody();
		if ((!shapeBody->IsStatic()&& shapeBody->GetMass() > 0.0f && !includestatic)
			 ||
			 includestatic)
		{
			bool inside = shapes[i]->TestPoint(shapeBody->GetXForm(), thepoint);
			if (inside)
			{
				body = shapes[i]->GetBody();
				return body;
			}
		}
	}//LOOP END

	return NULL;
}

//Query for bodies inside AABB
std::vector<b2Body*> PhysicsManager::QueryforBodies(const b2AABB &boundingbox, bool includestatic)
{
	//Vector to return
	std::vector<b2Body*> bodiesvector;
	//Create a list of maximum number of shapes to find
	b2Shape* shapes[MAXFOUNDSHAPES];
	int32 count = mpTheWorld->Query(boundingbox, shapes, MAXFOUNDSHAPES);

	//LOOP - Append results to return vector
	for(int i = 0; i < count; ++i)
	{
		b2Body* shapebody(shapes[i]->GetBody());
		if ((shapebody->IsDynamic()&& shapebody->GetMass() > 0.0f && !includestatic)
			 ||
			 includestatic)
		{
			bodiesvector.push_back(shapebody);
		}
	}
	//LOOP END


	//Return a COPY of the created vector (Ugly but works fine with vector of pointers (small size) and limited vector size)
	return bodiesvector;
}

//Query for a specific body inside an AABB
bool PhysicsManager::QueryforoneBody(const b2AABB &boundingbox, const std::string &bodytofind)
{
	//Be sure body exists
	PhysBodiesMapIterator itr = mBodiesMap.find(bodytofind);
	b2Body* thebody = NULL;
	if(itr != mBodiesMap.end())
	{
		thebody = (*itr).second;
	}
	else
	{
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager::QueryforoneBody","Attempt to find non-existent body!",LOGEXCEPTION);
		return false;
	}

	//Create a list of maximum number of shapes to find
	b2Shape* shapes[MAXFOUNDSHAPES];
	int32 count = mpTheWorld->Query(boundingbox, shapes, MAXFOUNDSHAPES);
	//LOOP - Search every result
	for (int i = 0; i < count; ++i)
	{
		b2Body* shapeBody = shapes[i]->GetBody();
		//IF - Is this body the asked one to find?
		if(shapeBody == thebody)
		{
			return true;
		}
	}//LOOP END

	return false;
}

//Changes de friction of all shapes within the body
void PhysicsManager::ChangeFrictionofBody(b2Body* thebody, float newfriction)
{
	//Parameters correctness
	assert(thebody);
	assert(newfriction >= 0.0f);

	//Get body's shape list
	b2Shape* nextshape = thebody->GetShapeList();

	//LOOP - Change friction of all body's shapes
	while(nextshape)
	{
		//Modify friction
		nextshape->SetFriction(newfriction);
		//Finally, recompute cached contact data (if existed). If not updated, friction in this
		//existing contact will remain the same
		mpTheWorld->Refilter(nextshape);
		nextshape = nextshape->GetNext();
	}//LOOP
}

//Events sending - New Contact
void PhysicsManager::_sendNewContactEvent(const ContactInfo& data)
{
	//Create event data and send it - NOTE: Use of Shared pointers, not need to delete
	SingletonGameEventMgr::Instance()->TriggerEvent(
						EventDataPointer (new CollisionEventData(Event_NewCollision,data))
						);
}

//Events sending - Deleted Contact
void PhysicsManager::_sendDeleteContactEvent(const ContactInfo& data)
{
	//Create event data and send it - NOTE: Use of Shared pointers, not need to delete
	SingletonGameEventMgr::Instance()->TriggerEvent(
						EventDataPointer (new CollisionEventData(Event_DeletedCollision,data))
						);
}

//Events sending - Persisted Contact
void PhysicsManager::_sendPersitedContactEvent(const ContactInfo& data)
{
	//Create event data and send it - NOTE: Use of Shared pointers, not need to delete
	SingletonGameEventMgr::Instance()->TriggerEvent(
						EventDataPointer (new CollisionEventData(Event_PersistantCollision,data))
						);
}

//Events sending - Contact Result
void PhysicsManager::_sendContactResultEvent(const ContactInfo& data)
{
	//Create event data and send it - NOTE: Use of Shared pointers, not need to delete
	SingletonGameEventMgr::Instance()->TriggerEvent(
						EventDataPointer (new CollisionEventData(Event_CollisionResult,data))
						);
}

//Events generation - Out of limits body
void PhysicsManager::_sendOutOfLimitsEvent(const OutofBoundsData& outofbounds)
{
	//Construct event data
	OutOfLimitsData data(outofbounds.first,outofbounds.second);
	//Send event
	SingletonGameEventMgr::Instance()->TriggerEvent(
						EventDataPointer( new OutOfLimitsEventData(Event_OutOfLimits,data))
						);
}