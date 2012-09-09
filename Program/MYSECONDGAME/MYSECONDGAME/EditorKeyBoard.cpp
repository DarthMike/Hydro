/*
	Filename: EditorKeyBoard.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to keyboard input
	Comments: It depends of IndieLib, as it will be library used for input
	          The keyboard is used to control things by the user in editor mode
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "EditorKeyBoard.h"
#include "Camera2D.h"
#include "IndieLibManager.h"
#include "EditorLogic.h"
#include "GameEvents.h"
#include "GameEventManager.h"

//Update method
void EditorKeyBoard::Update(float)
{
	//Get input and process
	IND_Input* input = SingletonIndieLib::Instance()->Input;
	Camera2DPointer currentcamera = mEditor->GetCurrentRenderCamera(); //Current editing camera
	
	//Exit
	if(input->IsKeyPressed(IND_ESCAPE))
	{
		//Send event of "exit"
		SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new EventData(Event_ExitGame))
											);
	}

	//Move commands
	if(!input->IsKeyPressed(IND_LCTRL)
	   &&
	   !input->IsKeyPressed(IND_RCTRL))
	{
		Vector2 direction(0,0);
		if(input->IsKeyPressed(IND_KEYLEFT))
		{
			direction.x -= 1.0;
		}
		if(input->IsKeyPressed(IND_KEYRIGHT))
		{
			direction.x += 1.0;
		}
		if(input->IsKeyPressed(IND_KEYUP))
		{
			direction.y += 1.0;
		}
		if(input->IsKeyPressed(IND_KEYDOWN))
		{
			direction.y -= 1.0;
		}
		//Send move command to editor logic
		mEditor->MoveCommand(direction);
	}
	//Tile move commands
	else if(input->IsKeyPressed(IND_LCTRL)
	   ||
	   input->IsKeyPressed(IND_RCTRL))
	{
		Vector2 tilemovedirection(0,0);
		if(input->OnKeyPress(IND_KEYLEFT))
		{
			tilemovedirection.x -= 1.0;
		}
		if(input->OnKeyPress(IND_KEYRIGHT))
		{
			tilemovedirection.x += 1.0;
		}
		if(input->OnKeyPress(IND_KEYUP))
		{
			tilemovedirection.y += 1.0;
		}
		if(input->OnKeyPress(IND_KEYDOWN))
		{
			tilemovedirection.y -= 1.0;
		}
		//Send move command to editor logic
		if(tilemovedirection.x != 0 || tilemovedirection.y != 0)
			mEditor->MoveTileExactCommand(tilemovedirection);
	}
	
	//Save - New - Load Commands
	if(input->IsKeyPressed(IND_LCTRL)
	   ||
	   input->IsKeyPressed(IND_RCTRL))
	{
		//IF - Save command
		if(input->OnKeyPress(IND_S))
		{
			mEditor->SaveLevelCommand();
		}//ELSE IF - Load command
		else if(input->OnKeyPress(IND_L))
		{	
			mEditor->LoadLevelCommand();
		}//ELSE IF - "New" command	
		else  if(input->OnKeyPress(IND_N))
		{
			mEditor->NewLevelCommand();
		}
	}
	//Toggle all info command
	else if(input->OnKeyPress(IND_SPACE))
		mEditor->ToggleInfoCommand();
	//Toggle sprite info command
	else if(input->OnKeyPress(IND_I))
		mEditor->ToggleSpriteInfoCommand();
	
	//Increase / Decrease editing layer commands
	else if(
		    input->OnKeyPress(IND_PLUS)
			||
			input->OnKeyPress(IND_KPLUS)
			)
		mEditor->IncreaseEditLayerCommand();
	else if(
			input->OnKeyPress(IND_MINUS)
			||
			input->OnKeyPress(IND_KMINUS)
			)
			mEditor->DecreaseEditLayerCommand();

	//Change insertion tile commands
	else if(input->OnKeyPress(IND_X))
	{
		mEditor->IncreaseDropTileCommand();
	}
	else if(input->OnKeyPress(IND_Z))
	{
		mEditor->DecreaseDropTileCommand();
		//Body shape type too
		mEditor->ChangeInsertionShape();
	}
	
	//Change insertion type commands
	else if(input->OnKeyPress(IND_0))
	{
		mEditor->ChangeInsertionTypeCommand(0);
	}
	else if(input->OnKeyPress(IND_1))
	{
		mEditor->ChangeInsertionTypeCommand(1);
	}
	else if(input->OnKeyPress(IND_2))
	{
		mEditor->ChangeInsertionTypeCommand(2);
	}

	//Change insertion mode for bodies
	else if(input->OnKeyPress(IND_TAB))
	{
		mEditor->ToggleShapeInsertionMode();
	}
	//Copy - paste tile commands
	if(input->IsKeyPressed(IND_LCTRL)
		)
	{
		//IF - Save command
		if(input->OnKeyPress(IND_C))
		{
			mEditor->CloneTileCommand();
		}//ELSE IF - Load command
		else if(input->OnKeyPress(IND_V))
		{	
			mEditor->DropClonedTileCommand();
		}
	}
	//Drop tile command
	if(input->OnKeyPress(IND_RETURN)
	   ||
	   input->OnKeyPress(IND_LSHIFT))
	{
		mEditor->DropTileCommand();
	}
	//Rotate tile commands
	else if(input->IsKeyPressed(IND_R))
	{
		mEditor->RotateTileCommand(true);
	}
	else if(!input->IsKeyPressed(IND_R))
	{
		mEditor->RotateTileCommand(false);
	}

	//Scale tile commands
	if(input->IsKeyPressed(IND_Q))
	{
		if(input->IsKeyPressed(IND_N))
			mEditor->ScaleTileCommand(true,true,true,false);
		else if(input->IsKeyPressed(IND_M))
			mEditor->ScaleTileCommand(true,true,false,true);
		else
			mEditor->ScaleTileCommand(true,true);
	}
	else if(!input->IsKeyPressed(IND_Q))
	{
		mEditor->ScaleTileCommand(true,false);
	}

	
	if(input->IsKeyPressed(IND_E))
	{
		if(input->IsKeyPressed(IND_N))
			mEditor->ScaleTileCommand(false,true,true,false);
		else if(input->IsKeyPressed(IND_M))
			mEditor->ScaleTileCommand(false,true,false,true);
		else
			mEditor->ScaleTileCommand(false,true);
	}
	else if(!input->IsKeyPressed(IND_E))
	{
		mEditor->ScaleTileCommand(false,false);
	}

	//Tile flipping commands
	if(input->OnKeyPress(IND_F))
	{
		mEditor->FlipTileHorCommand();
	}
	if(input->OnKeyPress(IND_G))
	{
		mEditor->FlipTileVerCommand();
	}

	//Transparency commands
	if(input->OnKeyPress(IND_A))
	{
		mEditor->IncreaseTileTranspCommand();
	}
	if(input->OnKeyPress(IND_D))
	{
		mEditor->DecreaseTileTranspCommand();
	}

	//Z- Ordering commands
	if(input->OnKeyPress(IND_W))
	{
		mEditor->IncreaseTileZCommand();
	}
	else if(input->OnKeyPress(IND_S))
	{
		mEditor->DecreaseTileZCommand();
	}

	//Reset scale and rotation commands
	if(input->OnKeyPress(IND_O))
	{
		mEditor->ResetTileScaleCommand();
	}
	if(input->OnKeyPress(IND_P))
	{	
		mEditor->ResetTileRotationCommand();
	}

	//Tile Wrapping commands

	if(input->IsKeyPressed(IND_LCTRL)
	   &&
	   input->OnKeyPress(IND_T))
	{
		mEditor->ToggleWrapCommand();
	}

	if(input->IsKeyPressed(IND_H))
	{
		if(input->IsKeyPressed(IND_N))
			mEditor->WrapSectionChangeCommand(true,true,true,false);
		else if(input->IsKeyPressed(IND_M))
			mEditor->WrapSectionChangeCommand(true,true,false,true);
		else
			mEditor->WrapSectionChangeCommand(true,true);
	}
	else if(!input->IsKeyPressed(IND_Q))
	{
		mEditor->WrapSectionChangeCommand(true,false);
	}

	if(input->IsKeyPressed(IND_J))
	{
		if(input->IsKeyPressed(IND_N))
			mEditor->WrapSectionChangeCommand(false,true,true,false);
		else if(input->IsKeyPressed(IND_M))
			mEditor->WrapSectionChangeCommand(false,true,false,true);
		else
			mEditor->WrapSectionChangeCommand(false,true);
	}
	else if(!input->IsKeyPressed(IND_E))
	{
		mEditor->WrapSectionChangeCommand(false,false);
	}

	//Delete tile command
	if(input->OnKeyPress(IND_BACKSPACE)
	   ||
	   input->OnKeyPress(IND_DELETE)
	   )
	{
		mEditor->DeleteTileCommand();
	}

	//Bodies showing/disabling
	if(input->OnKeyPress(IND_F1))
	{
		mEditor->ToggleDrawPhysics();
	}
}
	
//Render necessary elements
void EditorKeyBoard::Render()
{
	//For now done by entities
}