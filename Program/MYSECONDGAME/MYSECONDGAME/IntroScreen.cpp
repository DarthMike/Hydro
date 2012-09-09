/*
	Filename: IntroScreen.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Implementation of a screen in game - INTROSCREEN
	Comments: Dependant of IndieLib Graphics library - derived from abtract class "GameScreen"
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/


#include "IntroScreen.h"
#include "LevelBuilder.h"
#include "ConfigOptions.h"
#include "IndieLibManager.h"
#include "GFXEffects.h"
#include "SoundManager.h"

//Global config options declaration
extern ConfigOptions g_ConfigOptions;  //Global properties of game

//Constants declarations
const std::string IntroScreen::mScreenAssetsFileName = "IntroScreenAssets.xml";

//To execute on start
void IntroScreen::ExecuteEnter()
{
	//Perform a fadein of main text
	SpritePointer textsprite (mScreenAssets->GetEntity("IntroText"));
	textsprite->SetText("A game by:\n\nMiguel Angel Quinones");
	textsprite->SetTint(255,255,255);
	SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(textsprite,4.0f,Vector3(0,0,0));

	//Play intro music
	SingletonSoundMgr::Instance()->PlayMusic("IntroMusic",true);   //With looping
}
void IntroScreen::ExecuteRender()
{
	//Rendering is done by entities -> IndieLib
}

//To execute while rendering
void IntroScreen::ExecuteLogicUpdate(float dt)
{
	//Internal counter
	static float elapsedtime = 0.0f;

	elapsedtime += dt;

	//Fade out text once shown a time
	static bool textfaded = false;
	if(elapsedtime >= 7000.0f && !textfaded)
	{
		SpritePointer textsprite (mScreenAssets->GetEntity("IntroText"));
		SingletonIndieLib::Instance()->GFXEffectsManager->FadeOutEntity(textsprite,2.5f,Vector3(0,0,0));
		textfaded = true;
	}

	//Exit screen timed (once faded out)
	if(elapsedtime > 11000.0f)
	{
		mExit = true;
	}
}

//To execute at exit
void IntroScreen::ExecuteExit()
{
}

//Process possible events
bool IntroScreen::HandleEvent(const EventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}


//Return a game state if it is necessary to exit
GameState IntroScreen::IsExit()
{
	//Check if it is needed to change screen
	if(mExit)
		return (MAINMENU); //To main menu 
	else
		return(NOSTATE); //No change
}

//Internal init
void IntroScreen::_init()
{
	//**********************LOAD NEEDED ASSETS*************************************
	//Load entities related to this screen
	std::string assetsfilepath = g_ConfigOptions.GetScriptsPath() + mScreenAssetsFileName;
	
	//---Creation of entities through level builder---
	LevelBuilder thebuilder; //Load level assets - Container mode
			
	//Call creation of object (level)
	thebuilder.LoadLevel(assetsfilepath,"-**NOEDIT**INTROSCREENASSTETS**-",true);  //Only assets loading (last "true")
	
	//No errors, get level pointer (used to store entities assets)
	mScreenAssets = thebuilder.GetCreatedLevel();
	//**********************************ASSETS LOADED*******************************

	//Check all entities were loaded
	SpritePointer pt;
	pt = mScreenAssets->GetEntity("IntroText");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'IntroText'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}

	//Set initial properties
	pt->SetShow(false);

}
//Internal release
void IntroScreen::_release()
{
	//Dynamically allocated memory...	
	//Release from memory and graphics system is done by base class
}