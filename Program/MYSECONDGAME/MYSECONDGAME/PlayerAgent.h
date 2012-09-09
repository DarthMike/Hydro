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
/*	TODO:
		- ADD SAVE AND LOAD CAPABILITIES; NOW IT IS NOT TOP PRIORITY FOR MY GAMES...
		- IT WOULD BE NICE TO SEPARATE DRAWING AND LOGIC. FOR NOW IT IS LIKE THAT... ALL IN ONE CLASS...
*/

#ifndef _PLAYERAGENT
#define _PLAYERAGENT

//Library dependencies
#include <list>
//Classes dependencies
#include "IAgent.h"
#include "Shared_Resources.h"
#include "BlobController.h"
#include "AnimationController.h"
#include "GFXDefs.h"

//Definitions
//Properties to contain from agent - Inherited
struct PlayerAgentPar : public GameAgentPar
{
		PlayerAgentPar():
		GameAgentPar(),
		maxcontrolforce(0.1f),
		maxspeed(0.1f),
		destructionspeed(10.0f),
		damageforce(0.5f),
		throwingforce(0.1f),
		damageratio(0.5f),
		drawcolor(255,255,255,255),
		originaldrawcolor(255,255,255,255)
		{
			//Determine type
			type = PLAYER;
		}
	
		ContainedSprite sprite;  //GFX
		float maxcontrolforce;	  //Maximum control values
		float maxspeed;
		float destructionspeed;   //Max speed allowed when colliding, if higher, direct destruction
		float damageforce;		  //Minimum collision force to trigger damage to blob
		float throwingforce;	  //Blob throwing force
		float damageratio;		  //Damage scaling when creating small blob
		ColorHSLA drawcolor;			//Drawing color
		ColorHSLA originaldrawcolor;	//Original draw color on creation
		//Assignment operator for fast copying
		PlayerAgentPar& operator=(const PlayerAgentPar &newparams)
		{
			//Deep copy of elements in struct
			position = newparams.position;
			rotation = newparams.rotation;
			sprite = newparams.sprite;
			maxcontrolforce = newparams.maxcontrolforce;
			maxspeed = newparams.maxspeed;
			damageforce =  newparams.damageforce;
			throwingforce = newparams.throwingforce;
			damageratio = newparams.damageratio;
			drawcolor = newparams.drawcolor;
			originaldrawcolor = newparams.originaldrawcolor;
			return (*this);
		}
};

class PlayerAgent : public IAgent
{
//Definitions
public:
	typedef struct BlobCollisionInfo  //A collision with other blob!
	{
		BlobCollisionInfo(BlobControllerPointer nextblob):
	    theotherblob(nextblob),
		numcollisions(1),
		timecollided(0.0f)
		{}

		BlobControllerPointer theotherblob;
		int numcollisions;
		float timecollided;
	}BlobCollisionInfo;
	typedef std::list<BlobControllerPointer> BlobControllerList; //A blob list
	typedef std::list<BlobCollisionInfo> BlobCollisionList;  //A list of collisions with other blobs
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	PlayerAgent(PhysicsManagerPointer physicsptr):
	  mGlobalScale(1.0f),
	  mResY(800.0f),
	  mActive(false),
	  mControlDelay(-1.0f),
	  mAlive(true),
	  mPhysicsMgr(physicsptr),
	  mSecondControl(false),
	  mMassesRadius(0.0f),
	  mSubBlobMassesRadius(0.0f)
	{
		_init();
	};
	virtual ~PlayerAgent(){ _release(); }
	//----- VALUES GET/SET ---------------
	virtual AgentType GetType() { return mParams.type; }			//Get type of agent
	virtual bool IsAlive()  { return (mAlive && mActive); }             //Get if agent was destroyed
	void SetBlobController(BlobControllerPointer pointer);          //Called to set the pointer to blob controller
	//----- OTHER FUNCTIONS --------------
	//Interface implementations
	virtual void UpdateState(float dt);								//Update object status
	virtual bool HandleCollision(const CollisionEventData& data);	//Process possible collisions
	virtual bool HandleEvent(const EventData& data);				//Process possible events
	virtual void HandleOutOfLimits(const OutOfLimitsEventData& data);								//Handle out of limits
	virtual void Create( const GameAgentPar *params);				//Create from params
	virtual void Destroy();											//Destroy body

protected:
	//---- INTERNAL VARIABLES ----
	PlayerAgentPar mParams;					//All parameters needed to create the agent
	bool mActive;							//Internal "active" tracking
	bool mAlive;							//Delayed "alive" signal, it is active delayed 1 logic tick
	float mGlobalScale;						//Global scale to draw entities
	float mResY;							//Resolution of screen in Y axis to draw entities
	float mControlDelay;					//Timer to delay transitions and disable control by player temporally

	PhysicsManagerPointer mPhysicsMgr;			//Physics manager pointer
	AnimationControllerPointer mAnimController;		//Animation controller

	BlobControllerPointer mBlobController;			//Blob center body (the one is controlled)
	float mMassesRadius;								//Radius of external masses (for drawing)
	float mSubBlobMassesRadius;							//Radius of external masses (for drawing other blob)
	BlobControllerPointer mSecondBlobController;	//Blob controller for 2nd thrown blob
	bool mSecondControl;							//To switch control between blobs
	Vector2 mOriginalScale;						//Original Scale of blob face image
	Vector2 mLinearVel;							//Current linear velocity of the controlled blob

	BlobControllerList mBlobsList;				//List of used blobs
	BlobCollisionList mBlobCollisionsList;		//List of collided blobs
	//---- INTERNAL FUNCTIONS ----
	void _drawBlob(BlobControllerPointer thepointer,float radiusoffset,const ColorRGBA& drawcolor); //Draw a blob
	void _updateBlobGFX(float dt);						//GFX updating (indielib)
	void _init();
	void _release();								//Release internal resorces
};

#endif
