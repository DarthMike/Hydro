/*
	Filename: GameLevel.cpp
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

#include "GameLevel.h"
#include "IndieLibManager.h"
#include "Camera2D.h"
#include "Math.h"

//Get an entity in the level
SpritePointer GameLevel::GetEntity(const std::string& entityname)
{
	//Find entity in level
	EntitiesMapIterator it = mEntitiesMap.find(entityname);
	//IF - Found entity
	if(it != mEntitiesMap.end())
	{
		return((*it).second);
	}//IF
	else //ELSE - Entity not found
	{
		//Write failure
		SingletonLogMgr::Instance()->AddNewLine("GameLevel::GetEntity","Trying to access non-existing level entity: '" + entityname + "'",LOGEXCEPTION);
	}

	return (SpritePointer());

}

//The update logic call (if there is logic to process)
void GameLevel::Update(float)
{	
	static bool firstupdate = true; //Local control variables
	assert(mGeneralCamPointer);	//Assert camera pointer was created
	
	//IF - It is the first update
	if(firstupdate)
	{
		//Get tracking values only, without updating (not to make a big change on-start)
		mCameraPosition = mGeneralCamPointer->GetPosition();
		mCurrentZoom = mGeneralCamPointer->GetZoom();
		firstupdate = false;
	}//ELSE - NOT FIRST UPDATE
	else
	{
		//Move cameras per difference of movement
		Vector2 refpos = mGeneralCamPointer->GetPosition();
		//Vector2 speed =  mGeneralCamPointer->GetSpeed();
		//float zoomdifference = mGeneralCamPointer->GetZoom() - mCurrentZoom;
		//float zoomspeed = zoomdifference * dt/1000; //Get the zoom speed of last iteration
		ParallaxListIterator itr;
		//LOOP - Move all parallax layers
		for(itr = mParallaxList.begin();itr != mParallaxList.end();++itr)
		{
			//Apply a change of speed as per layer config
			/*Vector2 speedtoapply(0,0);
			speedtoapply.x = speed.x * (*itr).relvelocity.x;
			speedtoapply.y = speed.y * (*itr).relvelocity.y;
			(*itr).thecamera->
			(*itr).thecamera->Translate(speedtoapply);*/
			Vector2 camerapos(0,0);
			camerapos.x = refpos.x * (*itr).relvelocity.x;
			camerapos.y = refpos.y * (*itr).relvelocity.y;
			(*itr).thecamera->SetPosition(camerapos);

			//TODO: Apply a change of zoom as per layer config
			//float zoomtoapply = zoomspeed * (*itr).relzoom;
			
		}//LOOP END
	}

	//Store previous position
	mCameraPosition = mGeneralCamPointer->GetPosition();
}

//Init function
void GameLevel::_init()
{
	mGeneralCamPointer = SingletonIndieLib::Instance()->GetCamera("General");
	mGeneralCamPointer->SetLimitingMoves(true);
}

//Release function
void GameLevel::_release()
{
	//Entities from level must be deleted from indielib, as they maintain 
	//a naked pointer reference... 

	//IndieLib pointer
	IndieLibManager *ILib =  SingletonIndieLib::Instance();

	//Created entities - Delete them from being rendered
	EntitiesMapIterator eit;
	//LOOP - Release entities from Indielib
	for(eit = mEntitiesMap.begin(); eit!=mEntitiesMap.end();++eit)
	{
		if((*eit).second)
		{
			ILib->Entity2dManager->Delete((*eit).second.get());
		}
	}//LOOP END
	mEntitiesMap.clear();

	//Registered cameras - Delete them from being rendered
	ParallaxListIterator pit;
	//LOOP - Deregister cameras
	for(pit = mParallaxList.begin(); pit!=mParallaxList.end();++pit)
	{
		if((*pit).thecamera)
		{
			ILib->DeRegisterCamera((*pit).thecamera->GetName());
		}
	}//LOOP END
	mParallaxList.clear();
}

//Sorting function class for parallax layers
bool GameLevel::ParallaxCompareClass::operator()(const GameLevel::ParallaxInfo& firstlayer, const GameLevel::ParallaxInfo& secondlayer)
{
	int layer1 = firstlayer.thecamera->GetLayer();
	int layer2 = secondlayer.thecamera->GetLayer();

	//IF - Not irreflexive and intransitive (ejem... weak ordering means elements CANT be equal...)
	if(layer1 == layer2)
		throw GenericException("Camera '" + firstlayer.thecamera->GetName() + "' and '" + secondlayer.thecamera->GetName() +"' Have the same layer assigned! REVIEW XML FILES",GenericException::INVALIDPARAMS);

	return(layer1 < layer2);
}