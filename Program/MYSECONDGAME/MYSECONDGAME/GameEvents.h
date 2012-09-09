/*
	Filename: GameEvents.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Definition of specific game-related events
	Comments: Shared pointer (boost smart pointer implementation) used to define events data
	Attribution: "Game coding complete" was the inspiration , great book! http://www.mcshaffry.com/GameCode/
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _GAMEEVENTS
#define _GAMEEVENTS

//Library dependencies
#include <vector>

//Classes dependencies
#include "GameEventsDef.h"
#include "GameLogicDefs.h"
#include "Vector2.h"

typedef struct DebugStringInfo //New debug string info
{
	DebugStringInfo(const std::string& thestring):
	message(thestring)
	{}

	//New target position
	std::string message;
}DebugStringInfo;

typedef struct NewTargetInfo //New movement target
{
	NewTargetInfo(const Vector2& newtarget):
	target(newtarget)
	{}

	//New target position
	Vector2 target;
}NewPositionInfo;

typedef struct SolidCollisionInfo  //New collision info
{
	SolidCollisionInfo(MaterialType material)
	:
	material(material)
	{}

	MaterialType material;
}SolidCollisionInfo;

typedef struct BlobMoveInfo //Blob movement command
{
	BlobMoveInfo(const Vector2& newdirection):
	direction(newdirection)
	{}

	//Movement by keyboard - Direction scaled 0-1
	Vector2 direction;
}BlobMoveInfo;

typedef struct BlobPositionInfo //New blob position update
{
	BlobPositionInfo(const Vector2& newposition, float maxspeed, const Vector2& speed):
	position(newposition),
	speed(speed),
	maxspeed(maxspeed)
	{}

	//Position coordinates of currently controlled blob
	Vector2 position;
	//Speed of blob
	Vector2 speed;
	//Max speed
	float maxspeed;
}BlobPositionInfo;

typedef struct BlobHealthInfo //New blob health update
{
	BlobHealthInfo(float newhealth, bool critical):
	health(newhealth),
	iscritical(critical)
	{}

	//New health
	float health;
	bool iscritical; //Is it critical?
}BlobHealthInfo;

//Forward declarations
class b2Body;

typedef struct BlobDeathInfo  //Blob death info
{
	BlobDeathInfo(const Vector2& pos, bool ismainblob, float radius):
	position(pos),
	mainblob(ismainblob),
	radius(radius)
	{}

	//Position of Death
	Vector2 position;
	//Is main blob
	bool mainblob;
	//Blob radius when died
	float radius;
	//"Affected" bodies by death of blob
	std::vector<b2Body*> affectedbodies;
}BlobDeathInfo;

typedef struct ShootBlobCommand  //Shoot a new blob command
{
	ShootBlobCommand(const Vector2& targetpos,float forcepercent):
	shoottargetpos(targetpos),
	forcepercentage(forcepercent)
	{}	
	Vector2 shoottargetpos;  //The position to throw to
	float forcepercentage;	 //The percentage of force
}ShootBlobCommand;

typedef struct RenderInLayerInfo //Perform additional per-layer rendering
{
	RenderInLayerInfo(int thelayer):
	layer(thelayer)
	{}
	int layer;  //Layer to render in
}RenderInLayerInfo;

class IAgent;
typedef struct DropCollidedInfo //A drop was collected by blob
{
	DropCollidedInfo(IAgent* collectableptr):
	collected(collectableptr)
	{}
	IAgent* collected; //Collected agent pointer
}DropCollidedInfo;

typedef struct CollectedValuesInfo //New values of level collected and/or total to collect
{
	CollectedValuesInfo(int collected, int tocollect):
	totalcollected(collected),
	tocollect(tocollect)
	{}
	int totalcollected;   //Values
	int tocollect;
}CollectedValuesInfo;

//DATA WHICH WILL BE SENT  - NEW MOVEMENT TARGET //TODO: REMOVE
class DebugMessageEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	DebugMessageEvent(const GameEventType type, const DebugStringInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mDebugInfo(info)
	{}
	virtual ~DebugMessageEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	const std::string& GetString() const { return mDebugInfo.message; }
	
protected:
	//----- INTERNAL VARIABLES -----
	DebugStringInfo mDebugInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT  - NEW MOVEMENT TARGET //TODO: REMOVE
class NewTargetEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	NewTargetEvent(const GameEventType type, const NewTargetInfo& targetinfo, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mTargetInfo(targetinfo)
	{}
	virtual ~NewTargetEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	const Vector2& GetTargetPosition() const { return mTargetInfo.target; }
	
protected:
	//----- INTERNAL VARIABLES -----
	NewTargetInfo mTargetInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - COMMAND TO MOVE BLOB
class SolidCollisionEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	SolidCollisionEvent(const GameEventType type, const SolidCollisionInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~SolidCollisionEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	const SolidCollisionInfo& GetInfo() const  {return mInfo;}
	
protected:
	//----- INTERNAL VARIABLES -----
	SolidCollisionInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - COMMAND TO MOVE BLOB
class BlobMovementEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	BlobMovementEvent(const GameEventType type, const BlobMoveInfo& moveinfo, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mMoveInfo(moveinfo)
	{}
	virtual ~BlobMovementEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	const Vector2& GetMoveCommand() const { return mMoveInfo.direction; }
	
protected:
	//----- INTERNAL VARIABLES -----
	BlobMoveInfo mMoveInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - HEALTH UPDATE OF MAIN CONTROLLED BLOB
class BlobHealthEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	BlobHealthEvent(const GameEventType type, const BlobHealthInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mBlobHealthInfo(info)
	{}
	virtual ~BlobHealthEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	float GetHealth() const {return mBlobHealthInfo.health; }
	bool IsCritical() const { return mBlobHealthInfo.iscritical; }
protected:
	//----- INTERNAL VARIABLES -----
	BlobHealthInfo mBlobHealthInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - BLOB DIED
class BlobDeathEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	BlobDeathEvent(const GameEventType type, const BlobDeathInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mBlobDeathInfo(info)
	{}
	virtual ~BlobDeathEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	Vector2 GetPosition() const { return mBlobDeathInfo.position; }
	float GetRadius() const { return mBlobDeathInfo.radius; }
	bool IsMainBlob() const { return mBlobDeathInfo.mainblob; }

	//Quite ugly but I wont modify / Just to work in a nice fast way
	std::vector<b2Body*>::const_iterator GetFirstAffectedBody() const { return mBlobDeathInfo.affectedbodies.begin(); }
	std::vector<b2Body*>::const_iterator GetLastAffectedBody() const { return mBlobDeathInfo.affectedbodies.end(); }
protected:
	//----- INTERNAL VARIABLES -----
	BlobDeathInfo mBlobDeathInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - POSITION UPDATE OF MAIN CONTROLLED BLOB
class ShootBlobEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	ShootBlobEvent(const GameEventType type, const ShootBlobCommand& commandinfo, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mBlobCommandInfo(commandinfo)
	{}
	virtual ~ShootBlobEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	const Vector2& GetTargetPosition() const {return mBlobCommandInfo.shoottargetpos; }
	float GetForcePercent() const { return mBlobCommandInfo.forcepercentage; }
protected:
	//----- INTERNAL VARIABLES -----
	ShootBlobCommand mBlobCommandInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - POSITION UPDATE OF MAIN CONTROLLED BLOB
class BlobPositionEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	BlobPositionEvent(const GameEventType type, const BlobPositionInfo& posinfo, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mBlobPositionInfo(posinfo)
	{}
	virtual ~BlobPositionEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	const Vector2& GetNewPosition() const { return mBlobPositionInfo.position; }
	const Vector2& GetNewSpeed() const { return mBlobPositionInfo.speed; }
	float GetMaxSpeed() const { return mBlobPositionInfo.maxspeed; }

protected:
	//----- INTERNAL VARIABLES -----
	BlobPositionInfo mBlobPositionInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - RENDER DATA IN A CONCRETE LAYER
class RenderInLayerXEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	RenderInLayerXEvent(const GameEventType type, const RenderInLayerInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mLayerInfo(info)
	{}
	virtual ~RenderInLayerXEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	int GetLayer() const { return mLayerInfo.layer; }

protected:
	//----- INTERNAL VARIABLES -----
	RenderInLayerInfo mLayerInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - DROP COLLIDED
class DropCollidedEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	DropCollidedEvent(const GameEventType type, const DropCollidedInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~DropCollidedEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	IAgent* GetCollectedAgent()const { return mInfo.collected; }

protected:
	//----- INTERNAL VARIABLES -----
	DropCollidedInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - NEW "COLLECTED" VALUES
class NewCollectedValuesEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	NewCollectedValuesEvent(const GameEventType type, const CollectedValuesInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~NewCollectedValuesEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	int GetCollectedSoFar()const { return mInfo.totalcollected; }
	int GetTotalToCollect()const { return mInfo.tocollect; }

protected:
	//----- INTERNAL VARIABLES -----
	CollectedValuesInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

#endif