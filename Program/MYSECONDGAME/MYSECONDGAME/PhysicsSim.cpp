/*
	Filename: PhysicsSim.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Physics sim playtests
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/


#include "PhysicsSim.h"
#include "PhysicsSimListener.h"
#include "ConfigOptions.h"
#include "XMLParser.h"
#include "ResourceManager.h"
#include "IndieLibManager.h"
#include "Camera2D.h"
#include "AgentsManager.h"
#include "PhysicsManager.h"
#include "GameLevel.h"
#include "LevelBuilder.h"

//Global config options declaration
extern ConfigOptions g_ConfigOptions;  //Global properties of game

//Static variables declarationc
const float PhysicsSim::FIRSTTICKDELAY = 3000.0f;   //3 seconds to start logic updates
//Logic tick update
void PhysicsSim::UpdateGameState(float dt)
{	
	//*************FIRST-TIME LEVEL LOGIC**************************
	//IF - First time execution
	if(mFirstStart)
	{
		//Logic only on first update
		//Generate event to update count of collected drops
		CollectedValuesInfo info(mCurrentLevelPointer->GetDropsCollected(),mCurrentLevelPointer->GetDropsToCollect());
		SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new NewCollectedValuesEvent(Event_NewCollectedValues,info))
											);
		//Reset first time execution
		mFirstStart = false;
	}//IF
	//**************************************************************

	if(mFirstTickDelayCounter < FIRSTTICKDELAY)
		mFirstTickDelayCounter += dt;

	//***********GAME STATE UPDATE**********************************
	if(mFirstTickDelayCounter >= FIRSTTICKDELAY)
	{
		//Update physics with dt supplied
		mPhysicsMgr->Update(dt);
		if(mPhysicsMgr->IsPhysicsStepped())
		{
			float steppedtime (mPhysicsMgr->GetSteppedTime());

			//Update agents
			mAgentsManager->UpdateAgents(steppedtime);
			//Update Level Logic
			mCurrentLevelPointer->Update(steppedtime);
			//Update camera in course
			SingletonIndieLib::Instance()->UpdateCameras(steppedtime);
		}
	}
	//***************************************************************

	//*******************OTHER LEVEL LOGIC***************************
	//IF - Level completed
	if(mLoadNextLevel)
	{
		LoadNextLevel();
		mLoadNextLevel = false;
	}//IF	

	//IF - Restart level
	if(mRestartLevel)
	{
		//Apply command
		RestartCurrentLevel();
		mRestartLevel = false;
	}//IF 
	//****************************************************************
}

//Drawing of whole scene
void PhysicsSim::RenderScene ()
{
	SingletonIndieLib::Instance()->GetCamera("General")->SetAsCurrent();
	//Debug render of physics
	mPhysicsMgr->DebugRender();

	if(mFirstTickDelayCounter < FIRSTTICKDELAY)
		SingletonIndieLib::Instance()->Render->ClearViewPort(0,0,0);
}	

//Load the first level in sequence of levels
void PhysicsSim::LoadFirstLevel()
{
	//Load first level from vector info
	assert (!mLevels.empty());
	_resetAndPrepareLevel(mLevels[0].name,mLevels[0].fullpath);
}

//Load next level in sequence of levels
void PhysicsSim::LoadNextLevel()
{
	assert(mCurrentLevelPointer);

	//Local variables
	std::string levelname (mCurrentLevelPointer->GetName());
	
	//Search this level in levels vector
	LevelsVectorIterator levelitr;
	//LOOP - Search level
	for(levelitr = mLevels.begin(); levelitr != mLevels.end(); ++levelitr)
	{
		if((*levelitr).name == mCurrentLevelPointer->GetName())
		{
			//Mark as completed
			(*levelitr).completed = true;
			break;
		}
	}//LOOP END

	//-----Mark this level as completed in file------
	//Reset, delete all elements from level
	
	//Open,load document, modify and close
	ticpp::Document levelsdoc(mLevelsFilePath);
	levelsdoc.LoadFile();	//Parsing			
	ticpp::Element* levelssection = levelsdoc.FirstChildElement("Levels");  //Read levels section
	ticpp::Iterator<ticpp::Element> levelsecitr;
	//LOOP - Search for level
	for(levelsecitr = levelsecitr.begin(levelssection); levelsecitr != levelsecitr.end(); levelsecitr++)
	{
		//If id is the same
		if((*levelsecitr).GetAttribute("Id") == levelname)
		{
			(*levelsecitr).SetAttribute("C",true);
			break;
		}	
	}//LOOP END
	levelsdoc.SaveFile();

	//-----Find next level and load it-----
	++levelitr;

	if(levelitr != mLevels.end())
	{
		//Once found, restart it by loading it again from scratch
		_resetAndPrepareLevel((*levelitr).name,(*levelitr).fullpath);
	}
	else
	{
		//TODO: PROCESS END OF LEVELS
	}
}

//Load a level given an index in sequence of levels
void PhysicsSim::LoadLevel(int)
{
	//assert(static_cast<size_t>(index) < mLevels.size());
	//TODO
}

void PhysicsSim::RestartCurrentLevel()
{
	//IF - A previous level existed
	if(mCurrentLevelPointer)
	{
		//Local variables
		std::string levelname (mCurrentLevelPointer->GetName());
	
		//Search this level in levels vector
		LevelsVectorIterator levelitr;
		//LOOP - Search level
		for(levelitr = mLevels.begin(); levelitr != mLevels.end(); ++levelitr)
		{
			if((*levelitr).name == mCurrentLevelPointer->GetName())
			{
				break;
			}
		}//LOOP END

		assert(levelitr != mLevels.end());

		//Once found, restart it by loading it again from scratch
		_resetAndPrepareLevel((*levelitr).name,(*levelitr).fullpath);
	}//IF
}

//Initialize game
void PhysicsSim::_init()
{
	//Init levels filepath
	//Read from xml levels file, the path of this level
	mLevelsFilePath = g_ConfigOptions.GetScriptsPath() + "Levels.xml";  //Path Hard-coded
	_loadLevels();  //Parse levels file
	//Create event listener
	mEventListener = new PhysicsSimListener(this);
}

//Release resources
void PhysicsSim::_release()
{
	//IF - A level was loaded
	if(mCurrentLevelPointer)
	{
		//Reset, delete all elements from level
		std::string prevlevelname = mCurrentLevelPointer->GetName();
		mCurrentLevelPointer.reset();
		//Release resources for this level
		SingletonResourceMgr::Instance()->ReleaseLevelResources(prevlevelname);
	}//IF

	//Event Listener
	if(mEventListener)
	{
		delete mEventListener;
		mEventListener = NULL;
	}
}

//Parse levels file
void PhysicsSim::_loadLevels()
{
	//********************LOAD LEVELS***************************************
	//Load the correct level
	//Open and load document
	//-----------Load level path and name from levels file----
	ticpp::Document levelsdoc(mLevelsFilePath);
	levelsdoc.LoadFile();	//Parsing	
	ticpp::Element* levelssection = levelsdoc.FirstChildElement("Levels");

	ticpp::Iterator<ticpp::Element> itr;
	//LOOP - Load all levels
	for(itr = itr.begin(levelssection); itr != itr.end(); itr++)
	{
		std::string levelid ((*itr).GetAttribute("Id"));
		//TODO: CHECK CORRECTNESS
		/*
		//Id
		std::string spriteId = child->GetAttribute("Id");
		if(
			spriteId == ""
			||
			mEntitiesMap.find(spriteId) != mEntitiesMap.end()
		 )
		 throw GenericException("Failure while reading '" + filepath + "'Id '" + spriteId + "' not correct (repeated or empty)!",GenericException::FILE_CONFIG_INCORRECT);
		*/
		std::string levelfilepath ((*itr).GetAttribute("Path"));
		//TODO: CHECK CORRECTNESS
		/*
		//Id
		std::string spriteId = child->GetAttribute("Id");
		if(
			spriteId == ""
			||
			mEntitiesMap.find(spriteId) != mEntitiesMap.end()
		 )
		 throw GenericException("Failure while reading '" + filepath + "'Id '" + spriteId + "' not correct (repeated or empty)!",GenericException::FILE_CONFIG_INCORRECT);	
		*/
		std::string fullpath (g_ConfigOptions.GetWorkingPath() + levelfilepath);
		bool completed(false);
		(*itr).GetAttribute("C",&completed);

		mLevels.push_back(LevelInfo(levelid,completed,fullpath));
	}//LOOP END

	if(mLevels.empty())
		throw GenericException("No levels to load!! Review corresponding file!",GenericException::FILE_CONFIG_INCORRECT);

		//******************LEVELS LOADED***********************************************	
}
//Init game state and load a level
void PhysicsSim::_resetAndPrepareLevel(const std::string& levelname, const std::string& levelfilepath)
{
	//***********************INIT OBJECTS MANAGERS******************************
	//Create physics manager with game parameters
	//Read game parameters for physics
	const PhysicsConfig physicsconf = g_ConfigOptions.GetPhysicsConfiguration(); //Physics
	mPhysicsMgr.reset();
	#ifdef _DEBUGGING //DEBUG MODE: REGISTER DEBUG DRAW
		mPhysicsMgr = PhysicsManagerPointer(
					new PhysicsManager(physicsconf.gravity,
										  physicsconf.timestep,
										  physicsconf.iterations,
										  physicsconf.worldaabbmax,
										  physicsconf.worldaabbmin,
										  SingletonIndieLib::Instance()->Box2DDebugRender)
					);
	#else //NOT DEBUG MODE: DONT REGISTER DEBUG DRAW
		mPhysicsMgr = PhysicsManagerPointer(
			         new PhysicsManager(physicsconf.gravity,
										  physicsconf.timestep,
										  physicsconf.iterations,
										  physicsconf.worldaabbmax,
										  physicsconf.worldaabbmin)
					);
	#endif
	
	//Create agents manager
	mAgentsManager.reset();
	mAgentsManager = AgentsManagerPointer(new AgentsManager(mPhysicsMgr));
	//***********************MANAGERS CREATED***********************************

	//***************************LOAD LEVEL*************************************
	//IF - A previous level existed
	if(mCurrentLevelPointer)
	{
		//Reset, delete all elements from level
		std::string prevlevelname = mCurrentLevelPointer->GetName();
		mCurrentLevelPointer.reset();
		//Release resources for this level
		SingletonResourceMgr::Instance()->ReleaseLevelResources(prevlevelname);
	}//IF
	
	//As we restarted level, empty queues of events
	SingletonGameEventMgr::Instance()->EmptyEventQueues();

	//--------Load resources for this level---------------------
	//Load resources for level
	SingletonResourceMgr::Instance()->LoadLevelResources(levelname);

	//---Level creation with builder object---
	LevelBuilder thebuilder(this);
			
	//Call creation of object
	thebuilder.LoadLevel(levelfilepath,levelname);
	
	//No errors, get level pointer
	mCurrentLevelPointer = thebuilder.GetCreatedLevel();
	//*****************************LEVEL LOADED*********************************

	//************************RESET INTERNAL LOGIC VARIABLES********************
	mFirstStart = true;
	mGameOver = false;
	mLoadNextLevel = false;
	mRestartLevel = false;
	mFirstTickDelayCounter = 0.0f;
	//*************************VARIABLES RESET**********************************
}

