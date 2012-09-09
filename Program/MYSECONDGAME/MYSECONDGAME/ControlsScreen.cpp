/*
	Filename: ControlsScreen.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Implementation of a screen in game - CONTROLSSCREEN
	Comments: Dependant of IndieLib Graphics library - derived from abtract class "GameScreen"
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/



#include "ControlsScreen.h"
#include "LevelBuilder.h"
#include "ConfigOptions.h"
#include "IndieLibManager.h"
#include "GFXEffects.h"
#include "SoundManager.h"

//Global config options declaration
extern ConfigOptions g_ConfigOptions;  //Global properties of game

//Constants declarations
const std::string ControlsScreen::mScreenAssetsFileName = "ControlsScreenAssets.xml";

//To execute on start
void ControlsScreen::ExecuteEnter()
{
	//Perform a fadein of main text
	SpritePointer firsttext (mScreenAssets->GetEntity("Controls_Page1"));
	SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(firsttext,3.0f,Vector3(0,0,0));

	//Play intro music
	SingletonSoundMgr::Instance()->PlayMusic("IntroMusic",true);   //With looping
}
void ControlsScreen::ExecuteRender()
{
	//Rendering is done by entities -> IndieLib
}

//To execute while rendering
void ControlsScreen::ExecuteLogicUpdate(float dt)
{
	//Increment internal counter
	mCounter += dt;

	//Finally, process inputs from user
	_processInputs();
}

//To execute at exit
void ControlsScreen::ExecuteExit()
{
}

//Process possible events
bool ControlsScreen::HandleEvent(const EventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}


//Return a game state if it is necessary to exit
GameState ControlsScreen::IsExit()
{
	//Check if it is needed to change screen
	if(mExit)
		return (MAINMENU); //To main menu 
	else
		return(NOSTATE); //No change
}

//Internal init
void ControlsScreen::_init()
{
	//**********************LOAD NEEDED ASSETS*************************************
	//Load entities related to this screen
	std::string assetsfilepath = g_ConfigOptions.GetScriptsPath() + mScreenAssetsFileName;
	
	//---Creation of entities through level builder---
	LevelBuilder thebuilder; //Load level assets - Container mode
			
	//Call creation of object (level)
	thebuilder.LoadLevel(assetsfilepath,"-**NOEDIT**CONTROLSCREENASSTETS**-",true);  //Only assets loading (last "true")
	
	//No errors, get level pointer (used to store entities assets)
	mScreenAssets = thebuilder.GetCreatedLevel();
	//**********************************ASSETS LOADED*******************************

	//Check all entities were loaded
	SpritePointer pt;
	pt = mScreenAssets->GetEntity("Controls_Page1");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'Controls_Page1'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}

	//Set initial properties
	pt->SetShow(false);

	pt = mScreenAssets->GetEntity("Rain_Background");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'Rain_Background'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}

}
//Internal release
void ControlsScreen::_release()
{
	//Dynamically allocated memory...	
	//Release from memory and graphics system is done by base class
}

void ControlsScreen::_processInputs()
{
	IndieLibManager* pIlib = SingletonIndieLib::Instance();

	//--------Exit-----------
	if(pIlib->Input->OnKeyPress(IND_ESCAPE)
	   ||
	   pIlib->Input->OnKeyPress(IND_RETURN)
	   ||
	   pIlib->Input->OnKeyPress(IND_MBUTTON_LEFT)
	   ||
	   pIlib->Input->OnKeyPress(IND_MBUTTON_RIGHT))
	{
		mExit = true;
	}
}