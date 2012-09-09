/*
	Filename: BlobController.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Blob physical controller to abstract general actions in the character
	             A data container to create the blob is also defined
	Comments: Related strictly to Box2D!
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
#ifndef _BLOBCONTROLLER
#define _BLOBCONTROLLER

//Library dependencies
#include "boost\shared_ptr.hpp"  //Smart pointer facilites - not using std because it cant go inside an 
								 //STL container as we do in event manager...
#include <vector>
#include <map>
#include <assert.h>
//Class dependencies
#include "Vector2.h"
#include "Shared_Resources.h"

//Forward declarations
class b2Body;
class b2Shape;
class b2DistanceJoint;
class CollisionEventData;
struct ContactInfo;
class IAgent;

//Definitions
class BlobController;
//A smart pointer to hold the created blob controller
typedef boost::shared_ptr<BlobController> BlobControllerPointer;

//A class to store created parameters
class BlobParameters
{
public:
	//Constructor
	BlobParameters():
	initialx(25.0f),
	initialy(35.f),
	radius(2.5f),
	bodies(50),
	damagefiltertime(400.0f),
	jointsdamping(0.01f),
	jointsfrequency(1.8f),
	skinjointsdamping(0.3f),
	skinjointsfrequency(40.0f),
	massesradius(0.3f),
	massesfriction(0.1f),
	massesrestitution(0.0f),
	massesdensity(0.05f),
	innermassradius(0.3f),
	innermassdensity(0.8f),
	doubleskinned(false),
	innerskinradius(2.0f),
	initialintegrity(100.0f)
	{}
	//Creation parameters for a blob
	float initialx,initialy;
	float radius;
	int bodies;
	float damagefiltertime;
	float jointsdamping, jointsfrequency, skinjointsdamping, skinjointsfrequency, massesradius;
	float massesfriction, massesrestitution, massesdensity;
	float innermassradius, innermassdensity;
	bool doubleskinned;
	float innerskinradius;
	float initialintegrity;
};

class BlobController
{
	friend class PlayerAgent;
//Definitions
public:
	typedef std::vector<b2Body*> BodiesVector;
	typedef BodiesVector::iterator BodiesVectorIterator;
	typedef std::vector<b2DistanceJoint*> JointsVector;
	typedef JointsVector::iterator JointsVectorIterator;
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	BlobController(IAgent* relatedagent, PhysicsManagerPointer physicsptr):
	  mRelatedAgent(relatedagent),
	  mPhysicsMgr(physicsptr),
	  mCurrentSpeed(0.0f,0.0f),
	  mFacingDirection(0.0f,0.0f),
	  mRotationDirection(0.0f),
	  mMoveDirection(0.0f,0.0f),
	  mMoveCommand(false),
      mMaxControlForce(0.1f),
	  mMaxSpeed(1.0f),
	  mDestructionSpeed(10.0f),
	  mActive(false),
	  mDestroyed(false),
	  mApplyCollisionDamage(true),
	  mCenterBody(NULL),
	  mBoundingCollisions(0), 
	  mIntegrity(100.0f),
	  mCurrentRadius(2.0f),
	  mDamageForce(0.5f),
	  mDamageFilterCounter(0.0f),
	  mDamaged(false),
	  mIsMainBlob(true),
	  mTotalMass(10.0f),
	  mAffectWhenDying(true),
	  mMainBlob(false)
	{
	}
	~BlobController()
	{
	}

	//----- GET/SET FUNCTIONS -----
	void SetInitialParameters(const BlobParameters& params) { mInitialParams = params; }
	BlobParameters GetInitialParameters() const { return mInitialParams; }   //Return a copy of initial parameters
	b2Body* GetCenterBody() const { return mCenterBody; }			//Get the important center body
	BodiesVector::const_reverse_iterator GetOuterBodiesListStart() { return mBodiesVector.rbegin(); }			//Get the outer bodies
	BodiesVector::const_reverse_iterator GetOuterBodiesListEnd() {return mBodiesVector.rend(); }
	void SetCenterBody(b2Body* body);				 // Set the important center body
	void SetTotalMass(float mass) { assert(mass > 0.0f); mTotalMass = mass; }					//Set total mass of blob 
	void SetMaxControlForce(float newforce) { mMaxControlForce = newforce; }   //Change control force
	void SetMaxSpeed(float newspeed) { mMaxSpeed = newspeed; }	//Change maximum speed
	void SetDestructionSpeed(float newspeed) { mDestructionSpeed = newspeed; }	//Change maximum speed
	void SetDamageForce(float newforce) { mDamageForce =  newforce; } //Change collision damage force
	void SetIntegrity(float newintegrity) { assert (newintegrity <= mInitialParams.initialintegrity); mIntegrity = newintegrity; }
	float GetIntegrity() const { return mIntegrity; }
	float GetIntegrityPercent() const { return mIntegrity / mInitialParams.initialintegrity; }
	void DisableAffectBodiesWhenDeath() { mAffectWhenDying = false; }
	//----- OTHER FUNCTIONS -----
	void StartControlling(bool ismainblob);				 //Call to start logic of controller (finished creation)
	void StopControlling();					//Call to stop control
	void Sleep();							//Call to sleep bodies
	void Update(float dt);					  //Update callback
	void AddBodyToControl(b2Body* body);	  //Add a body (used in creation step)
	void AddJointToList(b2DistanceJoint* joint);	//Add a joint created (used in creation step)
	void Destroy();		//Called to finish control and destroy related bodies and joints
	bool HandleCollision(const CollisionEventData& data);	//Process possible collisions

	//Logic 
	bool IsIntegrityVeryLow(); //Call to know if the blob is about to "die"
	void IgnoreCollisionDamage() { mApplyCollisionDamage = false; }
	void DontIgnoreCollisionDamage() { mApplyCollisionDamage = true; }
	void ApplyDamage() { _handleDamage(mDamageForce); }  //Apply a damage to the blob
	void ApplyHealth() { _handleHealth(); }		//Apply health to the blob
	bool IsBlobDamaged() { return mDamaged; }  //Returns if a collision made damage to blob
	bool IsCollisionInBlobBody(const ContactInfo& collisioninfo); //Internal check function
	bool IsBodyInBlob(b2Body* thebody);	//Check if a body belongs to blob
	void SetAsMainBlob(bool set) { mMainBlob = set; }  //Set as main blob (to optimize bodies calculations)
	//Physical manipulations
	void MoveBlob(const Vector2& direction);  //Command to move the controlled blob
	void ApplyImpulseToBlob(const Vector2& force); //Command to move by force the blob

private:
	//----- INTERNAL VARIABLES -----
	BlobParameters mInitialParams;	//Initial creation parameters
	static const float BLOBBROKENTOLERANCE;	//Broken tolerance ratio
	IAgent* mRelatedAgent;			//Related agent
	PhysicsManagerPointer mPhysicsMgr; //Physics manager
	BodiesVector mBodiesVector;	//Bodies composing the blob
	JointsVector mJointsVector; //Joints composing the blob
	b2Body* mCenterBody;		//The center body
	bool mActive;				//Active tracking
	bool mMoveCommand;			//Move command tracking
	bool mDestroyed;			//Destroyed tracking
	bool mIsMainBlob;			//Tracking if it is main blob

	Vector2 mCurrentSpeed;		//The current movement speed
	Vector2 mMoveDirection;		//The moving direction command
	Vector2 mFacingDirection;	//The facing direction (an average of collisions)
	float mRotationDirection;	//The facing direction expressed in angle
	int mBoundingCollisions;	//Bounding sensor collisions count
	bool mAffectWhenDying;		//Affect bodies (generate death event with data) when blob dies (to make wet)
	float mMaxControlForce;     //Max controller force to apply
	float mMaxSpeed;			//Max inner mass speed
	float mDestructionSpeed;	//Direct destruction speed when colliding
	float mIntegrity;			//Health of the blob
	float mCurrentRadius;		//Current blob radius
	float mDamageForce;			//Trigger force from collision to apply damage
	bool mDamaged;				//Tracking a collision occured, which damaged blob
	float mDamageFilterCounter;	//Filtering collision damage counter
	float mTotalMass;			//Total mass of blob: Used to scale impact forces
	bool mMainBlob;				//Is the currently controlled blob?

	bool mApplyCollisionDamage;	//To disable damage temporary

	//----- INTERNAL FUNCTIONS -----
	void _handleDamage(float amount);  //Internal handle damage function
	void _handleHealth();				//Internal health function
	bool _blobBroken();					//"Blob broken" tracking
	
};

#endif