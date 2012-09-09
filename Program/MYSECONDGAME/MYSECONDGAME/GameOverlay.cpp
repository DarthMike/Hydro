/*
	Filename: GameOverlay.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to overlay presentation (UI)
	Comments: It depends of IndieLib
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/		

#include "GameOverlay.h"
#include "GameOverlayListener.h"
#include "Math.h"
#include "GameMouse.h"
#include "GameKeyboard.h"
#include "ConfigOptions.h"
#include "GameEventManager.h"
#include "IndieLibManager.h"
#include "GFXEffects.h"
#include "GameLevel.h"
#include "LevelBuilder.h"
#include "GameEvents.h"
#include "Camera2D.h"
#include "SoundManager.h"
#include "ResourceManager.h"

//Global config options declaration
extern ConfigOptions g_ConfigOptions;  //Global properties of game

//Definition of entities file to editor
const std::string GameOverlay::mOverlayAssetsFileName = "OverlayAssets.xml";

//Update method
void GameOverlay::Update(float dt)
{
	//Play level music
	SingletonSoundMgr::Instance()->PlayMusic("LevelMusic",true);

	//Update created dynamic animations
	std::list<AnimationControllerPointer>::iterator itr = mDynamicAnimations.begin();

	//LOOP - For each element created
	while(itr != mDynamicAnimations.end())
	{
		//Update controller
		(*itr)->Update(dt);
		if((*itr)->IsCurrentAnimationFinished())	
		{
			itr = mDynamicAnimations.erase(itr);
		}
		else
		{
			++itr;
		}

	}//LOOP END

	//Update elements of overlay
	//Update input controllers
	mGameMouse->Update(dt);
	mGameKeyBoard->Update(dt);
	
	//Update internal timers
	mCounter += dt;
	mPositionUpdateDelay += dt;


	//Debug text is updated by messages
	//Messages text is updated by events
	//Collectable text is updated by events

	//Handle overlay logic:
	IndieLibManager* Ilib = SingletonIndieLib::Instance();

	//---Game Over--
	//IF - Game over
	if(mGameOver)
	{
		//Local control variable
		static bool processed(false);
		//Display message to pass to next level
		//IF - Processed text display for next level
		if(mCounter > 3000.0f && !processed)
		{
			//Show corresponding message
			SpritePointer messagetext = mOverlayAssets->GetEntity("MessagesText");
			Vector2 position = SingletonIndieLib::Instance()->GetCamera("Overlay")->GetPositionPix();
			int coordz = messagetext->GetPosZ();
			messagetext->SetPosition(static_cast<float>(position.x),
									 static_cast<float>(position.y),
									 coordz);
			mMessagesText = mMessagesText + "\nPress SPACE to try again\nESC to exit";
			messagetext->SetText(const_cast<char*>(mMessagesText.c_str()));
			SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(messagetext,1.5f,Vector3(0.1,0,0));
			processed = true;
		}
		else if(mCounter > 3000.0f && processed)
		{
			//IF - Mouse was pressed now
			if(Ilib->Input->OnKeyPress(IND_SPACE))
			{
				//Send message to restart level
				SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new EventData(Event_RestartLevel))
											);

				_resetVariables();
			}//IF	
		
			//TODO: PROCESS RETURN TO MENU
		}
		else
		{
			processed = false;
		}//IF
	}//IF
		
	//---Restart---
	SpritePointer overlaymouse = mOverlayAssets->GetEntity("MouseCursor");
	SpritePointer overlaybutton = mOverlayAssets->GetEntity("RestartButton");
	//IF - Query for collision between mouse and restart indicator
	if(Ilib->Entity2dManager->IsCollision(overlaybutton.get(),"*",overlaymouse.get(),"*"))
	{
		overlaybutton->SetTransparency(255);
		//IF - Not level completed
		if(!mLevelCompleted)
		{
			//IF - Mouse was pressed now
			if(Ilib->Input->OnMouseButtonPress(IND_MBUTTON_LEFT))
			{
				//Send message to restart level
				SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new EventData(Event_RestartLevel))
											);

				_resetVariables();
			}//IF
		}//IF
	}//ELSE - No collision
	else
	{
		overlaybutton->SetTransparency(100);
		overlaybutton->SetTint(255,255,255);
	}//IF

	//---Level completed--
	//IF - Level completed
	if(mLevelCompleted)
	{
		//Local control variable
		static bool processed(false);
		//Display message to pass to next level
		//IF - Processed text display for next level
		if(mCounter > 4000.0f && !processed)
		{
			//Show corresponding message
			SpritePointer messagetext = mOverlayAssets->GetEntity("MessagesText");
			Vector2 position = SingletonIndieLib::Instance()->GetCamera("Overlay")->GetPositionPix();
			int coordz = messagetext->GetPosZ();
			messagetext->SetPosition(static_cast<float>(position.x),
									 static_cast<float>(position.y),
									 coordz);
			mMessagesText = "Press SPACE to continue";
			messagetext->SetText(const_cast<char*>(mMessagesText.c_str()));
			SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(messagetext,1.5f,Vector3(0.1,0,0));
			processed = true;
		}
		else if(mCounter > 4000.0f && processed)
		{
			//IF - Mouse was pressed now
			if(Ilib->Input->OnKeyPress(IND_SPACE))
			{
				//Send message to restart level
				SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new EventData(Event_NextLevel))
											);
				_resetVariables();
			}//IF			
		}
		else
		{
			processed = false;
		}//IF
	}//IF
}

//Render necessary elements
void GameOverlay::Render()	
{
	mGameMouse->Render();
	mGameKeyBoard->Render();
}

//Event handling
bool GameOverlay::_handleEvents(const EventData& theevent)
{
	//Local variables
	bool eventprocessed (false);

	//Receiving of events:
	//Check type of event
	//IF - Event is Update position of camera with blob
	if(theevent.GetEventType() == Event_BlobPosition)
	{
		if(!mFreeCameraMode && mPositionUpdateDelay > 1200.0f)
		{
			const BlobPositionEvent& data =  static_cast<const BlobPositionEvent&>(theevent);
			Vector2 newposition = data.GetNewPosition();
			Vector2 newspeed = data.GetNewSpeed();
			float maxspeed = data.GetMaxSpeed();

			assert(maxspeed > 0.01);
			//Percentage about max speed vector
			newspeed /= maxspeed;

			SingletonIndieLib::Instance()->GetCamera("General")->MoveTo(newposition + (newspeed * 0.1));
		}

		eventprocessed = true;
	}//ELSE IF - Game Over event
	else if(theevent.GetEventType() == Event_GameOver && !mLevelCompleted)
	{
		//IF  - MESSAGES UPDATE
		if(mDisplayNextMessages)
		{
			mDisplayNextMessages = false;
			//Show corresponding message
			SpritePointer messagetext = mOverlayAssets->GetEntity("MessagesText");
			Vector2 position = SingletonIndieLib::Instance()->GetCamera("Overlay")->GetPositionPix();
			int coordz = messagetext->GetPosZ();
			messagetext->SetPosition(static_cast<float>(position.x),
									 static_cast<float>(position.y),
									 coordz);
			mMessagesText = "Game Over!";
			messagetext->SetText(const_cast<char*>(mMessagesText.c_str()));
			SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(messagetext,2.0f,Vector3(0.1,0,0));
			mCounter = 0.0f;
			mGameOver = true;
		}//IF
	}//ELSE IF - Solid bodies collision
	else if(theevent.GetEventType() == Event_SolidCollision)
	{
		const SolidCollisionEvent& data = static_cast<const SolidCollisionEvent&>(theevent);
		const SolidCollisionInfo& info = data.GetInfo();
		
		//Generate a random number to play a sound
		int newrand (SingletonMath::Instance()->NewRandom(1,100));
		//Play sound depending on material
		switch(info.material)
		{
		case(GENERIC):
			{
				if(newrand < 50)
				{
					SingletonSoundMgr::Instance()->PlayAction("GenericClank1");
				}
				else
				{
					SingletonSoundMgr::Instance()->PlayAction("GenericClank2");
				}
			}
			break;
		case(WOODISH):
			{
				if(newrand < 25)
				{
					SingletonSoundMgr::Instance()->PlayAction("WoodClank1");
				}
				else if(newrand < 50 && newrand >= 25)
				{
					SingletonSoundMgr::Instance()->PlayAction("WoodClank2");
				}
				else if(newrand < 75 && newrand >= 50)
				{
					SingletonSoundMgr::Instance()->PlayAction("WoodClank3");
				}
				else
				{
					SingletonSoundMgr::Instance()->PlayAction("WoodClank4");
				}
			}
			break;
		case(METALLIC):
			{
				//TODO: METALLIC PLAYS
			}
			break;
		case(STONE):
			{
				if(newrand < 25)
				{
					SingletonSoundMgr::Instance()->PlayAction("StoneClank1");
				}
				else if(newrand < 50 && newrand >= 25)
				{
					SingletonSoundMgr::Instance()->PlayAction("StoneClank2");
				}
				else if(newrand < 75 && newrand >= 50)
				{
					SingletonSoundMgr::Instance()->PlayAction("StoneClank3");
				}
				else
				{
					SingletonSoundMgr::Instance()->PlayAction("StoneClank4");
				}
			}	
			break;
		default:
			{
				//NEVER!!!
				assert(false);
			}	
			break;
		}

	}//ELSE IF - Blob health update event
	else if(theevent.GetEventType() == Event_BlobHealth)
	{
		const BlobHealthEvent& data = static_cast<const BlobHealthEvent&>(theevent);
		float newhealth = data.GetHealth();
		if(mLastHealth != newhealth)
		{
			mFreeCameraMode = false;  //No free camera! health update!
			//Update health message 
			std::stringstream ss; 
			ss<<"Health: "<<newhealth<<"\0";
			mHealthText = ss.str();
			SpritePointer messagetext = mOverlayAssets->GetEntity("HealthText");
			messagetext->SetText(const_cast<char*>(mHealthText.c_str()));
		}

		//Sounds
		if(mLastHealth > newhealth)
		{
			//Play a damage sound randomly
			int newrand (SingletonMath::Instance()->NewRandom(1,100));
			if(newrand <= 50)
			{
				SingletonSoundMgr::Instance()->PlayAction("BlobDamage1");
			}
			else
			{
				SingletonSoundMgr::Instance()->PlayAction("BlobDamage2");
			}
		}

		mLastHealth = data.GetHealth(); 
	}//ELSE - Blob died
	else if(theevent.GetEventType() == Event_BlobDeath)
	{
		const BlobDeathEvent& data = static_cast<const BlobDeathEvent&>(theevent);

		//Play death sound
		//IF - Main blob died
		if(data.IsMainBlob())
		{
			SingletonSoundMgr::Instance()->PlayAction("BlobDeath");
		}
		else//ELSE - other blob died
		{
			SingletonSoundMgr::Instance()->PlayAction("SmallBlobDeath");
			//Show when blob last died for a time: Reset delay counter
			mPositionUpdateDelay = 0.0f;
		}

		//Play animation where it died
		//Get animation resource from resource manager		
		AnimationPointer theanimation (SingletonResourceMgr::Instance()->GetAnimationResource("BlobDeath"));
		//IF - Check animation exists
		if(theanimation)
		{
			//SMALL HACK: ANIMATION IS CREATED BY A PARTICLE EDITOR (MAGIC PARTICLES)
			//THE TROUBLE IS WITH DIMENSIONS OF FRAME TO MAKE IT NICE-LOOKING. SO a A HARD-CODED SCALING FACTOR
			//CONTROLS DE DIFFERENCE BETWEEN ORIGINAL DIMS AND REAL BLOB DIMS
			//NOT GOOD BUT IT IS TO GET THE JOB DONE... I WANT TO FINISH THIS DEMO!

			//Create new sprite entity
			SpritePointer newentity(new IND_Entity2d());
			int targetlayer = SingletonIndieLib::Instance()->GetCamera("General")->GetLayer();  //Render in "General" layer
			SingletonIndieLib::Instance()->Entity2dManager->Add(targetlayer,newentity.get());
			newentity->SetAnimation(theanimation.get());
			newentity->SetHotSpot(0.5f,0.5f);
			newentity->ShowGridAreas(false); 
			newentity->ShowCollisionAreas(false);
			
			//Scaling of dimensions of all entity frames
			Vector2 originaldims (newentity->GetAnimation()->GetHighWidth(0), newentity->GetAnimation()->GetHighHeight(0));
			
			//HACK
			originaldims.x *= (0.2f * 0.7f);  //Real dims is much lower than total image in start 
			originaldims.y *= 0.2f;
			Vector2 radiusvec (data.GetRadius(),data.GetRadius());
			Vector2 destinationdims (SingletonIndieLib::Instance()->FromCoordToPix(radiusvec));
			//Scale
			float scalex(static_cast<float>(destinationdims.x / originaldims.x));
			float scaley(static_cast<float>(destinationdims.y / originaldims.y));
			newentity->SetScale(scalex,scaley);
			//Position
			Vector2 pospix = SingletonIndieLib::Instance()->FromCoordToPix(data.GetPosition());
			newentity->SetPosition(static_cast<float>(pospix.x),static_cast<float>(pospix.y),1000);  //With Z big enough to be over everything in layer
			//Create an animation controller for this entity and push it in dynamic container
			AnimationControllerPointer newanimcontroller (new AnimationController(newentity,0,false,true)); //Without looping
			mDynamicAnimations.push_front(newanimcontroller);
		}//IF
	}//ELSE - A drop was collected NOTE: DIFFERENT FROM VALUES UPDATING OF COLLECTED!
	else if(theevent.GetEventType() == Event_DropCollected)
	{
		//Play Drop collected sound
		SingletonSoundMgr::Instance()->PlayAction("Collected1");
	}//ELSE - "Collected" values updated
	else if(theevent.GetEventType() == Event_NewCollectedValues)
	{
		const NewCollectedValuesEvent& data = static_cast<const NewCollectedValuesEvent&>(theevent);
		//Get the values of total and collected so far
		int total = data.GetTotalToCollect();
		int collected = data.GetCollectedSoFar();
		std::stringstream ss;
		ss<<"Collect:  "<<"\n"<<collected<<" of "<<total;
		mCollectedText = ss.str();
		SpritePointer collectedtext = mOverlayAssets->GetEntity("CollectedText");
		collectedtext->SetText(const_cast<char*>(mCollectedText.c_str()));
	}//ELSE - "Level completed" event
	else if(theevent.GetEventType() == Event_LevelCompleted && !mGameOver)
	{	
		//The data is empty, just show a rewarding message ;)
		if(mDisplayNextMessages)
		{
			mDisplayNextMessages = false;
			SpritePointer messagetext = mOverlayAssets->GetEntity("MessagesText");
			Vector2 position = SingletonIndieLib::Instance()->GetCamera("Overlay")->GetPositionPix();
			int coordz = messagetext->GetPosZ();
			messagetext->SetPosition(static_cast<float>(position.x),
									 static_cast<float>(position.y),
									 coordz);
			mMessagesText = "Good job!\nLevel Completed!";
			messagetext->SetText(const_cast<char*>(mMessagesText.c_str()));
			SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(messagetext,4.0f,Vector3(0.1,0,0));
			mCounter = 0.0f;
			mLevelCompleted = true;
			//Fade-Out Level music
			SingletonSoundMgr::Instance()->FadeOutMusic("LevelMusic",3.0f);
			//Play Sound:
			SingletonSoundMgr::Instance()->PlayAction("LevelComplete");
		}
	}//ELSE - "Exit game" event
	else if(theevent.GetEventType() == Event_ExitGame)
	{
		//Just stop music
		//SingletonSoundMgr::Instance()->FadeOutMusic("LevelMusic",1.5f);
		SingletonSoundMgr::Instance()->StopMusic("LevelMusic",true);
	}//ELSE - Debug string
	else if(theevent.GetEventType() == Event_DebugString)
	{
		const DebugMessageEvent& data = static_cast<const DebugMessageEvent&>(theevent);
		static int linenum = 0;
		if(mDebugLines <= 5)
		{
			mDebugLines += 1;
		}
		else
		{
			mDebugMSGstream.seekp(0);
			mDebugMSGstream.seekg(0);
			mDebugMSGstream.clear();
			mDebugLines = 0;
		}
		linenum++;
		std::string newmsg = data.GetString();;
		mDebugMSGstream<<"\n"<<linenum<<":";
		mDebugMSGstream<<newmsg;
		//Just display the messages
		mDebugText = mDebugMSGstream.str();
		SpritePointer debugtext = mOverlayAssets->GetEntity("DebugText");
		debugtext->SetText(const_cast<char*>(mDebugText.c_str()));
		eventprocessed = true;
	}

	return (eventprocessed);
}

void GameOverlay::_init()
{
	//**********************LOAD NEEDED ASSETS*************************************
	//Load entities related to this screen
	std::string assetsfilepath = g_ConfigOptions.GetScriptsPath() + mOverlayAssetsFileName;
	
	//---Creation of entities through level builder---
	LevelBuilder thebuilder; //Load level assets - Container mode
			
	//Call creation of object (level)
	thebuilder.LoadLevel(assetsfilepath,"-**NOEDIT**OVERLAYASSETS-",true);  //Only assets loading (last "true")
	
	//No errors, get level pointer (used to store entities assets)
	mOverlayAssets = thebuilder.GetCreatedLevel();
	//**********************************ASSETS LOADED*******************************
	
	//**********************SET INTERNAL VARIABLES**********************************
	mScaleFactor = SingletonIndieLib::Instance()->GetGeneralScale();
	mResX = static_cast<float>(SingletonIndieLib::Instance()->Window->GetWidth());
	mResY = static_cast<float>(SingletonIndieLib::Instance()->Window->GetHeight());

	//Check all entities were loaded
	SpritePointer pt;
	pt = mOverlayAssets->GetEntity("RestartButton");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'RestartButton'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}

	pt = mOverlayAssets->GetEntity("DebugText");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'DebugText'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	#ifdef _DEBUGGING
	pt->SetText("--DEBUG TEXT HERE--");
	#endif
	pt = mOverlayAssets->GetEntity("CollectedText");
	pt->SetTint(0,0,0);
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'CollectedText'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt = mOverlayAssets->GetEntity("HealthText");
	pt->SetTint(0,0,0);
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'HealthText'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt = mOverlayAssets->GetEntity("MessagesText");

	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'MessagesText'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt->SetTint(0,0,0);
	
	_resetVariables();
	//*********************CREATE INPUT CONTROLLERS********************************
	pt = 	mOverlayAssets->GetEntity("MouseCursor");
	mGameMouse = new GameMouse(pt,this);
	mGameKeyBoard = new GameKeyBoard(this);

	//*****************INPUT CONTROLLERS CREATED***********************************

	//Register game overlay listener
	if(!mListener)
		mListener = new GameOverlayListener(this);
}

void GameOverlay::_resetVariables()
{
	mMessagesText = "";
	mCollectedText = "";
	mFreeCameraMode = false;
	mDisplayNextMessages = true;
	mLastHealth = 0.0f;
	mCounter = 0.0f;
	mLevelCompleted = false;
	mGameOver = false;
}

void GameOverlay::_release()
{
	//Input controllers
	if(mGameMouse)
	{
		delete mGameMouse;
		mGameMouse = NULL;
	}
	if(mGameKeyBoard)
	{
		delete mGameKeyBoard;
		mGameKeyBoard = NULL; 
	}

	if(mListener)
	{
		delete mListener;
		mListener = NULL;
	}
}