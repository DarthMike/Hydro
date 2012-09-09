/*
	Filename: PhysicsEvents.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Definition of events related to physics
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _PHYSICSEVENTS
#define _PHYSICSEVENTS

//Library dependencies	

//Class dependencies
#include "PhysicsManager.h"
#include "GameEventsDef.h"

//The collision event data - this event data is the same for 3 types of event
class CollisionEventData : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	CollisionEventData(const GameEventType type, const ContactInfo& data, float timestamp = 0.0f):
	  EventData(type,timestamp),
	  mContactInfo(data),
	  mActiveBody(NULL)
	  {
		  //Assert event created with correct enabled types
		  assert (
			         mType == Event_NewCollision || 
					 mType == Event_PersistantCollision || 
					 mType == Event_DeletedCollision || 
					 mType == Event_CollisionResult
				  );
	  }
	~CollisionEventData()
	{}
	//----- GET/SET FUNCTIONS -----
	b2Body* GetActiveBody() const { return mActiveBody; }
	void SetActiveBody(b2Body* activebody) const
	{ 
		assert (activebody == mContactInfo.collidedbody1 || activebody == mContactInfo.collidedbody2); 
		mActiveBody = activebody; //Modification through "mutable"
	}
	const ContactInfo& GetCollisionData() const { return mContactInfo; } //Get const contact info
	//----- OTHER FUNCTIONS -----

protected:
	//----- INTERNAL VARIABLES -----
	ContactInfo mContactInfo; //Contact info
	mutable b2Body* mActiveBody;  //Active body - It can be modified to change event forwarding by clients, even with const-data
	//----- INTERNAL FUNCTIONS -----
};

//The "out of limits" event data
typedef struct OutOfLimitsData
{
	OutOfLimitsData(b2Body* affected,IAgent* agent):
	body(affected),
	agent(agent)
	{}
	b2Body* body;
	IAgent* agent;
}OutOfLimitsData;

//Event class
class OutOfLimitsEventData : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	OutOfLimitsEventData(const GameEventType type, const OutOfLimitsData& data, float timestamp = 0.0f):
	  EventData(type,timestamp),
	  mData(data)
	  {
		  //Assert event created with correct enabled types
		  assert (
			         mType == Event_OutOfLimits
				  );
	  }
	~OutOfLimitsEventData()
	{}
	//----- GET/SET FUNCTIONS -----
	const OutOfLimitsData& GetEventData() const { return mData; } //Get const contact info
	//----- OTHER FUNCTIONS -----

protected:
	//----- INTERNAL VARIABLES -----
	OutOfLimitsData mData; //Event data
	//----- INTERNAL FUNCTIONS -----
};
#endif