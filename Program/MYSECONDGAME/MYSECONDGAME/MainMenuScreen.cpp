/*
	Filename: MainMenuScreen.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Implementation of a screen in game - MAINMENUSCREEN
	Comments: Dependant of IndieLib Graphics library - derived from abtract class "GameScreen"
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "MainMenuScreen.h"
#include "LevelBuilder.h"
#include "ConfigOptions.h"
#include "SoundManager.h"
#include "IndieLibManager.h"
#include "GFXEffects.h"
#include "Camera2D.h"

//Global config options declaration
extern ConfigOptions g_ConfigOptions;  //Global properties of game

//Constants declarations
const std::string MainMenuScreen::mScreenAssetsFileName = "MainMenuScreenAssets.xml";

//To execute on start
void MainMenuScreen::ExecuteEnter()
{
	//Put camera in "Good" Position
	SingletonIndieLib::Instance()->GetCamera("General")->SetMaxMinZoom(10.0f,0.1f);
	SingletonIndieLib::Instance()->GetCamera("General")->SetZoom(1.0f);
	SingletonIndieLib::Instance()->GetCamera("General")->SetPosition(Vector2(0,0));
	
	SpritePointer rainsprite (mScreenAssets->GetEntity("Rain_Background"));
	SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(rainsprite,1.5f,Vector3(0,0,0));

	//Play music (if it wasnt playing)
	SingletonSoundMgr::Instance()->PlayMusic("IntroMusic",true);   //With looping
}

void MainMenuScreen::ExecuteRender()
{
	//Rendering is done by entities -> IndieLib
}

//To execute while rendering
void MainMenuScreen::ExecuteLogicUpdate(float dt)
{
	//Update internal counter
	mCounter += dt;

	//Update mouse display position
	float newx (static_cast<float>(SingletonIndieLib::Instance()->Input->GetMouseX()));
	float newy (static_cast<float>(SingletonIndieLib::Instance()->Input->GetMouseY()));
	int resx (SingletonIndieLib::Instance()->Window->GetWidth());
	int resy (SingletonIndieLib::Instance()->Window->GetHeight());

	Vector2 campospix (SingletonIndieLib::Instance()->GetCamera("General")->GetPositionPix());
	SpritePointer mouse(mScreenAssets->GetEntity("MouseCursor"));
	int posz (mouse->GetPosZ());
	mouse->SetPosition(static_cast<float>(newx + campospix.x - resx/2) ,static_cast<float>(newy + campospix.y - resy/2),posz);

	//Correct position of drop sprite to center
	SpritePointer dropsprite (mScreenAssets->GetEntity("Drop"));
	Vector2 targetpos(SingletonIndieLib::Instance()->GetCamera("General")->GetPosition());
	Vector2 pospix(dropsprite->GetPosX(),dropsprite->GetPosY());
	Vector2 posreal(SingletonIndieLib::Instance()->FromPixToCoord(pospix));
	
	//IF - Not in good position yet
	if(!posreal.IsEqualWithToleranceTo(targetpos,0.1f))
	{
		//Correct position
		Vector2 correctdir(targetpos - posreal);
		correctdir.Normalise();
		posreal += correctdir * 0.0008f * dt;

		//Apply to entity
		pospix = SingletonIndieLib::Instance()->FromCoordToPix(posreal);
		int z (dropsprite->GetPosZ());
		dropsprite->SetPosition(static_cast<float>(pospix.x),static_cast<float>(pospix.y),z);
	}//IF

	//Make waves effect
	//Drop
	IND_Surface* spritesurf (dropsprite->GetSurface());
	int numblocksx (spritesurf->GetBlocksX());
	int numblocksy (spritesurf->GetBlocksY());
	int heightblock(spritesurf->GetHeightBlock());
	int widthblock(spritesurf->GetWidthBlock());

	//LOOP - All blocks of the sprite grid(X)
	for (int i = 1; i < numblocksx; i++)
	{
		//LOOP - All blocks of the sprite grid(Y)
		for (int j = 1; j < numblocksy; j++)
		{
			//Update using sinusoidal position
			spritesurf->SetVertexPos (j, 
									  i,										 //(w* t  + alpha)
									  static_cast<int>(((j * heightblock + cosf (mCounter/200 + (i + j)/2) *7))), 
									  static_cast<int>(((i * widthblock	+ sinf (mCounter/200 + (i + j)/2) * 7)))
									 );
		}//LOOP END
	}//LOOP END

	//Title
	SpritePointer titlesprite(mScreenAssets->GetEntity("Menu_Title"));
	spritesurf = titlesprite->GetSurface();
	numblocksx = spritesurf->GetBlocksX();
	numblocksy = spritesurf->GetBlocksY();
	heightblock = spritesurf->GetHeightBlock();
	widthblock = spritesurf->GetWidthBlock();

	//LOOP - All blocks of the sprite grid(X)
	for (int i = 1; i < numblocksx; i++)
	{
		//LOOP - All blocks of the sprite grid(Y)
		for (int j = 1; j < numblocksy; j++)
		{
			//Update using sinusoidal position
			spritesurf->SetVertexPos (j, 
									  i,										 //(w* t  + alpha)
									  static_cast<int>(((j * heightblock + cosf (mCounter/300 + (i + j)/2 + 0.5f) *2))), 
									  static_cast<int>(((i * widthblock	+ sinf (mCounter/300 + (i + j)/2 + 0.5f) * 2)))
									 );
		}//LOOP END
	}//LOOP END

	//Menu Item 1
	SpritePointer menuitem1(mScreenAssets->GetEntity("Menu_Start"));
	spritesurf = menuitem1->GetSurface();
	numblocksx = spritesurf->GetBlocksX();
	numblocksy = spritesurf->GetBlocksY();
	heightblock = spritesurf->GetHeightBlock();
	widthblock = spritesurf->GetWidthBlock();

	//LOOP - All blocks of the sprite grid(X)
	for (int i = 1; i < numblocksx; i++)
	{
		//LOOP - All blocks of the sprite grid(Y)
		for (int j = 1; j < numblocksy; j++)
		{
			//Update using sinusoidal position
			spritesurf->SetVertexPos (j, 
									  i,										 //(w* t  + alpha)
									  static_cast<int>(((j * heightblock + cosf (mCounter/300 + (i + j)/2 + 0.8f) *2))), 
									  static_cast<int>(((i * widthblock	+ sinf (mCounter/300 + (i + j)/2 + 0.8f) * 2)))
									 );
		}//LOOP END
	}//LOOP END

	//Menu Item 2
	SpritePointer menuitem2(mScreenAssets->GetEntity("Menu_Exit"));
	spritesurf = menuitem2->GetSurface();
	numblocksx = spritesurf->GetBlocksX();
	numblocksy = spritesurf->GetBlocksY();
	heightblock = spritesurf->GetHeightBlock();
	widthblock = spritesurf->GetWidthBlock();

	//LOOP - All blocks of the sprite grid(X)
	for (int i = 1; i < numblocksx; i++)
	{
		//LOOP - All blocks of the sprite grid(Y)
		for (int j = 1; j < numblocksy; j++)
		{
			//Update using sinusoidal position
			spritesurf->SetVertexPos (j, 
									  i,										 //(w* t  + alpha)
									  static_cast<int>(((j * heightblock + cosf (mCounter/300 + (i + j)/2 + 1) *2))), 
									  static_cast<int>(((i * widthblock	+ sinf (mCounter/300 + (i + j)/2 + 1) * 2)))
									 );
		}//LOOP END
	}//LOOP END

	//Menu Item 3
	SpritePointer menuitem3(mScreenAssets->GetEntity("Menu_Credits"));
	spritesurf = menuitem3->GetSurface();
	numblocksx = spritesurf->GetBlocksX();
	numblocksy = spritesurf->GetBlocksY();
	heightblock = spritesurf->GetHeightBlock();
	widthblock = spritesurf->GetWidthBlock();

	//LOOP - All blocks of the sprite grid(X)
	for (int i = 1; i < numblocksx; i++)
	{
		//LOOP - All blocks of the sprite grid(Y)
		for (int j = 1; j < numblocksy; j++)
		{
			//Update using sinusoidal position
			spritesurf->SetVertexPos (j, 
									  i,										 //(w* t  + alpha)
									  static_cast<int>(((j * heightblock + cosf (mCounter/300 + (i + j)/2 + 1.2f) *2))), 
									  static_cast<int>(((i * widthblock	+ sinf (mCounter/300 + (i + j)/2 + 1.2f) * 2)))
									 );
		}//LOOP END
	}//LOOP END

	//Menu Item 4
	SpritePointer menuitem4(mScreenAssets->GetEntity("Menu_Controls"));
	spritesurf = menuitem4->GetSurface();
	numblocksx = spritesurf->GetBlocksX();
	numblocksy = spritesurf->GetBlocksY();
	heightblock = spritesurf->GetHeightBlock();
	widthblock = spritesurf->GetWidthBlock();

	//LOOP - All blocks of the sprite grid(X)
	for (int i = 1; i < numblocksx; i++)
	{
		//LOOP - All blocks of the sprite grid(Y)
		for (int j = 1; j < numblocksy; j++)
		{
			//Update using sinusoidal position
			spritesurf->SetVertexPos (j, 
									  i,										 //(w* t  + alpha)
									  static_cast<int>(((j * heightblock + cosf (mCounter/300 + (i + j)/2 + 1.2f) *2))), 
									  static_cast<int>(((i * widthblock	+ sinf (mCounter/300 + (i + j)/2 + 1.2f) * 2)))
									 );
		}//LOOP END
	}//LOOP END

	//IF - First transition
	if(mCounter > 6000.0f && !mFirstTransition)
	{
		SpritePointer titlesprite (mScreenAssets->GetEntity("Menu_Title"));
		SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(titlesprite,2.0f,Vector3(0,0,0));
		mFirstTransition = true;
	}

	//IF - Second transition
	if(mCounter > 9000.0f && !mSecondTransition)
	{
		SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(menuitem1,3.0f,Vector3(0,0,0));
		SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(menuitem2,3.0f,Vector3(0,0,0));
		SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(menuitem3,3.0f,Vector3(0,0,0));
		SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(menuitem4,3.0f,Vector3(0,0,0));
		mSecondTransition = true;		
	}

	//Finally, process inputs from user
	_processInputs();
}

//To execute at exit
void MainMenuScreen::ExecuteExit()
{
	//Fade-Out music
	//SingletonSoundMgr::Instance()->FadeOutMusic("IntroMusic",2.0f);
	if(mStart)
		SingletonSoundMgr::Instance()->StopMusic("IntroMusic",true);
}

//Process possible events
bool MainMenuScreen::HandleEvent(const EventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}


//Return a game state if it is necessary to exit
GameState MainMenuScreen::IsExit()
{
	//Check if it is needed to change screen
	if(mExit)
		return (EXIT); //Exit
	else if (mStart)
		return (NORMALGAME);  //To game
	else if(mControls)
		return (CONTROLS);	//To credits
	else if(mCredits)
		return (CREDITS);	//To credits

	return(NOSTATE); //No change
}

//Internal init
void MainMenuScreen::_init()
{
	//**********************LOAD NEEDED ASSETS*************************************
	//Load entities related to this screen
	std::string assetsfilepath = g_ConfigOptions.GetScriptsPath() + mScreenAssetsFileName;
	
	//---Creation of entities through level builder---
	LevelBuilder thebuilder; //Load level assets - Container mode
			
	//Call creation of object (level)
	thebuilder.LoadLevel(assetsfilepath,"-**NOEDIT**MENUSCREENASSTETS**-",true);  //Only assets loading (last "true")
	
	//No errors, get level pointer (used to store entities assets)
	mScreenAssets = thebuilder.GetCreatedLevel();
	//**********************************ASSETS LOADED*******************************

	//Check all entities were loaded and init them
	SpritePointer pt;
	pt = mScreenAssets->GetEntity("Rain_Background");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'Rain_Background'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt->SetShow(false);

	pt = mScreenAssets->GetEntity("Drop");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'Drop'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	
	IND_Surface* spritesurf (pt->GetSurface());
	if(spritesurf)
	{
		spritesurf->SetGrid(16,16);
	}

	pt = mScreenAssets->GetEntity("Menu_Start");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'Menu_Start'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt->SetShow(false);
	spritesurf  = pt->GetSurface();
	if(spritesurf)
	{
		spritesurf->SetGrid(8,8);
	}

	pt = mScreenAssets->GetEntity("Menu_Exit");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'Menu_Exit'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt->SetShow(false);

	spritesurf  = pt->GetSurface();
	if(spritesurf)
	{
		spritesurf->SetGrid(8,8);
	}

	pt = mScreenAssets->GetEntity("Menu_Credits");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'Menu_Credits'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt->SetShow(false);
	
	spritesurf  = pt->GetSurface();
	if(spritesurf)
	{
		spritesurf->SetGrid(8,8);
	}
	
	pt = mScreenAssets->GetEntity("Menu_Controls");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'Menu_Credits'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt->SetShow(false);

	spritesurf  = pt->GetSurface();
	if(spritesurf)
	{
		spritesurf->SetGrid(8,8);
	}

	pt = mScreenAssets->GetEntity("Menu_Title");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'Menu_Title'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt->SetShow(false);
	spritesurf  = pt->GetSurface();
	if(spritesurf)
	{
		spritesurf->SetGrid(8,8);
	}

	pt = mScreenAssets->GetEntity("MouseCursor");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'MouseCursor'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
}
//Internal release
void MainMenuScreen::_release()
{
	//Dynamically allocated memory...	
	//Release from memory and graphics system is done by base class
}

void MainMenuScreen::_processInputs()
{
	IndieLibManager* pIlib = SingletonIndieLib::Instance();
	//--------Menu Selections---
	if(pIlib->Input->IsMouseButtonPressed(IND_MBUTTON_LEFT)
	   &&
	   mSecondTransition)
	{
		SpritePointer mouse(mScreenAssets->GetEntity("MouseCursor"));
		//Selection of game start
		SpritePointer startsel(mScreenAssets->GetEntity("Menu_Start"));
		if(pIlib->Entity2dManager->IsCollision(mouse.get(),"*",startsel.get(),"*"))
		{
			mStart = true;
			mCredits = false;
			mControls = false;
		}	
		//Selection of ccontrols
		SpritePointer controlssel(mScreenAssets->GetEntity("Menu_Controls"));
		if(pIlib->Entity2dManager->IsCollision(mouse.get(),"*",controlssel.get(),"*"))
		{
			mControls = true;
			mCredits = false;
			mStart = false;
		}	

		//Selection of credits
		SpritePointer creditssel(mScreenAssets->GetEntity("Menu_Credits"));
		if(pIlib->Entity2dManager->IsCollision(mouse.get(),"*",creditssel.get(),"*"))
		{
			mCredits = true;
			mStart = false;
			mControls = false;
		}	
		//Selection of game exit
		SpritePointer exitsel(mScreenAssets->GetEntity("Menu_Exit"));
		if(pIlib->Entity2dManager->IsCollision(mouse.get(),"*",exitsel.get(),"*"))
		{
			mExit = true;
			mStart = false;
			mCredits = false;
			mControls = false;
		}	
	}

	//--------Exit-----------
	static bool exittemp = false;
	if(pIlib->Input->OnKeyPress(IND_ESCAPE))
	{
		mExit = true;
		mStart = false;
		mCredits = false;
		mControls = false;
	}
}