/*
	Filename: EditorLogicListener.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Event listener for editor
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _EDITORLOGICLISTENER
#define _EDITORLOGICLISTENER
//Library dependencies

//Class dependencies
#include "GameEventManager.h"

//Forward declarations
class EditorLogic;
class EventData;

class EditorLogicListener : public IEventListener
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	EditorLogicListener(EditorLogic* editorptr)
		:mName("EditorLogicListener"),
		mEditor(editorptr)
	{
		//Register events to process
		SingletonGameEventMgr::Instance()->AddListener(this,Event_RenderInLayer);  //Special renderings in layer
		//Physics collisions
		SingletonGameEventMgr::Instance()->AddListener(this,Event_NewCollision);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_PersistantCollision);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_DeletedCollision);
		SingletonGameEventMgr::Instance()->AddListener(this,Event_CollisionResult);
	}
	~EditorLogicListener()
	{
		//Deregister events to process
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_RenderInLayer); //Special renderings in layer
		//Physics collisions
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_NewCollision);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_PersistantCollision);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_DeletedCollision);
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_CollisionResult);
	}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	//Implementation of handling
	virtual bool HandleEvent(const EventData& theevent);
	//Implementation of name
	virtual const std::string& GetName()
	{
		return mName;
	}
	//----- PUBLIC VARIABLES ------	

protected:
	//----- INTERNAL VARIABLES -----
	std::string mName;
	EditorLogic* mEditor;
	//----- INTERNAL FUNCTIONS -----
};

#endif