bool PhysicsSim::_handleEvents(const EventData& theevent)
{
	bool eventprocessed(false);

	//Events related to level logic
	//IF - Drop collected event
	if(theevent.GetEventType() == Event_DropCollected)
	{
		//Just increment counter in current level
		mCurrentLevelPointer->IncrementCollectedCount();

		//Generate event to update count of collected drops
		CollectedValuesInfo info(mCurrentLevelPointer->GetDropsCollected(),mCurrentLevelPointer->GetDropsToCollect());
		SingletonGameEventMgr::Instance()->QueueEvent(
												EventDataPointer(new NewCollectedValuesEvent(Event_NewCollectedValues,info))
												);

		//IF - Level completed
		if(mCurrentLevelPointer->AllDropsCollected())
		{
			//Send message of level complete
			SingletonGameEventMgr::Instance()->QueueEvent(
										EventDataPointer(new EventData(Event_LevelCompleted))
										);
		}//IF
	}//ELSE - Restart level  event
	else if(theevent.GetEventType() == Event_RestartLevel)
	{
		//Command to restart level!
		mRestartLevel = true;
	}//ELSE - Load next level event
	else if(theevent.GetEventType() == Event_NextLevel)
	{
		//Command to load next level!
		mLoadNextLevel = true;
	}//IF

	return eventprocessed;
}