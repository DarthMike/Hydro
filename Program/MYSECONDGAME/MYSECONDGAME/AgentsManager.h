/*
	Filename: AgentsManager.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to create and manage agents in-game
	Comments: A game agent is anything that reacts to game logic: Physic box, enemy, triggerpoint, coins, bombs...
			  This class calls update methods for created agents in run-time, and acts as "factory" to agents
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/


// TODO: ADDED TEMP REFERENCE TO PHYSICS MANAGER BECAUSE OF HACKS IN FORCES FOR AI AGENT!!!! CHANGE IT!
#ifndef _AGENTSMGR
#define _AGENTSMGR

//Library dependencies
#include <map>
#include <string>
//Classes dependencies
#include "LogManager.h"
#include "GenericException.h"
#include "Shared_Resources.h"
#include "Creatable_Agents.h"

//Forward declarations
class PhysicsManager;
class AgentsManagerListener;

class AgentsManager
{
//Friends
	friend class AgentsManagerListener;
//Definitions
public:
	//A map to contain agents
	typedef std::map<std::string, IAgent*> GameAgentsMap;
	typedef GameAgentsMap::iterator GameAgentsMapIterator; //the iterator for the map
public:
	//----CONSTRUCTORS/DESTRUCTORS----
	AgentsManager(PhysicsManagerPointer physicsptr):
	  mPhysicsManager(physicsptr)
	{
		_init();
	}
	~AgentsManager()
	{
		_release();
	};
	//----- VALUES GET/SET ---------------
	PhysicsManagerPointer GetPhysicsManager() { return mPhysicsManager; }
	IAgent* GetAgent(const std::string &name){ return(_searchAgent(name)); }
	//----- OTHER FUNCTIONS --------------
	IAgent* CreateNewAgent(const std::string &name,const GameAgentPar *newagentparams );	//Create a new agent instance
	void UpdateAgents(float dt); //Update all available agents state
private:
	//---- INTERNAL VARIABLES ---- 
	GameAgentsMap mAgentsMap;				//The map to contain agents
	static int mAgentCount;					//An internal count of added agents
	AgentsManagerListener* mEventListener;	//Internal friend object to manage event receiving
	PhysicsManagerPointer mPhysicsManager;
	//---- INTERNAL FUNCTIONS ----	
	void _init();
	void _release();
	IAgent* _searchAgent(const std::string &name);   //Search agent
	bool _agentExists(IAgent* agenttosearch); //Query if an agent exists
	bool _handleEvents(const EventData& eventdata);	//Handle collisions
};

#endif
