/*
	Filename: GameApp.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate Game application functionality (startup and cleanup)
	Comments: Implementation of game loop as compromise between good harware and bad hardware. A 
			  mix of variable-step and fixed-step, to get the work done ;)
	Attribution: Based in part of tutorial at gametuto.com - Javier López López (javilop.com)
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "GameApp.h"
#include "IndieLibManager.h"
#include "SoundManager.h"
#include "GameEventManager.h"
#include "ResourceManager.h"
#include "GFXEffects.h"

//Global config options declaration
extern ConfigOptions g_ConfigOptions;  //Global properties of game

//Static variables
const unsigned int GameApp::mLogicUpdateTicks = 20;   // More or less 50Hz

//System initialization of game
void GameApp::_init()
{
	//Init log managing system
	SingletonLogMgr::Instance()->AddNewLine("GameApp::_init()","Launching game...",LOGNORMAL);
	
	//Read config options
	g_ConfigOptions.ReadConfigOptions();

	//Init event manager
	SingletonGameEventMgr::Instance();

	//Init IndieLib
	SingletonIndieLib::Instance();

	//Init Sound System
	SingletonSoundMgr::Instance();

	//Init Resources
	SingletonResourceMgr::Instance();

	//Ready to run
	//Initialize game state
	mStateManager = new GameStateManager(mEditMode);  //Create a state manager for game (create the game with all states)
	mStateManager->Init();

	//Init timer
	mMainTimer.Start();

	mInitialized = true;
	
}

//System release of game
void GameApp::_release()
{
	SingletonLogMgr::Instance()->AddNewLine("GameApp::_release()","Game shutdown...",LOGNORMAL);

	//Kill game logic
	if(mStateManager)
	{
		delete mStateManager;
		mStateManager = NULL;
	}

	//Release Resource Manager
	SingletonResourceMgr::Destroy();

	//Release Sound System
	SingletonSoundMgr::Destroy();

	//Release IndieLib
	SingletonIndieLib::Destroy();

	//Release Event Manager
	SingletonGameEventMgr::Destroy();
	
	//Release Math Manager (if used)
	SingletonMath::Destroy();

	SingletonLogMgr::Instance()->AddNewLine("GameApp::_release()","Game shutdown complete",LOGNORMAL);

	//Release logging system
	SingletonLogMgr::Destroy();
}

//Run (Main game Loop)
void GameApp::Run()
{
	//Assert game is initialized
	assert(mInitialized);
	
	//Local variables for timing
	float dt(0.0f);
	float gametimestepms (g_ConfigOptions.GetPhysicsConfiguration().timestep * 1000);

	IndieLibManager *Ilib (SingletonIndieLib::Instance());
	mMainTimer.Start();

	//LOOP - GAME LOOP
	while(!mStateManager->Exit())
	{			
		dt = mMainTimer.GetTicks();
		mMainTimer.Start();

		//------ Game Logic Update ------
		//------ Input Capture ----------
		//Update input every frame (needed for IndieLib)
		Ilib->Input->Update();
		//-------------------------------
		//Manage states of game - This will update all game logic and things to render
		mStateManager->UpdateLogic(dt);  //Note: Inside, game updates can be made many times... (physics engine)
		
		//Events processing
		SingletonGameEventMgr::Instance()->Update(dt);
		//Update logic of sound system
		SingletonSoundMgr::Instance()->Update(dt);
		//Update Graphics effects of IndieLib
		Ilib->GFXEffectsManager->Update(dt);
		//--------------------------------
		
		//---------Render---------------
		Ilib->Render->BeginScene();
		Ilib->Render->ClearViewPort(0,0,0);
		//Render routine
		Ilib->RenderRoutine();
		//Render other elements (from the game state)
		mStateManager->Render();
		Ilib->Render->EndScene ();	
		//-------------------------------	

		//--------FPS LIMITING----------------
		dt = mMainTimer.GetTicks();
		//Patch to work in high-speed hardware (I dont interpolate and those things
		//as it complicates drawing, displaying, etc.. Instead, a generic sleep is done
		//to keep things nice... (http://gafferongames.com/game-physics/fix-your-timestep/)
		//It is not nice and shouldnt be used in a serious comercial game... but...
		//Generally, the effect will be LIMITING FPS. For my game is not a problem. Why want
		//500 FPS if 100 FPS is enough for my eye? Waste of power? It serves purpose to cap FPS
		//and to adapt with dt throghout the game... Is this solution for GAME LOOP for this project
		//IF - dt big enough (floats division and that stuff... ;))
		if(dt >0.0001)
		{
			//IF - Surpassed max framerate (4 times timestep for physics engine)
			if((gametimestepms / dt ) > 4.0f)
			{
				//Sleep nicely to fill this time without updating
				::Sleep(static_cast<DWORD>(gametimestepms - dt));
			}
		}//ELSE IF - Too fast (out of precission)
		else if(dt <= 0.0001)
		{
			::Sleep(static_cast<DWORD>(1));
		}//IF
	
	}//LOOP END
}