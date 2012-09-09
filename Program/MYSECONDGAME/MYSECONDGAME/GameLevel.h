/*
	Filename: GameLevel.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which encapsulates all contained level data
	Comments: Agents are contained in agent manager, and physical bodies data is managed by 
	          physics engine, so here we find graphical stuff, and logic stuff. This class acts as
			  a default and base class (interface) for levels with specialized logic. I dont loose time
			  integrating LUA in this project, but this code here should be implemented in LUA with scripts
			  if the game is big enough.
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _GAMELEVEL
#define _GAMELEVEL

//Library dependencies	
#include <map>
#include <list>
//Class dependencies
#include "LogManager.h"
#include "GenericException.h"
#include "Shared_Resources.h"
#include "Vector2.h"

class GameLevel
{
friend class LevelBuilder;    //To let it access to internals in creation phase
friend class EditorLogic;	  //To let it edit a level at run-time
//Definitions
public:
	//Map of string->entity
	typedef std::map<std::string,SpritePointer> EntitiesMap;
	typedef EntitiesMap::iterator EntitiesMapIterator;
protected:
	typedef struct ParallaxInfo{ //Info which will be stored in a container to
		                         //loop all parallax cameras info by their speed
		//Constructor
		ParallaxInfo():
		relvelocity(100.0f,100.0f),
		relzoom(0.0f),
		startpos(0,0)
		{}
		//The camera pointer
		Camera2DPointer thecamera;
		//The relative velocity of this parallax scroll camera
		Vector2 relvelocity;
		//The relative zoom movement for this parallax layer
		float relzoom;
		//The starting position of parallax layer cam
		Vector2 startpos;
	}ParallaxInfo;

	//Map of parallax layers
	typedef std::list<ParallaxInfo> ParallaxList;
	typedef ParallaxList::iterator ParallaxListIterator;
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameLevel(const std::string& levelname):
    mCameraPosition(0,0),
	mCurrentZoom(1.0f),
	mDropsToCollect(1),
	mDropsCollected(0),
    mName(levelname)
	{
		_init();
	}
	~GameLevel()
	{
		_release(); //Release level resources
	}
	//----- GET/SET FUNCTIONS -----
	std::string GetName() { return mName; }  //Return level name
	virtual SpritePointer GetEntity(const std::string& entityname); //Get an entity in the level
	int GetDropsCollected() { return mDropsCollected; }  //Get the number of drops collected
	int GetDropsToCollect() { return mDropsToCollect; }	//Get the total number of drops to collect
	//----- OTHER FUNCTIONS -----
	void Update(float dt);	//The update logic call (if there is logic to process)

	void IncrementCollectedCount() { mDropsCollected++; }   //Increment collected count
	bool AllDropsCollected() { return (mDropsToCollect == mDropsCollected); }  //Query if all drops are collected
  
protected:
	//----- INTERNAL VARIABLES -----
	EntitiesMap mEntitiesMap;  //Rendered entities map
	ParallaxList mParallaxList;  //Parallax scroll info list
	
	std::string mName;			//Level name
	int mDropsCollected;		//Drops collected so far
	int mDropsToCollect;		//Number of drops to collect
	
	Camera2DPointer mGeneralCamPointer;  //The general camera pointer
	Vector2 mCameraPosition;	//The GENERAL camera STORED position (other cameras will derive from this)
	float mCurrentZoom;		//The GENERAL camera STORED zoom (other cameras will derive from this)
	//----- INTERNAL FUNCTIONS -----
	void _init();	 //Init function
	void _release(); //Release function
	//Sorting function class for parallax layers
	class ParallaxCompareClass
	{
	public:
		bool operator()(const GameLevel::ParallaxInfo& firstlayer, const GameLevel::ParallaxInfo& secondlayer);
	};
};

#endif

