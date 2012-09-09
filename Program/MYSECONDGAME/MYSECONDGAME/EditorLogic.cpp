/*
	Filename: EditorLogic.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Editor for my game
	Comments: Quite "not user friendly" but it is for internal use and speed up development, also for learning
	Attribution: Based on Javier Lopez Lopez and his great work in gametuto.com (tutorial of C++ editor)
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "EditorLogic.h"
#include "EditorLogicListener.h"
#include "Camera2D.h"
#include "EditorMouse.h"
#include "EditorKeyboard.h"
#include "IndieLibManager.h"
#include "PhysicsManager.h"
#include "LevelBuilder.h"
#include <windows.h>   //Interface dialogs to save/load

//Global config options declaration
extern ConfigOptions g_ConfigOptions;  //Global properties of game

//Definition of entities file to editor
const std::string EditorLogic::mEditorEntitiesFileName = "EditorAssets.xml";
const std::string EditorLogic::mEmptyLevelFileName = "EmptyLevel.xml";

int EditorLogic::mEditedSprites = 0;
int EditorLogic::mEditedBodies = 0;

//Update state of editor logic
void EditorLogic::UpdateState(float dt)
{
	//Formatted output variable
	std::stringstream messagestream;
	//Indielib Manager pointer
	IndieLibManager* Ilib = SingletonIndieLib::Instance();

	//Update input 
	mEditorMouse->Update(dt);
	mEditorKeyBoard->Update(dt);

	//IF - Editing
	if(mEditedLevel)
	{	
		//IF - No tile manipulation and editing tiles
		if(!mMoveTile && !mMoveTileExact && !mRotateCW && !mScaleTileUp && !mScaleTileDown
		   &&
		   (mEditingBrush.mInsertionType == 0
		   ||
		   mEditingBrush.mInsertionType == 1 )
		   )
		{
			//Reset body collisions
			mCurrentCollided.body = NULL;
			//Update collision with other sprites in layer
			SpritePointer newcollided = _checkCollisionswithBrush(mCurrentCollided.name);
			//IF - No Collision
			if(!newcollided)
			{
				//IF - Collision in previous step
				if(mCurrentCollided.sprite)
				{
					//Dont show collision in previous
					mCurrentCollided.sprite->ShowGridAreas(false);		
					//Update tracking of collided entity
					mCurrentCollided.sprite = newcollided;
					//Show brush
					mEditingBrush.currentsprite->SetShow(true);
				}//IF
			}//ELSE IF - New collision
			else if(newcollided != mCurrentCollided.sprite)
			{
				//IF - Collision in previous step
				if(mCurrentCollided.sprite)
				{
					//Dont show collision in previous
					mCurrentCollided.sprite->ShowGridAreas(false);		
				}//IF
				//Update tracking of collided entity
				mCurrentCollided.sprite = newcollided; 
				//Show collision in new entity
				mCurrentCollided.sprite->ShowGridAreas(true);
				//Hide brush
				mEditingBrush.currentsprite->SetShow(false);
			}//IF
		}//ELSE - Physics bodies manipulation
		else if (!mMoveTile && !mMoveTileExact && !mRotateCW
			     &&
				 mEditingBrush.mInsertionType == 2)
		{
			//Reset sprite collisions
			mCurrentCollided.sprite = SpritePointer();
			//Position to check body to:
			Vector2 positionpix(mEditingBrush.currentsprite->GetPosX(),mEditingBrush.currentsprite->GetPosY());
			Vector2 position = SingletonIndieLib::Instance()->FromPixToCoord(positionpix);
			b2Vec2 poscoords(static_cast<float>(position.x),static_cast<float>(position.y));
			//Query collision with body in current position
			mCurrentCollided.body = mPhysicsMgr->QueryforBodies(poscoords,true); //Including static bodies
			if(mCurrentCollided.body)
			{
				mCurrentCollided.name = mPhysicsMgr->GetBodyName(mCurrentCollided.body);
			}	
		}//IF

		//*****************Update Info Text Beginning********************
		messagestream << "EDITOR MODE: Editing Layer" << mCurrentEditingLayer;
		if(mEditingBrush.mInsertionType == 0)  //IF - SURFACES
		{
			messagestream <<"\nEDITING AND INSERTING: SURFACES";
			messagestream <<"\nPRESS: 1 - Animations 2 - Physical bodies";
		}
		else if(mEditingBrush.mInsertionType == 1) //IF - ANIMATIONS
		{
			messagestream <<"\nEDITING AND INSERTING: ANIMATIONS";
			messagestream <<"\nPRESS: 0 - Surfaces 2 - Physical bodies";
		}
		else if(mEditingBrush.mInsertionType == 2) //IF - BODIES
		{
			messagestream <<"\nEDITING AND INSERTING: PHYSICAL BODIES";
			if(mEditingBrush.addtobody)
			{
				messagestream <<"\nADDING SHAPES TO BODY '"<<mEditingBrush.currentbody<<"'";
				messagestream <<"\nCHANGE BASE BODY WITH 'CTRL + LEFT MOUSE CLICK";
			}
			else
				messagestream << "\nADDING SHAPES WITH NEW BODY";
			messagestream << "\n'TAB' TOGGLE INSERTION / ADDING OF SHAPE";
			if(mEditingBrush.shapetype == 0)
				messagestream <<"\nCurrent Shape: Rectangles";
			else if(mEditingBrush.shapetype == 1)
				messagestream <<"\nCurrent Shape: Circles";
			messagestream <<"\nPRESS: 0 - Surfaces 1 - Animations";
		}//IF

		//IF - Show full help text
		if(mFullInfoText)
		{
			//IF - Surfaces or animations
			if(mEditingBrush.mInsertionType == 0
			   ||
			   mEditingBrush.mInsertionType == 1)
			{
				messagestream << "\nPRESS SPACE TO HIDE ALL COMMANDS INFO"; 
				messagestream << "\n'CTRL + S' to save the level";
				messagestream << "\n'CTRL + N' to SAVE and start new empty level";
				messagestream << "\n'+' or '-' to change editing layer";
				messagestream << "\n'ArrowKeys' to move around the level";
				messagestream << "\n'MouseWheel' Zoom in/out";
				messagestream << "\n'Key Z / X' to change insert tile";
				messagestream << "\n'RETURN' or 'SHIFT' to drop tile";
				messagestream << "\n'CTRL + C' to copy tile";
				messagestream << "\n'CTRL + V' to drop copied tile";
				messagestream << "\n'Hover with mouse' to select tile in current layer";
				messagestream << "\n'I' to toggle info of selected tile";
				messagestream << "\n'Delete' or 'Backspace' to delete selected tile";
				messagestream << "\n'Left-Click and drag' to move selected tile";
				messagestream << "\n'CTRL + Arrow Keys' to move tile 1 pixel in direction";
				messagestream << "\n'W' and 'S' to change Z-Order in layer of selected tile";
				messagestream << "\n'R' to rotate selected tile";
				messagestream << "\n'Q' and 'E' to scale selected tile";
				messagestream << "\n'N' or 'M' when scaling to toggle only X or Y axis";
				messagestream << "\n'A' and 'D' to change transparency of selected tile";
				messagestream << "\n'F' and 'G' to flip tile horizontally and vertically";
				messagestream << "\n'O' and 'P' reset scale and rotation of selected sprite to 0";
				messagestream << "\n'CTRL + T' to toggle tiling in sprite";
				messagestream << "\n(If Sprite is tiled) 'H' or 'J' to change wrap zone'";
				messagestream << "\n(IF Sprite is tiled) 'N' or 'M' while wrapping to set X or Y axis only";

			}//ELSE - BODIES
			else if(mEditingBrush.mInsertionType == 2)
			{
				messagestream << "\nPRESS SPACE TO HIDE ALL COMMANDS INFO"; 
				messagestream << "\n'CTRL + S' to save the level";
				messagestream << "\n'CTRL + N' to SAVE and start new empty level";
				messagestream << "\n'+' or '-' to change editing layer";
				messagestream << "\n'ArrowKeys' to move around the level";
				messagestream << "\n'MouseWheel' Zoom in/out";
				messagestream << "\n'Key Z' to change insertion shape type";
				messagestream << "\n'Hover with mouse' to select body (in any layer)";
				messagestream << "\n'Left-Click and drag' to move selected body";
				messagestream << "\n'CTRL + Arrow Keys' to move body 1 unit in direction";
				messagestream << "\n'Right-Click and drag' to define a new shape for a body";
				messagestream << "\n'Delete' or 'Backspace' to delete selected tile";
				messagestream << "\n'R' to rotate selected body";
				messagestream << "\n'P' to reset rotation of selected body to 0";
			}//IF
		}
		else//ELSE - No all commands
		{
			messagestream << "\nPRESS SPACE TO SHOW ALL COMMANDS INFO";
		}//IF

		//---Showing info from tile----------------------------
		if( mEditingBrush.mInsertionType == 0 
			||
			mEditingBrush.mInsertionType == 1)
		{
			mCurrentCollided.infotextsprite->SetShow(mShowSpInfoText && mCurrentCollided.sprite);
		}
		else
			mCurrentCollided.infotextsprite->SetShow(mCurrentCollided.body != NULL);

		//*************Process commands************************
		//--------------Layer editing change-------------------
		//IF - Command tracking is activated
		if(mChangeLayer)
		{
			//Check if this layer is edited in level properties (parallax) or is
			//one free entities rendering layer from 10 - 62
			GameLevel::ParallaxListIterator pxitr;
			bool camerafound = false; 
			//LOOP - Search for created parallax layer
			for(pxitr = mEditedLevel->mParallaxList.begin();pxitr != mEditedLevel->mParallaxList.end();pxitr++)
			{
				//IF - Found level in 
				if((*pxitr).thecamera->GetLayer() == mCurrentEditingLayer)
				{
					//Save camera
					mCurrentCamera = (*pxitr).thecamera;
					camerafound = true;
					std::string thecameraname = mCurrentCamera->GetName();
					break;
				}//IF
			}//LOOP END

			//IF - Camera was not found
			if(!camerafound)
			{
				//Just assign current camera to the general one
				mCurrentCamera = Ilib->GetCamera("General");
			}//IF
			
			//Command processed
			mChangeLayer = false;
		}//IF

		//---------------Movement------------------------------
		if(mMovement)
		{
			//Get master camera to move all layers 
			Camera2DPointer generalcam = SingletonIndieLib::Instance()->GetCamera("General"); //Master gamera to move around
			generalcam->Translate(mMovingSpeed); //Apply translate to camera
			mMovement = false;
		}
		//---------------Zoom----------------------------------
		if(mZoomIn)
		{
			//Get master camera to move all layers 
			Camera2DPointer generalcam = SingletonIndieLib::Instance()->GetCamera("General"); //Master gamera to move around
			generalcam->Zoom(true);  //Just apply positive zoom
			mZoomIn = false;
			mZoomOut = false;
		}
		else if(mZoomOut)
		{
			//Get master camera to move all layers 
			Camera2DPointer generalcam = SingletonIndieLib::Instance()->GetCamera("General"); //Master gamera to move around
			generalcam->Zoom(false);//Just apply negative zoom
			mZoomOut = false;
			mZoomIn = false;
		}

		//----------------Insertion type changing--------------
		if(mChangeInsertionType)
		{
			//IF - Surface or animation
			if(mEditingBrush.mInsertionType == 0
			   ||
			   mEditingBrush.mInsertionType == 1)
			{
				//Show brush
				mEditingBrush.currentsprite->SetShow(true);
				//Restart attributes
				_putBrushAttributes();
				//Reset commands
				_resetCommands();
			}//ELSE - Body
			else if(mEditingBrush.mInsertionType == 2)
			{
				//Dont show brush
				mEditingBrush.currentsprite->SetShow(false);
				//Reset commands
				_resetCommands();
			}
			mChangeInsertionType = false;
		}
		//----------------Used tile changing-------------------	
		if(mIncreaseTileSurf)
		{	
			//IF - SURFACE
			if(mEditingBrush.mInsertionType == 0)
			{
				//Increment iterator
				mCurrentBackdropSurface++;
				//IF - End of available surfaces
				if(mCurrentBackdropSurface == mAvailableResources->surfacesMap.end())
				{
					mCurrentBackdropSurface = mAvailableResources->surfacesMap.begin(); //Assign beginning of list again
				}//IF
				
				_putBrushAttributes(); //Restart attributes (Indielib resets them)
			}//ELSE - ANIMATION
			else if(mEditingBrush.mInsertionType == 1)
			{
				//Increment iterator
				mCurrentBackdropAnimation++;
				//IF - End of available animations
				if(mCurrentBackdropAnimation == mAvailableResources->animationsMap.end())
				{
					mCurrentBackdropAnimation = mAvailableResources->animationsMap.begin(); //Assign beginning of list again
				}//IF
				
				_putBrushAttributes(); //Restart attributes (Indielib resets them)
			}
			mIncreaseTileSurf = false;
		}
		else if(mDecreaseTileSurf)
		{
			//IF - SURFACE
			if(mEditingBrush.mInsertionType == 0)
			{
				//IF - Beginning of available surfaces
				if(mCurrentBackdropSurface == mAvailableResources->surfacesMap.begin())
				{
					mCurrentBackdropSurface = mAvailableResources->surfacesMap.end(); //Assign end of list (not decrement!!)
				}//ELSE - Not beginning of list
				{
					mCurrentBackdropSurface--; //Decrease iterator
				}
				
				_putBrushAttributes(); //Restart attributes (Indielib resets them)
			}//ELSE - ANIMATION
			else if(mEditingBrush.mInsertionType == 1)
			{
				//IF - Beginning of available surfaces
				if(mCurrentBackdropAnimation == mAvailableResources->animationsMap.begin())
				{
					mCurrentBackdropAnimation = mAvailableResources->animationsMap.end(); //Assign end of list (not decrement!!)
				}//ELSE - Not beginning of list
				{
					mCurrentBackdropAnimation--; //Decrease iterator
				}
				
				_putBrushAttributes(); //Restart attributes (Indielib resets them)
			}
			mDecreaseTileSurf = false;
		}

		//---------------Tile Dropping-------------------------
		if(mInsertTile)
		{
			//IF - No tile is being modified
			if(!mMoveTile && !mIncreaseZ && !mDecreaseZ && !mRotateCW && !mScaleTileUp && !mScaleTileDown &&!mWrapTileUp && !mWrapTileDown)
			{
				//IF - Surface
				if(mEditingBrush.mInsertionType == 0)
				{
					//Create new sprite with default params from surface (user can change it after inserting
					SpritePointer newsprite(new IND_Entity2d());
					SurfacePointer surfptr = SingletonResourceMgr::Instance()->GetSurfaceResource(mEditingBrush.resourceId);
					newsprite->SetSurface(surfptr.get());
					newsprite->SetHotSpot(0.5f,0.5f);
					newsprite->SetPosition(mEditingBrush.currentsprite->GetPosX(),
										   mEditingBrush.currentsprite->GetPosY(),
										   0);
					newsprite->SetBoundingRectangle ("FullSurface", 0, 0, surfptr->GetWidth(), surfptr->GetHeight());
					newsprite->ShowCollisionAreas(false);
					newsprite->ShowGridAreas(false);
					//Generate a valid new name for entity
					bool notgood = true;
					//LOOP - Generate names till one is valid
					do
					{	//Generate a name
						std::stringstream namestream;
						namestream<<"Image"<<mEditedSprites;
						//Check if it is valid
						GameLevel::EntitiesMapIterator itr = mEditedLevel->mEntitiesMap.find(namestream.str());
						if(itr != mEditedLevel->mEntitiesMap.end())
							mEditedSprites++;
						else
						{
							mEditingBrush.spriteId = namestream.str();
							notgood = false;
						}
					}while(notgood); //LOOP END
					
					//Add it to level and IndieLib
					mEditedLevel->mEntitiesMap[mEditingBrush.spriteId] = newsprite;
					Ilib->Entity2dManager->Add(mCurrentEditingLayer,newsprite.get());
				}//ELSE - Animation
				else if(mEditingBrush.mInsertionType == 1)
				{
					//Create new sprite with default params from animation (user can change it after inserting
					SpritePointer newsprite(new IND_Entity2d());
					AnimationPointer animptr = SingletonResourceMgr::Instance()->GetAnimationResource(mEditingBrush.resourceId);
					newsprite->SetAnimation(animptr.get());
					newsprite->SetHotSpot(0.5f,0.5f);
					newsprite->SetPosition(mEditingBrush.currentsprite->GetPosX(),
										   mEditingBrush.currentsprite->GetPosY(),
										   0);
					//Note: Bounding areas per animation!
					newsprite->ShowCollisionAreas(false);
					newsprite->ShowGridAreas(false);
					//Generate a valid new name for entity
					bool notgood = true;
					//LOOP - Generate names till one is valid
					do
					{	//Generate a name
						std::stringstream namestream;
						namestream<<"Animation"<<mEditedSprites;
						//Check if it is valid
						GameLevel::EntitiesMapIterator itr = mEditedLevel->mEntitiesMap.find(namestream.str());
						if(itr != mEditedLevel->mEntitiesMap.end())
							mEditedSprites++;
						else
						{
							mEditingBrush.spriteId = namestream.str();
							notgood = false;
						}
					}while(notgood); //LOOP END
					
					//Add it to level and IndieLib
					mEditedLevel->mEntitiesMap[mEditingBrush.spriteId] = newsprite;
					Ilib->Entity2dManager->Add(mCurrentEditingLayer,newsprite.get());
				}//IF

			}//IF
			mInsertTile = false;
		}
		//---------------Tile Deleting-------------------------
		else if(mDeleteTile)
		{
			//IF - No tile is being modified and there is one selected
			if(!mMoveTile && !mIncreaseZ && !mDecreaseZ && !mRotateCW && !mScaleTileUp && !mScaleTileDown &&!mWrapTileUp && !mWrapTileDown
				&& mCurrentCollided.sprite)
			{
				//Deregister from IndieLib
				Ilib->Entity2dManager->Delete(mCurrentCollided.sprite.get());
				GameLevel::EntitiesMapIterator itr;
				//LOOP - Search for entity
				for(itr = mEditedLevel->mEntitiesMap.begin();itr != mEditedLevel->mEntitiesMap.end();++itr)
				{
				
					if((*itr).second ==  mCurrentCollided.sprite)
					{
						//Delete from level
						mEditedLevel->mEntitiesMap.erase(itr);
						break;
					}
				}//LOOP END
				
				mDeleteTile = false;
			}
		}
		//---------------Tile Cloning / Dropping of cloned-----
		if(mCloneTile)
		{
			//IF - There is some selection
			if(mCurrentCollided.sprite)
			{
				//Clone sprite into cloning brush
				_cloneSprite(mCurrentCollided);
			}//IF
			mCloneTile = false;
		}
		else if(mDropCloned && mCloningBrush.currentsprite)
		{
			//IF - Surface
			if(mCloningBrush.currentsprite->GetSurface() !=  NULL)
			{			
				//Create new sprite with params from cloning brush
				SpritePointer newsprite(new IND_Entity2d());
				//Copy surface
				SurfacePointer surfptr = SingletonResourceMgr::Instance()->GetSurfaceResource(mCloningBrush.resourceId);
				newsprite->SetSurface(surfptr.get());
				//Copy position in Z and assign current cursor position in X and Y
				newsprite->SetPosition(mEditingBrush.currentsprite->GetPosX(),
									   mEditingBrush.currentsprite->GetPosY(),
									   mCloningBrush.currentsprite->GetPosZ());
				//Copy scaling
				newsprite->SetScale(mCloningBrush.currentsprite->GetScaleX(),mCloningBrush.currentsprite->GetScaleY());
				//Copy rotation
				newsprite->SetAngleXYZ(mCloningBrush.currentsprite->GetAngleX(),
									   mCloningBrush.currentsprite->GetAngleY(),
									   mCloningBrush.currentsprite->GetAngleZ());
				//Copy transparency
				newsprite->SetTransparency(mCloningBrush.currentsprite->GetTransparency());
				//Copy mirroring X and Y
				newsprite->SetMirrorX(mCloningBrush.currentsprite->GetMirrorX());
				newsprite->SetMirrorY(mCloningBrush.currentsprite->GetMirrorY());
				//Set other parameters
				newsprite->SetHotSpot(0.5f,0.5f);
				newsprite->SetBoundingRectangle ("FullSurface", 0, 0, surfptr->GetWidth(), surfptr->GetHeight());
				newsprite->ShowCollisionAreas(false);
				newsprite->ShowGridAreas(false);
				
				//Add it to level and IndieLib
				mEditedLevel->mEntitiesMap[mCloningBrush.spriteId] = newsprite;
				Ilib->Entity2dManager->Add(mCurrentEditingLayer,newsprite.get());

				//Change name to be able to add it again without conflicts
				mEditedSprites = 0;
				bool notgood = true;
				//LOOP - Generate names till one is valid
				do
				{	//Generate a name
					std::stringstream name;
					name<<"ImageCopy"<<mEditedSprites;
					//Check if it is valid
					GameLevel::EntitiesMapIterator itr = mEditedLevel->mEntitiesMap.find(name.str());
					if(itr != mEditedLevel->mEntitiesMap.end())
						mEditedSprites++;
					else
					{
						mCloningBrush.spriteId = name.str();
						notgood = false;
					}
				}while(notgood); //LOOP END

				
			}//ELSE - Animation
			else if(mCloningBrush.currentsprite->GetAnimation() != NULL)
			{
				//Create new sprite with params from cloning brush
				SpritePointer newsprite(new IND_Entity2d());
				//Copy animation
				AnimationPointer animptr = SingletonResourceMgr::Instance()->GetAnimationResource(mCloningBrush.resourceId);
				newsprite->SetAnimation(animptr.get());
				//Copy position in Z and assign current cursor position in X and Y
				newsprite->SetPosition(mEditingBrush.currentsprite->GetPosX(),
									   mEditingBrush.currentsprite->GetPosY(),
									   mCloningBrush.currentsprite->GetPosZ());
				//Copy scaling
				newsprite->SetScale(mCloningBrush.currentsprite->GetScaleX(),mCloningBrush.currentsprite->GetScaleY());
				//Copy rotation
				newsprite->SetAngleXYZ(mCloningBrush.currentsprite->GetAngleX(),
									   mCloningBrush.currentsprite->GetAngleY(),
									   mCloningBrush.currentsprite->GetAngleZ());
				//Copy transparency
				newsprite->SetTransparency(mCloningBrush.currentsprite->GetTransparency());
				//Copy mirroring X and Y
				newsprite->SetMirrorX(mCloningBrush.currentsprite->GetMirrorX());
				newsprite->SetMirrorY(mCloningBrush.currentsprite->GetMirrorY());
				//Set other parameters
				newsprite->SetHotSpot(0.5f,0.5f);
				newsprite->ShowCollisionAreas(false);
				newsprite->ShowGridAreas(false);
				
				//Add it to level and IndieLib
				mEditedLevel->mEntitiesMap[mCloningBrush.spriteId] = newsprite;
				Ilib->Entity2dManager->Add(mCurrentEditingLayer,newsprite.get());

				//Change name to be able to add it again without conflicts
				mEditedSprites = 0;
				bool notgood = true;
				//LOOP - Generate names till one is valid
				do{	//Generate a name
					std::stringstream name;
					name<<"AnimationCopy"<<mEditedSprites;
					//Check if it is valid
					GameLevel::EntitiesMapIterator itr = mEditedLevel->mEntitiesMap.find(name.str());
					if(itr != mEditedLevel->mEntitiesMap.end())
						mEditedSprites++;
					else
					{
						mCloningBrush.spriteId = name.str();
						notgood = false;
					}
				}while(notgood); //LOOP END
			}//IF
			mDropCloned = false;
		}
		//---Tile manipulations: ------------------------------
		//---Moving, rotation, scaling, transparency, mirrorXY-
		if(mCurrentCollided.sprite)
		{
		
			//----------Movement-------------------------------
			if(!mRotateCW && !mScaleTileUp && !mScaleTileDown &&!mWrapTileUp && !mWrapTileDown)
			{
				//In X/Y
				if(mMoveTile)
				{
					//Follow position of brush entity
					int posz = mCurrentCollided.sprite->GetPosZ();

					mCurrentCollided.sprite->SetPosition(mInitialPosX + mEditingBrush.currentsprite->GetPosX(),
												  mInitialPosY + mEditingBrush.currentsprite->GetPosY(),
												  posz);
				}//In X/Y 1 by 1
				else if(mMoveTileExact)
				{
					float posx =  mCurrentCollided.sprite->GetPosX() + static_cast<int>(mMoveExactDirection.x);
					float posy =  mCurrentCollided.sprite->GetPosY() - static_cast<int>(mMoveExactDirection.y);
					int posz = mCurrentCollided.sprite->GetPosZ();

					mCurrentCollided.sprite->SetPosition(posx,
												  posy,
												  posz);
					mMoveTileExact = false;
				}//In Z
				else if(mIncreaseZ)
				{
					float posx =  mCurrentCollided.sprite->GetPosX();
					float posy =  mCurrentCollided.sprite->GetPosY();
					int posz = mCurrentCollided.sprite->GetPosZ() + 1;

					mCurrentCollided.sprite->SetPosition(posx,
												  posy,
												  posz);
					mIncreaseZ = false;

				}
				else if(mDecreaseZ)
				{
					float posx =  mCurrentCollided.sprite->GetPosX();
					float posy =  mCurrentCollided.sprite->GetPosY();
					int posz = mCurrentCollided.sprite->GetPosZ() - 1;

					mCurrentCollided.sprite->SetPosition(posx,
												  posy,
												  posz);
					mDecreaseZ = false;
				}

			}

			//----------Rotation-------------------------------
			float rotspeed = 0.1f;	
			if(!mMoveTile && !mScaleTileUp && !mScaleTileDown &&!mWrapTileUp && !mWrapTileDown)
			{
				if(mResetRotation)
				{
					mCurrentCollided.sprite->SetAngleXYZ(0,0,0);
					mResetRotation = false;
				}
				else if(mRotateCW)
				{
					float angle = mCurrentCollided.sprite->GetAngleZ();
					angle += rotspeed * dt;
					mCurrentCollided.sprite->SetAngleXYZ(0,0,angle);
				}
			}//IF
			//--------Scaling----------------------------------
			float scalespeed = 0.001f;
			if(!mMoveTile && !mRotateCW &&!mWrapTileUp && !mWrapTileDown)
			{
				if(mResetScale)
				{
					mCurrentCollided.sprite->SetScale(1,1);
					mResetScale = false;
				}
				else if(mScaleTileUp)
				{
					
					float scalex = mCurrentCollided.sprite->GetScaleX();
					float scaley = mCurrentCollided.sprite->GetScaleY();
					if(mScaleTileX)
						scalex += scalespeed * dt;
					if(mScaleTileY)
						scaley += scalespeed * dt;
					mCurrentCollided.sprite->SetScale(scalex,scaley);
				}
				else if(mScaleTileDown)
				{
					float scalex = mCurrentCollided.sprite->GetScaleX();
					float scaley = mCurrentCollided.sprite->GetScaleY();
					if(mScaleTileX)
						scalex -= scalespeed * dt;
					if(mScaleTileY)
						scaley -= scalespeed * dt;
					mCurrentCollided.sprite->SetScale(scalex,scaley);
				}
			}
			
			//---------Tile wrapping-------------------------------
			float tilewrapspeed = 0.5f;
			if(!mMoveTile && !mRotateCW && !mScaleTileUp && !mScaleTileDown)
			{
				if(mToggleWrap)
				{
					bool wrap(mCurrentCollided.sprite->IfWrap());
					wrap = !wrap;
					mCurrentCollided.sprite->ToggleWrap(wrap);

					if(wrap)
					{
						IND_Surface* surf (mCurrentCollided.sprite->GetSurface());
						IND_Animation* anim (mCurrentCollided.sprite->GetAnimation());
						
						if(surf)
							mCurrentCollided.sprite->SetRegion(0,0,surf->GetWidth(),surf->GetHeight());
						
						if(anim)
							mCurrentCollided.sprite->SetRegion(0,0,anim->GetHighWidth(0),anim->GetHighHeight(0));

					}

				}
				
				if(mCurrentCollided.sprite->IfWrap())
				{
					if(mWrapTileUp)
					{
						int width = mCurrentCollided.sprite->GetRegionWidth();
						int height = mCurrentCollided.sprite->GetRegionHeight();
						if(mWrapTileX)
							width += static_cast<int>(tilewrapspeed * dt);
						if(mWrapTileY)
							height += static_cast<int>(tilewrapspeed * dt);

						mCurrentCollided.sprite->SetRegion(0,0,width,height);
					}
					else if(mWrapTileDown)
					{
						int width = mCurrentCollided.sprite->GetRegionWidth();
						int height = mCurrentCollided.sprite->GetRegionHeight();
						if(mWrapTileX)
							width -= static_cast<int>(tilewrapspeed * dt);
						if(mWrapTileY)
							height -= static_cast<int>(tilewrapspeed * dt);

						mCurrentCollided.sprite->SetRegion(0,0,width,height);
					}
				}	

				mToggleWrap = false;
			}

			if(!mMoveTile && !mScaleTileUp && !mScaleTileDown && !mRotateCW &&!mWrapTileUp && !mWrapTileDown)
			{
				//---------Horizontal and vertical flipping---------
				if(mVertFlip)
				{
					(mCurrentCollided.sprite->GetMirrorX() == true) ? mCurrentCollided.sprite->SetMirrorX (false) : mCurrentCollided.sprite->SetMirrorX (true);
					mVertFlip = false;
				}
				else if(mHorFlip)
				{
					(mCurrentCollided.sprite->GetMirrorY() == true) ? mCurrentCollided.sprite->SetMirrorY (false) : mCurrentCollided.sprite->SetMirrorY (true);
					mHorFlip = false;
				}

				//-------------Transparency-------------------------
				if(mIncreaseTransp)
				{
					byte transp = mCurrentCollided.sprite->GetTransparency();
					if (transp < 255) 
						mCurrentCollided.sprite->SetTransparency (transp + 1);
					
					mIncreaseTransp =  false;
				}
				else if (mDecreaseTransp)
				{
					byte transp = mCurrentCollided.sprite->GetTransparency();
					if (transp > 0) 
						mCurrentCollided.sprite->SetTransparency (transp - 1);

					mDecreaseTransp = false;
				}
			}
		}

		//-------------BODIES: Add-to-body / New shape with new body---
		if(mToggleBodiesEditMode && !mDefineBody && !mDefiningBody)
		{
			mEditingBrush.addtobody = !mEditingBrush.addtobody;
			mToggleBodiesEditMode = false;
		}
 
		//-------------BODIES: Shape type change-----------------------
		if(mChangeInsertShape && mEditingBrush.mInsertionType == 2)
		{
			//IF - Last shape type index
			if(mEditingBrush.shapetype == 1)
				mEditingBrush.shapetype = 0;
			else //ELSE - Middle shape type index
				mEditingBrush.shapetype++;

			mChangeInsertShape = false;
		}

		//-------------BODIES: Add to body , body change---------------
		if(mChangeAddingBody)
		{
			if(mEditingBrush.addtobody && !mDefineBody && !mDefiningBody && mCurrentCollided.body )
			{
				mEditingBrush.currentbody = mPhysicsMgr->GetBodyName(mCurrentCollided.body);
			}
			mChangeAddingBody = false;
		}
		//-------------BODIES: Body definition (dropping)--------------
		if(mDefineBody && mEditingBrush.mInsertionType == 2)
		{
			//Be sure a body is big enough...
			Vector2 dims = mEditingBrush.startdefpoint - mEditingBrush.enddefpoint;
			if(!dims.IsEqualWithToleranceTo(Vector2(0,0),0.3f)
				&&
				(
				(mEditingBrush.addtobody && mEditingBrush.currentbody != "")
				||
				!mEditingBrush.addtobody)
			  )
			{
				//IF - New body
				if(!mEditingBrush.addtobody)
				{
					//Generate name for body
					bool notgood(true);
					//LOOP - Generate names till one is valid
					do
					{	//Generate a name
						std::stringstream namestream;
						namestream<<"StaticBody"<<mEditedBodies;
						BodiesListIterator boditr;
						//LOOP - Search bodies list
						for(boditr = mBodiesList.begin(); boditr != mBodiesList.end(); boditr++)
						{
							if((*boditr) == namestream.str())
							{
								break;
							}
						}//LOOP END

						//IF - Name already exists
						if(boditr != mBodiesList.end())
						{
							mEditedBodies ++;
						}
						else
						{
							mEditingBrush.spriteId= namestream.str();
							notgood = false;
						}
					}while(notgood); //LOOP END

					//Create a new body
					b2BodyDef bodydef;
					bodydef.allowSleep = true;
					bodydef.angle = 0.0f;
					bodydef.angularDamping = 0.1f;
					bodydef.linearDamping = 0.0f;
					//Position of body (center)
					//IF - Insertion is rectangle
					if(mEditingBrush.shapetype == 0)
					{
						//Position is midpoint of definition
						Vector2 position =  mEditingBrush.startdefpoint + ((mEditingBrush.enddefpoint - mEditingBrush.startdefpoint)/ 2);
						bodydef.position = b2Vec2(static_cast<float>(position.x),
												  static_cast<float>(position.y));
					}//ELSE IF - Insertion is circle
					else if(mEditingBrush.shapetype == 1)
					{
						//Position is startpoint
						bodydef.position = b2Vec2(static_cast<float>(mEditingBrush.startdefpoint.x),
												  static_cast<float>(mEditingBrush.startdefpoint.y));
					}//IF
					mPhysicsMgr->CreateBody(&bodydef,mEditingBrush.spriteId); //Create the body
				}//ELSE - Adding to body
				else
				{
					mEditingBrush.spriteId = mEditingBrush.currentbody;
				}//IF

				//Add the shape to the body
				//IF - Insertion is rectangle
				if(mEditingBrush.shapetype == 0)
				{
					b2PolygonDef rectshape;
					rectshape.density = 1000.0f;   //Create static bodies allways with density 1000
					rectshape.friction = 1.0f; //Default friction
					rectshape.restitution = 0.0f; //Default restitution
					//Compute box dimensions and position
					if(!mEditingBrush.addtobody)
					{
						//All body (position is body origin) (0,0)
						rectshape.SetAsBox(abs(static_cast<float>(mEditingBrush.enddefpoint.x - mEditingBrush.startdefpoint.x)/2),
									   abs(static_cast<float>(mEditingBrush.enddefpoint.y - mEditingBrush.startdefpoint.y)/2)
									   );
					}
					else
					{
						//Compute additionally the position respective to body coords
						b2Body* thebody = mPhysicsMgr->GetBody(mEditingBrush.spriteId);  //Body pointer (adding)
						Vector2 center = mEditingBrush.startdefpoint + ((mEditingBrush.enddefpoint - mEditingBrush.startdefpoint)/ 2); 
						rectshape.SetAsBox(abs(static_cast<float>(mEditingBrush.enddefpoint.x - mEditingBrush.startdefpoint.x)/2),
									   abs(static_cast<float>(mEditingBrush.enddefpoint.y - mEditingBrush.startdefpoint.y)/2),
									  thebody->GetLocalPoint( b2Vec2(static_cast<float>(center.x),static_cast<float>(center.y))),//Local coords of world center point
									   0  //Rotation angle
									   );
					}
					mPhysicsMgr->CreatePolygonShape(&rectshape,mEditingBrush.spriteId);
				}//ELSE IF - Insertion is circle
				else if(mEditingBrush.shapetype == 1)
				{
					b2CircleDef circleshape;
					circleshape.density = 1000.0f;   //Create static bodies allways
					circleshape.friction = 1.0f; //Default friction
					circleshape.restitution = 0.0f; //Default restitution
					//Calculate radius
					Vector2 radiusvec = mEditingBrush.enddefpoint - mEditingBrush.startdefpoint;
					circleshape.radius = static_cast<float>(radiusvec.Length());
					//Compute position inside body
					if(mEditingBrush.addtobody)
					{
						b2Body* thebody = mPhysicsMgr->GetBody(mEditingBrush.spriteId);  //Body pointer (adding)
						Vector2 center = mEditingBrush.startdefpoint + ((mEditingBrush.enddefpoint - mEditingBrush.startdefpoint)/ 2); 						
						circleshape.localPosition = thebody->GetLocalPoint(b2Vec2(static_cast<float>(center.x),static_cast<float>(center.y)));//Local coords of world center point
					}
					mPhysicsMgr->CreateCircleShape(&circleshape,mEditingBrush.spriteId);
				}//IF
				//Store body name, finally
				AppendBody(mEditingBrush.spriteId);
				mEditingBrush.currentbody = mEditingBrush.spriteId;
			}//IF
			mDefineBody = false;
		}

		//---Body manipulations: ------------------------------
		//---Moving, rotation, deleting
		if(mCurrentCollided.body)
		{
		
			//----------Movement-------------------------------
			if(!mRotateCW)
			{
				//In X/Y
				if(mMoveTile)
				{
					Vector2 pospix = Vector2(mEditingBrush.currentsprite->GetPosX(),
											mEditingBrush.currentsprite->GetPosY());
					Vector2 pos = SingletonIndieLib::Instance()->FromPixToCoord(pospix);
					pos += mInitialBodyPos;
					float angle = mCurrentCollided.body->GetAngle();
					mCurrentCollided.body->SetXForm(b2Vec2(static_cast<float>(pos.x),
														   static_cast<float>(pos.y))
														   ,angle);
				}//In X/Y 1 by 1
				else if(mMoveTileExact)
				{
					b2Vec2 pos = mCurrentCollided.body->GetPosition();
					pos += b2Vec2(static_cast<float>(mMoveExactDirection.x),static_cast<float>(mMoveExactDirection.y));
					float angle = mCurrentCollided.body->GetAngle();
					mCurrentCollided.body->SetXForm(pos,angle);

					mMoveTileExact = false;
				}
			}

			//----------Rotation-------------------------------
			float rotspeed = 0.001f;	
			if(!mMoveTile)
			{
				if(mResetRotation)
				{
					b2Vec2 pos = mCurrentCollided.body->GetPosition();
					mCurrentCollided.body->SetXForm(pos,0.0f);
					mResetRotation = false;
				}
				else if(mRotateCW)
				{
					float angle = mCurrentCollided.body->GetAngle();
					angle += rotspeed * dt;
					b2Vec2 pos = mCurrentCollided.body->GetPosition();
					mCurrentCollided.body->SetXForm(pos,angle);
				}
			}//IF
			//--------Deleting of bodies-----------------------
			if(mDeleteTile)
			{
				//Find body in list and delete reference to name
				std::string bodyname = mPhysicsMgr->GetBodyName(mCurrentCollided.body);
				mBodiesList.remove(bodyname);
				mPhysicsMgr->DestroyBody(mCurrentCollided.body);
				mCurrentCollided.body = NULL;
				mDeleteTile = false;
			}
		}
		//-------------Update edited tile info-------------------
		_updateSpriteInfo();
		//************Commands processed***********************
		
		//***************Additionals***************************
		//Update editing camera
		mEditLayerCam->SetPosition(mCurrentCamera->GetPosition());
		mEditLayerCam->SetZoom(mCurrentCamera->GetZoom());
		//Update level logic
		mEditedLevel->Update(dt);
		//Update cameras
		Ilib->UpdateCameras(dt);	
	}
	else //ELSE - Not editing
	{
		messagestream << "EDITOR MODE:\nPress 'CTRL + N' to create a new level \n'CTRL + L' to load an existing level";
	}//IF

	//Copy formatted string contents to member string and to member sprite display
	mInfoText.assign(messagestream.str());
	mInfoTextSprite->SetText(const_cast<char*>(mInfoText.c_str())); //UGLY const_cast for indielib...
}

//Render scene
void EditorLogic::RenderScene()
{
	//Call render for input objects
	mEditorMouse->Render();
	mEditorKeyBoard->Render();

	//IF - Editing bodies
	if(mEditingBrush.mInsertionType == 2)
	{
		mDrawPhysicsElements = true;
		SingletonIndieLib::Instance()->GetCamera("General")->SetAsCurrent();
		
		//IF - In process of defining body
		if(mDefiningBody)
		{
			//IF - Shape defined is a rectangle
			if(mEditingBrush.shapetype == 0)
			{
				//Draw current newdefinition of body
				//A little hack to draw a rectangle through box2d debug renderer (reuse of code... ;) )
				b2Vec2 vertices0 (static_cast<float>(mEditingBrush.startdefpoint.x),static_cast<float>(mEditingBrush.enddefpoint.y));
				b2Vec2 vertices1 (static_cast<float>(mEditingBrush.enddefpoint.x),static_cast<float>(mEditingBrush.enddefpoint.y));
				b2Vec2 vertices2 (static_cast<float>(mEditingBrush.enddefpoint.x),static_cast<float>(mEditingBrush.startdefpoint.y));
				b2Vec2 vertices3 (static_cast<float>(mEditingBrush.startdefpoint.x),static_cast<float>(mEditingBrush.startdefpoint.y));
				b2Vec2 vertices[4] = {vertices0,vertices1,vertices2,vertices3};
				SingletonIndieLib::Instance()->Box2DDebugRender->DrawPolygon(vertices,
																			 4,
																			 b2Color(1.0f,1.0f,1.0f));
			}//ELSE - Shape defined is a circle
			else if(mEditingBrush.shapetype == 1)
			{
				//A little hack to draw a circle through box2d debug renderer (reuse of code... ;) )
				b2Vec2 center (static_cast<float>(mEditingBrush.startdefpoint.x),
							   static_cast<float>(mEditingBrush.startdefpoint.y));
				Vector2 radiusvec = mEditingBrush.enddefpoint - mEditingBrush.startdefpoint;
				SingletonIndieLib::Instance()->Box2DDebugRender->DrawCircle(center,static_cast<float>(radiusvec.Length()),b2Color(1.0f,1.0f,1.0f));
			}//IF
		}//IF
	}//IF

	//Draw physics elements if requested (mode or manual drawing)
	if(mDrawPhysicsElements)
	{
		SingletonIndieLib::Instance()->GetCamera("General")->SetAsCurrent();
		mPhysicsMgr->DebugRender();
	}
}

//Creation function helper
void EditorLogic::AppendBody(const std::string& name)
{
	mBodiesList.push_front(name);
}

//Handle functions
//Open - New - Load Levels commands
void EditorLogic::LoadLevelCommand()
{
	//Local variables
	bool processload = true;
	
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		//IF - User saved level
		if(!SaveLevelCommand())
		{
			processload = false;
		}
	}//IF

	//IF - User didnt cancel before
	if(processload)
	{
		//Call internal function to create a windows dialog (option "Open" to true)
		std::string filepath = _fileDialog("XML\0*.xml;", true, "Choose a map file");

		//IF - Some File chosen
		if (filepath != "") 
		{
			//Load the level with builder object
			LevelBuilder thebuilder(this); //Create edit-mode level loading
				
			//TODO: SEARCH NAMES AND CHOOSE CORRECT FILEPATH AND LEVELNAME
			//Call creation of object (level)
			thebuilder.LoadLevel(filepath,"NONAME");
			//Get the created level
			mEditedLevel = thebuilder.GetCreatedLevel();
			//Hack:
			SingletonIndieLib::Instance()->GetCamera("General")->SetMaxMinZoom(1.0f,0.01f);
		}//IF
	}//IF
}
bool EditorLogic::SaveLevelCommand()
{
	//Local variables
	bool success = false;

	//IF - There is a current level editing
	if(mEditedLevel)
	{
		//Call internal function to create a windows dialog (option "Open" to false)
		std::string filepath = _fileDialog("XML\0*.xml;", false, "Choose save location: IT HAS TO BE SCRIPTS FOLDER!!");

		//IF - Some File chosen
		if (filepath != "") 
		{
			//Use builder to save level
			LevelBuilder builder(this);
			builder.SaveLevel(filepath,mEditedLevel);

			mEditedLevel.reset(); //Clear pointer and resources
			//Clear all physics bodies
			BodiesListIterator itr;
			//LOOP - All bodies
			for(itr = mBodiesList.begin(); itr != mBodiesList.end();++itr)
			{
				mPhysicsMgr->DestroyBody((*itr));
			}//LOOP END
			mBodiesList.clear();
			success = true;
		}
	}
	else//ELSE  - There is no level to edit
	{
		success = true;
	}//IF

	return(success);
}

void EditorLogic::NewLevelCommand()
{
	//Local variables
	bool saveprocessed = true;
	
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		//IF - User saved level
		if(!SaveLevelCommand())
		{
			saveprocessed = false;
		}
	}//IF

	//IF - Save was processed correctly (or there was nothing to save)
	if(saveprocessed)
	{
		std::string emptylevelfilepath = g_ConfigOptions.GetScriptsPath() + mEmptyLevelFileName;

		//Load the level with builder object
		LevelBuilder thebuilder(this); //Create edit-mode level loading
				
		//TODO: SEARCH NAMES AND CHOOSE CORRECT FILEPATH AND LEVELNAME
		//Call creation of object (level)
		thebuilder.LoadLevel(emptylevelfilepath,"NEWLEVEL");
		//Get the created level
		mEditedLevel = thebuilder.GetCreatedLevel();
	}//IF
}

//Toggle all commands info
void EditorLogic::ToggleInfoCommand()
{
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		//Toggle display
		mFullInfoText = !mFullInfoText;
	}//IF
}

//Toggle sprite info command
void EditorLogic::ToggleSpriteInfoCommand()
{
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		//Toggle
		mShowSpInfoText = !mShowSpInfoText;
	}
}	

//Current layer commands
void EditorLogic::IncreaseEditLayerCommand()
{
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		//Increment editing layer number
		mCurrentEditingLayer ++;
		//Check limits (lower and higher just for security)
		if(mCurrentEditingLayer > 61)
			mCurrentEditingLayer = 61;
		if(mCurrentEditingLayer < 0)
			mCurrentEditingLayer = 0;

		mChangeLayer = true;
	}//IF
}

void EditorLogic::DecreaseEditLayerCommand()
{
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		//Decrement editing layer number
		mCurrentEditingLayer --;
		//Check limits (lower and higher just for security)
		if(mCurrentEditingLayer > 61)
			mCurrentEditingLayer = 61;
		if(mCurrentEditingLayer < 0)
			mCurrentEditingLayer = 0;

		mChangeLayer = true;
	}//IF
}

//Movement and Zoom Commands
void EditorLogic::ZoomInCommand()
{
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		//Set tracking only
		mZoomIn = true;
	}//IF
}

void EditorLogic::ZoomOutCommand()
{
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		//Set tracking only
		mZoomOut = true;
	}//IF
}

void EditorLogic::MoveCommand(const Vector2& direction)
{
	//Just translate camera direction multiplied by speed
	float speed = 0.02f;
	mMovingSpeed = direction * speed;
	mMovement = true;
}

//Brush commands
void EditorLogic::IncreaseDropTileCommand()
{
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		mIncreaseTileSurf = true;
	}//IF
}

void EditorLogic::DecreaseDropTileCommand()
{
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		mDecreaseTileSurf = true;
	}//IF
}

//Change insertion type
void EditorLogic::ChangeInsertionTypeCommand(int type)
{
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		if(mEditingBrush.mInsertionType != type)
		{
			assert(type <= 2);
			mEditingBrush.mInsertionType = type; //Type 0-Surface 1-Animation 2-Body
			mChangeInsertionType = true;
		}
	}//IF
}

//Tile manipulation commands
void EditorLogic::MoveTileCommand(bool activate)
{
	//IF - There is a current level editing and not other commands to move
	if(mEditedLevel && !mMoveTileExact)
	{
		//IF - First activation of moving
		if(!mMoveTile&& activate)
		{
			//IF - Insertion of gfx
			if(mEditingBrush.mInsertionType == 0
			   ||
			   mEditingBrush.mInsertionType == 1)
			{
				if(mCurrentCollided.sprite)
				{
					mInitialPosX = mCurrentCollided.sprite->GetPosX() - mEditingBrush.currentsprite->GetPosX();
					mInitialPosY = mCurrentCollided.sprite->GetPosY() - mEditingBrush.currentsprite->GetPosY();
				}
				else
				{
					mInitialPosX = mEditingBrush.currentsprite->GetPosX();
					mInitialPosY = mEditingBrush.currentsprite->GetPosY();
				}
			}//ELSE - insertion of tiles
			else if(mEditingBrush.mInsertionType == 2)
			{
				if(mCurrentCollided.body)
				{
					Vector2 cursorpospix (mEditingBrush.currentsprite->GetPosX(), mEditingBrush.currentsprite->GetPosY());
					Vector2 cursorpos = SingletonIndieLib::Instance()->FromPixToCoord(cursorpospix);
					b2Vec2 bodypos(mCurrentCollided.body->GetPosition());
					mInitialBodyPos = Vector2(bodypos.x,bodypos.y) - cursorpos;
				}
				else
				{
					Vector2 cursorpospix (mEditingBrush.currentsprite->GetPosX(), mEditingBrush.currentsprite->GetPosY());
					Vector2 cursorpos = SingletonIndieLib::Instance()->FromPixToCoord(cursorpospix);
					mInitialBodyPos = cursorpos;
				}
			}//IF
			mMoveTile = true;
		}//ELSE - End of moving
		else if (!activate)
		{
			mMoveTile = false;
		}//IF
	}//IF
}

void EditorLogic::MoveTileExactCommand(const Vector2& direction)
{
	//IF - There is a current level editing and not other commands to move
	if(mEditedLevel && !mMoveTile)
	{
		assert(direction.Length() <= 1.0f);
		mMoveExactDirection = direction;
		mMoveTileExact = true;
	}//IF
}

void EditorLogic::RotateTileCommand(bool rotate)
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		mRotateCW = rotate;
	}//IF
}

void EditorLogic::ScaleTileCommand(bool updown, bool active, bool x, bool y)
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		if(updown)
		{
			mScaleTileUp = active;
		}
		else
		{
			mScaleTileDown = active;
		}

		if(active)
		{
			mScaleTileX = x;
			mScaleTileY = y;
		}
	}//IF
}

void EditorLogic::FlipTileHorCommand()	
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		mHorFlip = true;
		mVertFlip = false;
	}//IF
}

void EditorLogic::FlipTileVerCommand()
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		mVertFlip = true;
		mHorFlip = false;
	}//IF
}

void EditorLogic::IncreaseTileTranspCommand()
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		mIncreaseTransp = true;
		mDecreaseTransp = false;
	}//IF
}

void EditorLogic::DecreaseTileTranspCommand()
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		mIncreaseTransp = false;
		mDecreaseTransp = true;
	}//IF
}

void EditorLogic::IncreaseTileZCommand()
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		mIncreaseZ = true;
		mDecreaseZ = false;
	}//IF
}
	
void EditorLogic::DecreaseTileZCommand()
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		mIncreaseZ = false;
		mDecreaseZ = true;
	}//IF
}
	
void EditorLogic::ResetTileRotationCommand()
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		mResetRotation = true;
	}//IF
}
	
void EditorLogic::ResetTileScaleCommand()
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		mResetScale = true;
	}//IF
}

void EditorLogic::ToggleWrapCommand()
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		mToggleWrap = true;
	}//IF
}
	
void EditorLogic::WrapSectionChangeCommand(bool updown, bool active,bool x, bool y)
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		if(updown)
		{
			mWrapTileUp = active;
		}
		else
		{
			mWrapTileDown = active;
		}

		if(active)
		{
			mWrapTileX = x;
			mWrapTileY = y;
		}
	}//IF
}

void EditorLogic::DropTileCommand()
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		mInsertTile = true;
	}//IF
}

void EditorLogic::CloneTileCommand()
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		mCloneTile = true;
	}//IF
}
	

void EditorLogic::DropClonedTileCommand()
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		mDropCloned = true;
	}//IF
}

void EditorLogic::DeleteTileCommand()
{
	//IF - There is a current level editing and some tile collided to move
	if(mEditedLevel)
	{
		mDeleteTile =  true;
	}//IF
}

//Physics body manipulation commands
void EditorLogic::DefineBodyCommand(bool activate)
{
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		//IF - First activation of moving
		if(!mDefiningBody&& activate)
		{
			//Update initial position of brush
			Vector2 pospix = Vector2(mEditingBrush.currentsprite->GetPosX(), mEditingBrush.currentsprite->GetPosY());
			mEditingBrush.startdefpoint = SingletonIndieLib::Instance()->FromPixToCoord(pospix);
			mEditingBrush.enddefpoint = mEditingBrush.startdefpoint;  //Both points are equal initially
			mDefiningBody = true; 
		}//ELSE - While moving
		else if (mDefiningBody && activate)
		{
			//Just update new position in brush
			Vector2 pospix = Vector2(mEditingBrush.currentsprite->GetPosX(), mEditingBrush.currentsprite->GetPosY());
			mEditingBrush.enddefpoint = SingletonIndieLib::Instance()->FromPixToCoord(pospix);
		}//ELSE - End of moving
		else if (mDefiningBody &&!activate)
		{
			//Finally update last position
			Vector2 pospix = Vector2(mEditingBrush.currentsprite->GetPosX(), mEditingBrush.currentsprite->GetPosY());
			mEditingBrush.enddefpoint = SingletonIndieLib::Instance()->FromPixToCoord(pospix);
			//Store definition of body commnand
			mDefineBody = true;
			mDefiningBody = false;
		}//IF
	}//IF
}

void EditorLogic::ToggleShapeInsertionMode()
{
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		mToggleBodiesEditMode = true;
	}
}

void EditorLogic::ChangeInsertionShape()
{
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		mChangeInsertShape = true;
	}//IF
}

void EditorLogic::ChangeBodyToAddShapesTo()
{
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		mChangeAddingBody = true;
	}//IF
}

void EditorLogic::ToggleDrawPhysics()
{
	//IF - There is a current level editing
	if(mEditedLevel)
	{
		mDrawPhysicsElements = !mDrawPhysicsElements;
	}//IF
}	
	
//Init
void EditorLogic::_init()
{

	//***********************CREATE OBJECT MANAGERS*********************************
	//Create physics manager with game parameters
	//Read game parameters for physics
	const PhysicsConfig physicsconf = g_ConfigOptions.GetPhysicsConfiguration(); //Physics
	//Register physics manager with debug draw allways
	mPhysicsMgr = PhysicsManagerPointer(
				new PhysicsManager(physicsconf.gravity,
								  physicsconf.timestep,
								  physicsconf.iterations,
								  physicsconf.worldaabbmax,
								  physicsconf.worldaabbmin,
								  SingletonIndieLib::Instance()->Box2DDebugRender)
					);

	SingletonIndieLib::Instance()->SetDebugDraw(true);		//Set debug drawing in indielib to true allways

	//**********************LOAD NEEDED EDITOR ASSETS*****************************
	//Resource manager loads by default the "Common" resources, so there is no need
	//to load more. Game level editor works with "Commmon" resources
	std::string assetsfilepath = g_ConfigOptions.GetScriptsPath() + mEditorEntitiesFileName;
	
	//---Creation of entities through level builder---
	LevelBuilder thebuilder; //Create edit-mode level loading
			
	//Call creation of object (level)
	thebuilder.LoadLevel(assetsfilepath,"-**NOEDIT**EDITORASSETS-",true);//Only assets loading (last "true")
	
	//No errors, get level pointer (used to store entities assets)
	mLevelEditorEntities = thebuilder.GetCreatedLevel();
	//**************************EDITOR ASSETS LOADED*******************************

	//********************INIT INTERNAL VARIABLES***********************************
	//Current editing camera
	mCurrentCamera = SingletonIndieLib::Instance()->GetCamera("General");

	//Text to show messages to user
	mInfoTextSprite = mLevelEditorEntities->GetEntity("InfoText");
	if(!mInfoTextSprite)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'InfoText'. Editor needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	//Editing brush entity to show current element to insert
	mEditingBrush.currentsprite = mLevelEditorEntities->GetEntity("EditBrush");
	if(!mEditingBrush.currentsprite)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'EditBrush'. Editor needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}

	//Cloning brush (hidden and not added to indielib)
	mCloningBrush.currentsprite = SpritePointer(new IND_Entity2d());
	mCloningBrush.currentsprite->SetShow(false);

	//Text to show current info from edited sprite
	mCurrentCollided.infotextsprite = mLevelEditorEntities->GetEntity("EditText");
	if(!mCurrentCollided.infotextsprite)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'EditText'. Editor needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	mCurrentCollided.infotextsprite->SetShow(false);		//Dont show initially
	
	//Get container with pointers of all available common resources
	mAvailableResources = SingletonResourceMgr::Instance()->GetResourcesOfLevel(""); //Without name is common resources
	mCurrentBackdropSurface = mAvailableResources->surfacesMap.begin();
	mCurrentBackdropAnimation = mAvailableResources->animationsMap.begin();
	//TODO: CHECK THERE IS ANY RESOURCE LOADED TO ASSIGN!!!
	//Assign current brush initial surface
	mEditingBrush.resourceId = (*mCurrentBackdropSurface).first;  //Get Id of surface and assign to editing brush
	_putBrushAttributes();	 //Put attributes to brush
	_resetCommands();  //Reset input commands
	//*****************************************************************************
	
	//*************************CREATE EDITING LAYER CAMERA*************************
	//Create a special editing layer camera, and override max and min zooms for edit mode
	mEditLayerCam = SingletonIndieLib::Instance()->RegisterCamera("EditLayer",1.0f,Vector2(0,0),62);   //Editing layer is one below overlay
	SingletonIndieLib::Instance()->GetCamera("General")->SetMaxMinZoom(1.0f,0.01f);
	mEditLayerCam->SetMaxMinZoom(1.0f,0.01f);
	mEditLayerCam->SetLimitingMoves(false);  //Dont Limit camera moves by AABB
	//*****************************************************************************

	//*********************CREATE INPUT CONTROLLERS********************************
	//Mouse cursor
	SpritePointer pt = 	mLevelEditorEntities->GetEntity("MouseCursor");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'MouseCursor'. Editor needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	mEditorMouse = new EditorMouse(pt,this);
	mEditorKeyBoard = new EditorKeyBoard(this);

	//*****************INPUT CONTROLLERS CREATED***********************************
	
	//**************************LISTEN FOR EVENTS**********************************
	mListener = new EditorLogicListener(this);
	//*****************************************************************************

	SingletonLogMgr::Instance()->AddNewLine("EditorLogic","Game started in EditMode",LOGNORMAL);
}

//Release
void EditorLogic::_release()
{
	//Delete input controllers classes
	if(mEditorMouse)
	{
		delete mEditorMouse;
		mEditorMouse = NULL;
	}

	if(mEditorKeyBoard)
	{
		delete mEditorKeyBoard;
		mEditorKeyBoard = NULL;
	}

	//Delete event listener
	if(mListener)
	{
		delete mListener;
		mListener = NULL;
	}
}

//Reset commands
void EditorLogic::_resetCommands()
{
	mChangeLayer = false;
	mMovement= false;
	mZoomIn= false;
	mZoomOut= false;
	mIncreaseTileSurf= false;
	mDecreaseTileSurf= false;
	mChangeInsertionType= false;
	mMoveTile= false;
	mRotateCW= false;
	mScaleTileUp= false;
	mScaleTileDown= false;
	mScaleTileX = false;
	mScaleTileY = false;
	mVertFlip= false;
	mHorFlip= false;
	mIncreaseTransp= false;
	mDecreaseTransp= false;
	mIncreaseZ= false;
	mDecreaseZ= false;
	mResetRotation= false;
	mResetScale= false;
	mInsertTile= false;
	mDeleteTile= false;
	mCloneTile= false;
	mDropCloned= false;
	mDefineBody= false;
	mChangeInsertShape = false;
	mToggleBodiesEditMode= false;
	mChangeAddingBody = false;
	mDrawPhysicsElements = false;
	mToggleWrap = false;
	mWrapTileUp = false;
	mWrapTileDown = false;
	mWrapTileX = false;
	mWrapTileY = false;
}

//Windows open file dialog creation
std::string EditorLogic::_fileDialog (char *filter, bool open, char *title)
{	
	bool keepfullscreen = false;

	if (SingletonIndieLib::Instance()->Window->IsFullScreen())
	{
		SingletonIndieLib::Instance()->Render->ToggleFullScreen();
		keepfullscreen = true;
	}

	OPENFILENAME filename;
	char szFile[MAX_PATH];
	char currentdir[MAX_PATH];

	szFile[0] = 0;
	GetCurrentDirectory (MAX_PATH, currentdir);

	filename.lStructSize = sizeof (OPENFILENAME);
	filename.hwndOwner = SingletonIndieLib::Instance()->Window->GetWnd();
	filename.lpstrFilter = filter;
	filename.lpstrCustomFilter = NULL;
	filename.nMaxCustFilter = 0;
	filename.nFilterIndex = 0;
	filename.lpstrFile = szFile;
	filename.nMaxFile = sizeof( szFile );
	filename.lpstrFileTitle = 0;
	filename.nMaxFileTitle = 0;
	filename.lpstrInitialDir = currentdir;
	filename.lpstrTitle = title;
	filename.nFileOffset = 0;
	filename.nFileExtension = 0;
	filename.lpstrDefExt = NULL;
	filename.lCustData = 0;
	filename.lpfnHook = NULL;
	filename.lpTemplateName = NULL;
	filename.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR; // Keep the relative part intact.

	std::string result(""); //Construct filepath result

	//IF - OpenDialog
	if (open)
	{
		//IF - User accepts dialog
		if (GetOpenFileName (&filename))
		{
			//Change to fullscreen if it was
			if (keepfullscreen)	
				SingletonIndieLib::Instance()->Render->ToggleFullScreen ();
			
			result.append(szFile);	
		}//IF
	}
	else //ELSE - SaveDialog
	{
		//IF - User accepts dialog
		if (GetSaveFileName (&filename))
		{
			//Change to fullscreen if it was
			if (keepfullscreen)
				SingletonIndieLib::Instance()->Render->ToggleFullScreen ();
			
			result.append(szFile);	
		}//IF
	}//IF

	return result;
}

//Default attributes for brush when changing it (IndieLib resets them)
void EditorLogic::_putBrushAttributes()
{
	//Default brush attributes when changed
	if(mEditingBrush.mInsertionType == 0)
	{
		mEditingBrush.resourceId = (*mCurrentBackdropSurface).first;
		mEditingBrush.currentsprite->SetSurface((*mCurrentBackdropSurface).second.get()); //Set the surface in indielib
	}
	else if(mEditingBrush.mInsertionType == 1)
	{
		mEditingBrush.resourceId = (*mCurrentBackdropAnimation).first;
		mEditingBrush.currentsprite->SetAnimation((*mCurrentBackdropAnimation).second.get()); //Set the animation in indielib
	}
	mEditingBrush.currentsprite->SetHotSpot(0.5f, 0.5f); //Center editing hotspot
	mEditingBrush.currentsprite->SetTransparency(128);  //Show brush semitransparent
	mEditingBrush.currentsprite->ShowGridAreas(false);	//Dont show grid areas
	mEditingBrush.currentsprite->ShowCollisionAreas(false); //Dont show collision areas
}


//Copy attribues from sprite
void EditorLogic::_cloneSprite(const EditedElement& toclone)
{
	//Clone attributes to cloning brush
	//Create new sprite with default params from surface (user can change it after inserting
	//IF - Surface
	if(toclone.sprite->GetSurface() !=  NULL)
	{
		//Copy surface
		IND_Surface* thesurface = toclone.sprite->GetSurface();
		mCloningBrush.resourceId = SingletonResourceMgr::Instance()->GetSurfaceId(thesurface);
		mCloningBrush.currentsprite->SetSurface(thesurface);
		
		//Generate a valid new name of cloned
		mEditedSprites = 0;
		bool notgood = true;
		//LOOP - Generate names till one is valid
		do
		{	//Generate a name
			std::stringstream name;
			name<<"ImageCopy"<<mEditedSprites;
			//Check if it is valid
			GameLevel::EntitiesMapIterator itr = mEditedLevel->mEntitiesMap.find(name.str());
			if(itr != mEditedLevel->mEntitiesMap.end())
				mEditedSprites++;
			else
			{
				mCloningBrush.spriteId = name.str();
				notgood = false;
			}
		}while(notgood); //LOOP END

	}//ELSE - Animation
	else if(toclone.sprite->GetAnimation() !=  NULL)
	{
		//Copy animation
		IND_Animation* theanimation = toclone.sprite->GetAnimation();
		mCloningBrush.resourceId = SingletonResourceMgr::Instance()->GetAnimationId(theanimation);
		mCloningBrush.currentsprite->SetAnimation(theanimation);
		
		//Generate a valid new name of cloned
		mEditedSprites = 0;
		bool notgood = true;
		//LOOP - Generate names till one is valid
		do
		{	//Generate a name
			std::stringstream name;
			name<<"AnimationCopy"<<mEditedSprites;
			//Check if it is valid
			GameLevel::EntitiesMapIterator itr = mEditedLevel->mEntitiesMap.find(name.str());
			if(itr != mEditedLevel->mEntitiesMap.end())
				mEditedSprites++;
			else
			{
				mCloningBrush.spriteId = name.str();
				notgood = false;
			}
		}while(notgood); //LOOP END
	}
	//Copy Z order
	float currentx = mCloningBrush.currentsprite->GetPosX();
	float currenty = mCloningBrush.currentsprite->GetPosY();
	mCloningBrush.currentsprite->SetPosition(currentx,currenty,toclone.sprite->GetPosZ());
	//Copy scaling
	mCloningBrush.currentsprite->SetScale(toclone.sprite->GetScaleX(),toclone.sprite->GetScaleY());
	//Copy rotation
	mCloningBrush.currentsprite->SetAngleXYZ(toclone.sprite->GetAngleX(),
											 toclone.sprite->GetAngleY(),
											 toclone.sprite->GetAngleZ());
	//Copy transparency
	mCloningBrush.currentsprite->SetTransparency(toclone.sprite->GetTransparency());
	//Copy mirroring X and Y
	mCloningBrush.currentsprite->SetMirrorX(toclone.sprite->GetMirrorX());
	mCloningBrush.currentsprite->SetMirrorY(toclone.sprite->GetMirrorY());
	//Copy tile wrapping
	mCloningBrush.currentsprite->ToggleWrap(toclone.sprite->IfWrap());
	mCloningBrush.currentsprite->SetRegion(0,0,toclone.sprite->GetRegionWidth(),toclone.sprite->GetRegionHeight());
}
//Update showing info for edited sprite
void EditorLogic::_updateSpriteInfo()
{
	//IF - Editing GFX
	if((mEditingBrush.mInsertionType == 0 
	   ||
	   mEditingBrush.mInsertionType == 1)
	   &&
	   mCurrentCollided.sprite)
	{
		//Show info of: Position and name
		float posx = mCurrentCollided.sprite->GetPosX();  //Position
		float posy = mCurrentCollided.sprite->GetPosY();
		int posz = mCurrentCollided.sprite->GetPosZ();
		std::stringstream ss;
		ss<<mCurrentCollided.name<<"\n"<<"Position:"<<posx<<", "<<posy<<", "<<posz;  //And name added
		mCurrentCollided.infotext = ss.str();
		//Set the text of sprite
		mCurrentCollided.infotextsprite->SetText(const_cast<char*>(mCurrentCollided.infotext.c_str()));
	}//ELSE  -  Editing Physical bodies
	else if(mEditingBrush.mInsertionType == 2
		    &&
			mCurrentCollided.body)
	{
		//Show info of: Position and name
		b2Vec2 pos = mCurrentCollided.body->GetPosition();
		std::stringstream ss;
		ss<<mCurrentCollided.name<<"\n"<<"Position:"<<pos.x<<", "<<pos.y;  //And name added
		mCurrentCollided.infotext = ss.str();
		//Set the text of sprite
		mCurrentCollided.infotextsprite->SetText(const_cast<char*>(mCurrentCollided.infotext.c_str()));
	}//IF
}

//Check collisions with editing brush (returns a valid SpritePointer if collided)
SpritePointer EditorLogic::_checkCollisionswithBrush(std::string &collidedname)
{
	//Get IndieLib pointer
	IndieLibManager* Ilib = SingletonIndieLib::Instance();

	//Loop all entities in current editing layer, and check collision with them
	GameLevel::EntitiesMapIterator entitr;
	//LOOP - Loop all entities
	for(entitr = mEditedLevel->mEntitiesMap.begin(); entitr != mEditedLevel->mEntitiesMap.end();entitr++)
	{
		//Get entity
		SpritePointer secondentity = (*entitr).second;
		//IF - Entity is in current editing layer
		if(secondentity->GetLayer() == mCurrentEditingLayer)
		{
			//IF - Check collision with this entity
			if(
				Ilib->Entity2dManager
									 ->IsCollision(mLevelEditorEntities->GetEntity("MouseCursor").get(), //First entity
												   "*",
												   secondentity.get(), //Checked entity
												   "*")
												   )
			{				
				collidedname = (*entitr).first;
				return(secondentity);
			}//IF
		}//IF
	}//LOOP
				
	//Not found - Empty sprite pointer
	return(SpritePointer());
}