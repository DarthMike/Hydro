/*
	Filename: GFXEffects.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Graphics effects encapsulation
	Comments: It is used with IndieLib
	EFFECTS ADDED:
		FADEIN: Display for a time the entity incrementing its alpha, so it appears slowly to screen
		FADEOUT: Display for a time the entity, maybe moving it and decrementing its alpha, so it dissapears
		TIMED: Display for a time this entity.
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

/*CHANGELOG
	- This class is no longer a Singleton. It was poor decision, as it belongs to the singleton manager "IndieLib"
	- Implemented the same functionality using shared pointers, for ease of use and better managing... you know, shared ptrs are addicting..
	- Implemented Scale effect
	- Memory leak solved while creating new effect in heap not stack!
*/

/*
TODO: IMPLEMENT LOTS OF COOL EFFECTS! ;)
*/
#ifndef _GFXEffects
#define _GFXEffects

//Library dependencies	
#include <list>
//Class dependencies
#include "LogManager.h"
#include "Vector3.h"
#include "Shared_Resources.h"


//Forward declarations
class IndieLibManager;
class IND_Entity2d;

class GFXEffects
{
	//Definitions
public:
	typedef enum EffectType{FADEIN,FADEOUT,SHOWTIMED,SCALE}; //Effects types
	
	typedef struct EffectsParams{
		EffectsParams()
			:effect(FADEIN),
			remainingtime(0.0f),
			timelimit(5.0f),
			param(0),
			movement(0,0,0)
		{}
		EffectType effect;
		float remainingtime;
		float timelimit;
		float param;		//IMPORTANT: HERE WE STORE TEMPORAL VALUES, AS FOR GRANULARITY, SOMETIMES
						    //WORKING WITH TYPES LIKE UNSIGNED BYTE WONT WORK EVERY UPDATE (DT IS SMALL)
		Vector3 movement;
		SpritePointer entity;
	}EffectsParams; //Struct where we hold info about effect

protected:
	typedef std::list<EffectsParams> EffectsEntitiesList;
	typedef EffectsEntitiesList::iterator EffectsEntitiesListIterator;
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GFXEffects(IndieLibManager* ilibptr):mpILib(ilibptr)
	{
	}
	~GFXEffects()
	{
		if(!mAffectedEntities.empty())
		{
			SingletonLogMgr::Instance()->AddNewLine("GFXEffects","Graphics effects destructed but there are still effects in course!",LOGDEBUG);
		}
		else
			SingletonLogMgr::Instance()->AddNewLine("GFXEffects","Graphics Effects manager released",LOGDEBUG);
	}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	void FadeInEntity(SpritePointer entity,float fadetime, const Vector3 &MovSpeed);   //Mark entity to make effect - FADEIN
	void FadeOutEntity(SpritePointer entity,float fadetime, const Vector3 &MovSpeed);  //Mark entity to make effect - FADEOUT
	void ShowTimedEntity(SpritePointer entity, float time, const Vector3 &MovSpeed);   //Mark entity to make effect - TIMEDSHOW
	void ScaleEntity(SpritePointer entity, float time, const Vector3 &MovSpeed);	   //Mark entity to make effect - SCALE
	void Update(float dt);						//Update effects display
	void StopEffect(SpritePointer entity);	//Stop effects of an entity (And unregister it as affected)
	bool IsEffectComplete(SpritePointer entity);   //Prompts if effects are complete for an entity
	void ClearScreen();							    //Clear screen to black

protected:
	//----- INTERNAL VARIABLES -----
	IndieLibManager* mpILib;
	EffectsEntitiesList mAffectedEntities;		 //A vector storing which entities (text entities)will have effects
	//----- INTERNAL FUNCTIONS -----
	EffectsEntitiesListIterator _getAffectedEntity(SpritePointer entity);  //Gets an entity from the list (if exists)
	bool _foundEffect(SpritePointer entity,EffectType type);
	void _updateFadeIn(float dt, EffectsParams& affected);
	void _updateFadeOut(float dt, EffectsParams& affected);
	void _updateShowTimed(float dt, EffectsParams& affected);
};

#endif