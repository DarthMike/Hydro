/*
	Filename: PhysicsManager.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Physics Managing encapsulation / provides single entry point through this class to physics
				 Class for contact managing (collision) is also created
	Comments: In construction, creates a parametrized physics simulation world.
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _PHYSICSMANAGER
#define _PHYSICSMANAGER

//VISUAL C GIVES COMPILER WARNING AS WE HAVE A CONST MEMBER, AND HE CANT CREATE AN
//ASSIGNMENT OPERATOR---- WE WILL NOT COPY THIS CLASS! DISABLE WARNING
#pragma warning(push)  //Store warnings state
#pragma warning(disable : 4512)  //Disable this warning

//Library dependencies	
#include <map>
#include "Box2D\Box2D.h"
//Class dependencies
#include "LogManager.h"
#include "GenericException.h"
#include "GameEventManager.h"

//---------------Custom physics contact listener (collision detection)-----------------------
class PhysicsManager;
class GameContactListener : public b2ContactListener
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameContactListener(PhysicsManager* managerptr)
		:mPhysicsMgr(managerptr),
		mIgnoreCollisions(false)
	{}
	~GameContactListener()
	{}

	//----- OTHER FUNCTIONS -----
	//IMPLEMENTATION OF VIRTUAL INTERFACE
	void Add(const b2ContactPoint* point);		//Custom handle - a new contact point was added
	void Persist(const b2ContactPoint* point);	//Custom handle - a contact point persisted for more than one timestep
	void Remove(const b2ContactPoint* point);	//Custom handle - a contact point was deleted
    void Result(const b2ContactResult* point);	//Custom handle - Collision result parameters
	//Overriding of collision handling
	void IgnoreCollisions() { mIgnoreCollisions = true; }
	void ListenCollisions() { mIgnoreCollisions = false; }
private:
	PhysicsManager* mPhysicsMgr;
	bool mIgnoreCollisions;
	static int debugcollisionsnum;
};

//Definitions
const int MAXFOUNDSHAPES = 15;

//Custom contact info to analyze and use in-game
enum ContactState {ADDED, PERSISTED, REMOVED, RESULT};
class IAgent;
//Contact info structure: 2 parts can be parametrized: Contact (static) info, and Result (dynamic result) info
typedef struct ContactInfo
{
	//Construction with contactpoint
	ContactInfo(const b2ContactPoint& point):
	  position(point.position),
	  normal(point.normal),
	  separation(point.separation),
	  restitution(point.restitution),
	  friction(point.friction),
	  relvelocity(point.velocity),
	  agent1(static_cast<IAgent*>(point.shape1->GetBody()->GetUserData())),
	  agent2(static_cast<IAgent*>(point.shape2->GetBody()->GetUserData())),
	  collidedshape1(point.shape1),
	  collidedshape2(point.shape2),
	  collidedbody1(point.shape1->GetBody()),
	  collidedbody2(point.shape2->GetBody()),
	  normalimpulse(0.0f),  //Result data initialized to 0
	  tangentimpulse(0.0f),  //Result data initialized to 0
	  contactid(point.id.key),
	  state(ADDED)
	  {}

    //Construction with collision result
	ContactInfo(const b2ContactResult& result):
	  position(result.position),
	  normal(result.normal),
	  separation(0.0f), //Contact data initialized to 0
	  restitution(0.0f), //Contact data initialized to 0
	  friction(0.0f), //Contact data initialized to 0
	  relvelocity(0.0f,0.0f), //Contact data initialized to 0
	  agent1(static_cast<IAgent*>(result.shape1->GetBody()->GetUserData())),
	  agent2(static_cast<IAgent*>(result.shape2->GetBody()->GetUserData())),
	  collidedshape1(result.shape1),
	  collidedshape2(result.shape2),
	  collidedbody1(result.shape1->GetBody()),
	  collidedbody2(result.shape2->GetBody()),
	  normalimpulse(result.normalImpulse),
	  tangentimpulse(result.tangentImpulse),
	  contactid(result.id.key),
	  state(RESULT)
	  {}

	//Both sides info
	IAgent* agent1;
	IAgent* agent2;
	b2Body* collidedbody1;
	b2Body* collidedbody2;
	b2Shape* collidedshape1;
	b2Shape* collidedshape2;
	b2Vec2 position;
	b2Vec2 normal;

	//Contact-side info
	float32 separation;
	float32 restitution;
	float32 friction;
	b2Vec2 relvelocity;

	//Result-side info
	float32 normalimpulse;	///< the normal impulse applied to body2
	float32 tangentimpulse;	///< the tangent impulse applied to body2

	//Id and general info
	uint32 contactid;
	ContactState state;
}ContactInfo;

//Custom key to store contacts info in an ordered map
class ContactInfoKey
{
public:
	ContactInfoKey(b2Shape* shape1, uint32 contactid, b2Shape* shape2,const ContactState& state):
	mShape1(shape1),
	mContactId(contactid),
	mShape2(shape2),
	mState(state)
	{}
//It only has operator definitions
public:
	//Operator less - than to compare
	bool operator <(const ContactInfoKey& tocompare) const
	{
		//Ordering taking into account shapes and keys, because Box2D only assures that
		if(mContactId != tocompare.mContactId)
			return mContactId < tocompare.mContactId;
		else if(mShape1 != tocompare.mShape1)
			return mShape1 < tocompare.mShape1; //Ugly pointer less-than compare...
		else if(mShape2 != tocompare.mShape2)
			return mShape2 < tocompare.mShape2; //Ugly pointer less-than compare...
		else if(mState != tocompare.mState)
			return mState < tocompare.mState;   //Ugly enum-int less-than compare...
		else
			return false;
	}
private:
	//Internal variables
	b2Shape* mShape1;
	b2Shape* mShape2;
	uint32 mContactId;
	ContactState mState;
};

//------------------------------Custom boundary listener--------------------------------------
class PhysicsManager;
class GameBoundaryListener : public b2BoundaryListener 
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameBoundaryListener(PhysicsManager* managerptr)
		:mPhysicsMgr(managerptr)
	{}
	~GameBoundaryListener()
	{}
public:
	//----- OTHER FUNCTIONS -----
	//IMPLEMENTATION OF VIRTUAL INTERFACE
	virtual void Violation(b2Body* body);

private:
	//----- INTERNAL VARIABLES -----
	PhysicsManager* mPhysicsMgr;
};


//------------------------------Physics manager-----------------------------------------------

class PhysicsManager
{
	//Friend class is Contact Listener and boundary listener (callbacks from Box2d)
	friend class GameContactListener;
	friend class GameBoundaryListener;
	//Definitions
public:
	static const std::string MouseJointName;
protected:
	//Containers for created entities
	typedef std::map<std::string,b2Body*> PhysBodiesMap;
	typedef PhysBodiesMap::iterator PhysBodiesMapIterator;
	typedef std::vector<b2Body*> PhysBodiesVec;
	typedef PhysBodiesVec::iterator PhysBodiesVecIterator;
	typedef std::map<std::string,b2Joint*> JointsMap;
	typedef JointsMap::iterator JointsMapIterator;
	//container for out of bounds elements
	typedef std::pair<b2Body*,IAgent*> OutofBoundsData;
	typedef std::vector<OutofBoundsData> OutofBoundsVec;
	typedef OutofBoundsVec::iterator	OutofBoundsVecIterator;
	//Container for contact callbacks buffering
	typedef std::map<ContactInfoKey,ContactInfo> ContactPointsMap;
	typedef ContactPointsMap::iterator ContactPointsMapIterator;
	typedef std::pair<ContactInfoKey,ContactInfo> ContactPointMapPair;
	//Container for contact result callbacks buffering
	typedef std::map<ContactInfoKey,ContactInfo> ContactResultsMap;
	typedef ContactResultsMap::iterator ContactResultsMapIterator;
	typedef std::pair<ContactInfoKey,ContactInfo> ContactResultsMapPair;
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	PhysicsManager(const b2Vec2 &gravity,float32 timestep, int32 iterations, const b2Vec2 &upperbound,const b2Vec2 &lowerbound,b2DebugDraw *debugdrawimpl = NULL)
		:mIterations(iterations),
		 mTimeStep(timestep),
		 mTimestepms(timestep*1000),
		 MAXACCUMTIME(80.0f),
		 mTimeAccumulator(0),
		 mpContactListener(NULL),
		 mPhysicsStepped(false),
		 mTimeStepped(0.0f)
	{
		//Construct a world using parameters supplied
		//AABB for the world
		b2AABB worldAABB;
		worldAABB.lowerBound.Set(lowerbound.x, lowerbound.y);
		worldAABB.upperBound.Set(upperbound.x, upperbound.y);
	
		//Construct the world object, and allow bodies to sleep
		mpTheWorld = new b2World(worldAABB,gravity,true);
		
		//Config contact listener
		mpContactListener = new GameContactListener(this);
		mpTheWorld->SetContactListener(mpContactListener);
		//Config boundary listener
		mpBoundaryListener = new GameBoundaryListener(this);
		mpTheWorld->SetBoundaryListener(mpBoundaryListener);

		//Config debug draw(if it is supplied)
		if(debugdrawimpl)
		{
			mpTheWorld->SetDebugDraw(debugdrawimpl);
		}

		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager","World constructed successfully",LOGNORMAL);
	}
	~PhysicsManager()
	{
		//Clear dynamic memory - Note that by deleting only the world
		//all other elements contained are deleted... nice!
		if(mpTheWorld)
		{
			delete mpTheWorld;
			mpTheWorld = NULL;
		}

		//Delete contact listener
		if(mpContactListener)
		{
			delete mpContactListener;
			mpContactListener = NULL;
		}
		
		//Delete boundary listener
		if(mpBoundaryListener)
		{
			delete mpBoundaryListener;
			mpBoundaryListener = NULL;
		}
		SingletonLogMgr::Instance()->AddNewLine("PhysicsManager","Physics world destructed successfully",LOGNORMAL);
	}
	//----- GET/SET FUNCTIONS -----
	b2Body* GetBody(const std::string &name);  //Get a body pointer by name
	std::string GetBodyName(b2Body* body);  //Get a body name by pointer
	b2Joint* GetJoint(const std::string &name);
	bool IsPhysicsStepped() { return mPhysicsStepped; }    //Returns control variable to know if in last update physics was stepped
	float GetSteppedTime() { return mTimeStepped; }			//Returns the time which simulation advanced
	//----- OTHER FUNCTIONS -----
	//Methods to create / destroy physics elements
	b2Body* CreateBody(const b2BodyDef* definition,const std::string& name);
	void DestroyBody(const std::string& name);
	void DestroyBody(b2Body* bodypointer);
	void CreateCircleShape(b2CircleDef* definition, const std::string &bodyname);
	void CreatePolygonShape(b2PolygonDef* definition, const std::string &bodyname);
	void DestroyShape(b2Shape* theshape, const std::string& parentbodyname);
	bool CreateDistanceJoint(b2DistanceJointDef* definition, const std::string &jointname, const std::string& body1, const std::string& body2, const b2Vec2& worldpoint1, const b2Vec2& worldpoint2);
	bool CreateRevoluteJoint(b2RevoluteJointDef* definition, const std::string &jointname, const std::string& body1, const std::string& body2, const b2Vec2& worldpoint);
	bool CreatePrismaticJoint(b2PrismaticJointDef* definition, const std::string &jointname, const std::string& body1, const std::string& body2, const b2Vec2& worldpoint, const b2Vec2& axis);
	bool CreatePulleyJoint(b2PulleyJointDef* definition, const std::string &jointname, const std::string& body1, const std::string& body2);
	bool CreateGearJoint(b2GearJointDef* definition, const std::string &jointname, const std::string& joint1, const std::string& joint2);
	void DestroyJoint(const std::string& jointname);
	void CreateMouseJoint(b2MouseJointDef* jointdef);
	void DestroyMouseJoint();
	//Queries
	b2Body* QueryforBodies(const b2Vec2 &thepoint, bool includestatic = false);	//Query for bodies in a point (through AABB)
	std::vector <b2Body*> QueryforBodies(const b2AABB &boundingbox, bool includestatic = false);  //Query for bodies inside AABB
	bool QueryforoneBody(const b2AABB &boundingbox, const std::string &bodytofind); //Query for a specific body inside an AABB

	//Advanced (not simple) bodies properties modification
	void ChangeFrictionofBody(b2Body* thebody, float newfriction);   //Changes de friction of all shapes within the body
	
	//Updating methods
	void Update (float dt);
	void DebugRender();
	
protected:
	//----- INTERNAL VARIABLES -----
	const int32 mIterations; //General
	const float32 mTimeStep;
	const float32 mTimestepms;
	const float32 MAXACCUMTIME;

	float32 mTimeStepped;		//Time stepped when iterating physics
	
	float32 mTimeAccumulator; //Step control variables
	bool mPhysicsStepped;	  //Very important variable to check when actuating or applying forces to bodies,
							  //as refresh rate of game is not the same as physics engine...!
	
	b2World* mpTheWorld;  //The world

	GameContactListener* mpContactListener; //Contact-collision handling
	ContactPointsMap mContactPoints;
	ContactResultsMap mContactResults;

	GameBoundaryListener* mpBoundaryListener; //Boundary listener implementation

	PhysBodiesMap mBodiesMap;  //Containers of created elements
	JointsMap mJointsMap;
	OutofBoundsVec mOutofBoundsBodies;	//Container to know which bodies should be destroyed
	//----- INTERNAL FUNCTIONS -----
	//Events generation - Collisions
	void _sendNewContactEvent(const ContactInfo& data);
	void _sendDeleteContactEvent(const ContactInfo& data);
	void _sendPersitedContactEvent(const ContactInfo& data);
	void _sendContactResultEvent(const ContactInfo& data);
	//Events generation - Out of limits body
	void _sendOutOfLimitsEvent(const OutofBoundsData& outofbounds);
	
};

#pragma warning(pop)  //Restore warnings state
#endif