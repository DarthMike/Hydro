/*
	Filename: PhysicsSim.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Physics sim playtests - Game logic class for my game
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
#ifndef _PHYSICSSIM
#define _PHYSICSSIM

/*
	TODO:
		-Quite hacky code to create entities and agents. For now works but there is code duplicated. 
		 Work needs to be made in builder classes to encapsulate creation of singular XML elements
		-Same hacky code for different levels loading. Correct it making an internal list of available
		 levels and parsing it on creation of class
*/

//Library dependencies

//Class dependencies
#include "General_Resources.h"
#include "TestEventListener.h"
#include "Shared_Resources.h"

//Forward declarations
class PhysicsSimListener;

class PhysicsSim
{
	friend class PhysicsSimListener; //Listener is friend to this, as it has access to internals... (just a handle really)
//Definitions
typedef struct LevelInfo   //Level info structure
{
	//CONSTRUCTOR
	LevelInfo(const std::string& name, bool completed, const std::string& fullpath):
		name(name),
		fullpath(fullpath),
		completed(completed)
	{}

	//Variables
	std::string name;
	std::string fullpath;
	bool completed;
}LevelInfo;

//Containers for level info
typedef std::vector<LevelInfo> LevelsVector;
typedef LevelsVector::iterator LevelsVectorIterator;

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	PhysicsSim():
	  mEventListener(NULL),
	  mFirstStart(false),
	  mGameOver(false),
	  mRestartLevel(false),
	  mFirstTickDelayCounter(0.0f)
	{
		//Init resources, load managers and create game agents
		_init();
	}
	~PhysicsSim()
	{
		_release();
	}

	//----- GET/SET FUNCTIONS -----
	PhysicsManagerPointer GetPhysicsManager() { return mPhysicsMgr; }
	AgentsManagerPointer GetAgentsManager() { return mAgentsManager; }
	//----- OTHER FUNCTIONS -----
	void UpdateGameState(float dt);		//Logic tick update
	void RenderScene();					//Drawing of whole scene
	void LoadFirstLevel();				//L
	void LoadNextLevel();
	void LoadLevel(int index);
	void RestartCurrentLevel();

private:
	//----- INTERNAL VARIABLES -----
	std::string mLevelsFilePath;			//Levels filepath
	GameLevelPointer mCurrentLevelPointer; //Current loaded level data pointer
	static const float FIRSTTICKDELAY;
	float mFirstTickDelayCounter;					//Delay to update first logic tick
	bool mFirstStart;						//Logic tracking for first start of level
	bool mGameOver;							//Game Over tracking
	bool mLoadNextLevel;					//Command to load next level
	bool mRestartLevel;						//Command to restart level
	LevelsVector mLevels;					//Levels info container
	PhysicsManagerPointer mPhysicsMgr;	//Physics manager pointer
	AgentsManagerPointer mAgentsManager;	//Agents Manager pointer
	
	PhysicsSimListener* mEventListener;  //Listener for events
	
	//Event Listener (Debug purposes only)
	TestEventListener DebugTestListener;	
	//----- INTERNAL FUNCTIONS -----
	//Init and release
	void _init();
	void _release();
	void _loadLevels();
	//Load a level, reset managers and restart internal logic variables
	void _resetAndPrepareLevel(const std::string& levelname, const std::string& levelfilepath);

	//Event handling
	bool _handleEvents(const EventData& theevent);
};

#endif