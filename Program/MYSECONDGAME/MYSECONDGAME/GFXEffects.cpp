/*
	Filename: GFXEffects.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Graphics effects encapsulation
	Comments: It is used with IndieLib
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/



#include "GFXEffects.h"		
#include "IndieLibManager.h"
#include <sstream>

//Mark entity to make effect - FADEIN
void GFXEffects::FadeInEntity(SpritePointer entity,float fadetime, const Vector3 &MovSpeed)
{
	//Check there isnt a registered effect of this type for this entity
	if(!_foundEffect(entity,FADEIN))
	{
		//Fadetime is in SECONDS, MovSpeed is in Pixels/second
		//Create new effect structure
		EffectsParams neweffect;
		
		//Fill parameters
		neweffect.effect = FADEIN;  //Effect type
		neweffect.remainingtime = fadetime*1000;  //Time
		neweffect.timelimit = fadetime*1000;      //Time
		neweffect.movement = MovSpeed/1000;       //Movement speed(pixels/ms)
		neweffect.entity = entity;			  //Entity to affect

		//Setup effect
		neweffect.entity->SetTransparency(0);  //Not shown by transparency
		neweffect.param = 0.0f;			    //Transparency stored here (float value, not the byte of entity)
		neweffect.entity->SetShow(true);		//Show entity

		//Push into entities container
		mAffectedEntities.push_front(neweffect);
	}

}

//Mark entity to make effect - FADEOUT
void GFXEffects::FadeOutEntity(SpritePointer entity,float fadetime, const Vector3 &MovSpeed)
{
	//Fadetime is in SECONDS, MovSpeed is in Pixels/second
	
	//Check there isnt a registered effect of this type for this entity
	if(!_foundEffect(entity,FADEOUT))
	{
		//Create new effect structure
		EffectsParams neweffect;
		
		//Fill parameters
		neweffect.effect = FADEOUT;  //Effect type
		neweffect.remainingtime = fadetime*1000;  //Time
		neweffect.timelimit = fadetime*1000;      //Time
		neweffect.param = static_cast<float>(entity->GetTransparency()); //Transparency stored here (float value, not the byte of entity)
		neweffect.movement = MovSpeed/1000;       //Movement speed
		neweffect.entity = entity;			  //Entity to affect

		//Push into entities container
		mAffectedEntities.push_front(neweffect);
	}
}

//Mark entity to make effect - TIMEDSHOW
//IMPORTANT: TIME MUST BE IN SECONDS!
void GFXEffects::ShowTimedEntity(SpritePointer entity, float time, const Vector3 &MovSpeed)
{
	//time is in SECONDS

	//Check there isnt a registered effect of this type for this entity
	if(!_foundEffect(entity,SHOWTIMED))
	{
		//Create new effect structure
		EffectsParams neweffect;
		
		//Fill parameters
		neweffect.effect = SHOWTIMED;			//Effect type
		neweffect.remainingtime = time*1000;  //Time
		neweffect.timelimit = time*1000;      //Time
		neweffect.movement = MovSpeed/1000;       //Movement speed
		neweffect.entity = entity;			  //Entity to affect

		//Setup effect
		neweffect.entity->SetTransparency(255);  //Full show
		neweffect.entity->SetShow(true);		  //Show entity

		//Push into entities container
		mAffectedEntities.push_front(neweffect);
	}
}

//Prompts if effects are complete for an entity
bool GFXEffects::IsEffectComplete(SpritePointer entity)
{

	EffectsEntitiesListIterator itr;
	
	//LOOP - Search for an entity 
	for(itr = mAffectedEntities.begin(); itr != mAffectedEntities.end(); itr++)
	{
		//Registered?
		if((*itr).entity == entity)
		{
			return false;
		}
	}//LOOP END

	return true;
}

//Stop effects of an entity (And unregister it as affected)
void GFXEffects::StopEffect(SpritePointer entity)
{
	EffectsEntitiesListIterator it= mAffectedEntities.begin();
	
	//LOOP - Delete all effects registered for this entity
	while(it != mAffectedEntities.end())
	{
		if((*it).entity == entity)
		{
			it = mAffectedEntities.erase(it);
		}
		else
		{
			it++;
		}
	}//LOOP END
}

//Update effects display
void GFXEffects::Update(float dt)
{
	
	EffectsEntitiesListIterator itr = mAffectedEntities.begin();

	//LOOP - Search for an entity
	while(itr != mAffectedEntities.end())
	{
		//SWITCH - Effect
		switch((*itr).effect)
		{
		case(FADEIN):
			{
				//Update effect
				_updateFadeIn(dt,(*itr));
			}
			break;
		case(FADEOUT):
			{
				//Update effect
				_updateFadeOut(dt,(*itr));
			}
			break;
		case(SHOWTIMED):
			{		
				//Update effect
				_updateShowTimed(dt,(*itr));
			}
			break;
		default:
			{
				break;
			}

		}//CASE

		//IF- Effect complete
		if((*itr).remainingtime <= 0)
		{
			//Remove effect from list - effect complete
			itr = mAffectedEntities.erase(itr);
		}
		else //ELSE - Effect not complete
		{
			//Next effect
			itr++;
		}//IF
	}//LOOP END	
}
	
//Clear the screen
void GFXEffects::ClearScreen() 
{
	mpILib->Render->ClearViewPort(0,0,0);
}

//Gets an entity from the list (if exists)
GFXEffects::EffectsEntitiesListIterator GFXEffects::_getAffectedEntity(SpritePointer entity)
{
	
	EffectsEntitiesListIterator itr;
	//LOOP - Search for an entity
	for(itr = mAffectedEntities.begin(); itr != mAffectedEntities.end(); itr++)
	{
		if((*itr).entity == entity)
			return itr;
	}//LOOP END

	return mAffectedEntities.end();
}

//Check if there is an effect registered for this entity of the same type
bool GFXEffects::_foundEffect(SpritePointer entity,EffectType type)
{

	EffectsEntitiesListIterator itr;
	//LOOP - Search for an entity and effect 
	for(itr = mAffectedEntities.begin(); itr != mAffectedEntities.end(); itr++)
	{
		//Registered?
		if((*itr).entity == entity
			&&
			(*itr).effect == type
			)
			return true;
	}//LOOP END

	return false;
}
//Update fade in effect for an entity
void GFXEffects::_updateFadeIn(float dt, EffectsParams& affected)
{
	//Get entity
	SpritePointer entity = affected.entity;
			
	//-----Increment transparency level to maximum (255)------
	float rate = 255/affected.timelimit;	//Rate of increment/decrement

	affected.param += (rate*dt);  //Update transparency

	//Check limits
	if(affected.param >= 255.0f)
		affected.param = 255.0f;
	
	//Set the transparency
	entity->SetTransparency(static_cast<unsigned char>(affected.param));
	//--------------------------------------------------------
	//-----------------Move entity if necessary---------------
	//Get position of entity
	Vector3 position(entity->GetPosX(),entity->GetPosY(),entity->GetPosZ());
	//Update position vector by movement
	position += (affected.movement)*dt;

	//Set new position
	entity->SetPosition(static_cast<float>(position.x),
						static_cast<float>(position.y),
						static_cast<int>(position.z)
						);
	//--------------------------------------------------------

	//--------------Update counters---------------------------
	affected.remainingtime -= dt;
	//Check limits
	if(affected.remainingtime < 0)
		affected.remainingtime = 0;
}

void GFXEffects::_updateFadeOut(float dt, EffectsParams& affected)
{
	//Get entity
	SpritePointer entity = affected.entity;
			
	//-----Increment transparency level to maximum (255)------
	float rate = 255/affected.timelimit;	//Rate of increment/decrement

	affected.param -= (rate*dt);  //Update transparency

	//Check limits
	if(affected.param < 0.0f)
		affected.param = 0.0f;
	
	//Set the transparency
	entity->SetTransparency(static_cast<unsigned char>(affected.param));
	//--------------------------------------------------------
	//-----------------Move entity if necessary---------------
	//Get position of entity
	Vector3 position(entity->GetPosX(),entity->GetPosY(),entity->GetPosZ());
	//Update position vector by movement
	position += (affected.movement)*dt;

	//Set new position
	entity->SetPosition(static_cast<float>(position.x),
						static_cast<float>(position.y),
						static_cast<int>(position.z)
						);
	//--------------------------------------------------------

	//--------------Update counters---------------------------
	affected.remainingtime -= dt;
	//Check limits
	if(affected.remainingtime <= 0)
	{
		//ADDITIONALLY, IF FADED COMPLETELY, DONT SHOW
		affected.entity->SetShow(false);
		affected.remainingtime = 0;
	}
}
	
void GFXEffects::_updateShowTimed(float dt, EffectsParams& affected)
{
	//Get entity
	SpritePointer entity = affected.entity;
			
	//--------------Update counters---------------------------
	affected.remainingtime -= dt;
	//Check limits
	if(affected.remainingtime <= 0)
	{
		affected.remainingtime = 0;
		entity->SetShow(false);
	}
}