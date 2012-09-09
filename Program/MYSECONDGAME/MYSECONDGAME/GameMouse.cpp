/*
	Filename: GameMouse.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to mouse input in game screen
	Comments: It depends of IndieLib, as it will be library used for input
	          The game mouse, can take objects and move them using the physics engine
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "GameMouse.h"
#include "GameOverlay.h"
#include "PhysicsSim.h"
#include "PhysicsManager.h"
#include "GameEventManager.h"
#include "GameEvents.h"
#include "Camera2D.h"

//Update method
void GameMouse::Update(float dt)
{
	//Get input and physics manager
	IND_Input* input = SingletonIndieLib::Instance()->Input;
	

	//Update mouse position
	float newx = static_cast<float>(input->GetMouseX());
	float newy = static_cast<float>(input->GetMouseY());
	_updatePosition(newx,newy);
	
	if(input->OnMouseButtonPress(IND_MBUTTON_WHEELUP ))
	{
		Camera2DPointer camera = SingletonIndieLib::Instance()->GetCamera("General");
		camera->Zoom(true);
	}

	if(input->OnMouseButtonPress(IND_MBUTTON_WHEELDOWN ))
	{
		Camera2DPointer  camera = SingletonIndieLib::Instance()->GetCamera("General");
		camera->Zoom(false);
	}

	//Static control variable
	static bool increasing =  true;
	//BLOB THROWING
	if(input->IsMouseButtonPressed(IND_MBUTTON_RIGHT))
	{
		//Construct a new target info		
		mMouseGFX->SetTint(255,0,0);
		mForceCommand = mMouseGFX->GetScaleX();

		//Increment size of pointer to show grow in force
		//IF - Scale not too big
		if(increasing)
		{
			//Increase scale
			mForceCommand += 0.005f*dt;
			if(mForceCommand > 10.0f)
				increasing = false;
			
		}//ELSE - Force scale is too big
		else
		{
			//Decrease scale
			mForceCommand -= 0.005f*dt;
			if(mForceCommand <= 1.0f)
				increasing = true;
		}

		mMouseGFX->SetScale(mForceCommand,mForceCommand);
	}
	else if(!input->IsMouseButtonPressed(IND_MBUTTON_RIGHT))
	{
		//Check if a new blob should be thrown
		if(mForceCommand > 2.0f)
		{
			ShootBlobCommand command(mPosition,mForceCommand/10.0f);  //Create command (force is in percentage value 0 - 1)
			SingletonGameEventMgr::Instance()->QueueEvent(
														  EventDataPointer(new ShootBlobEvent(Event_ShootBlobCommand,command))
														  );
		}
		//Reset mouse pointer
		mMouseGFX->SetTint(255,255,255);
		mMouseGFX->SetScale(1.0f,1.0f);
		mForceCommand = 0.0f;
		increasing = true;
	}

//******DEBUG COMMANDS*******
#ifdef _DEBUGGING	
	//COMMENTED AS THIS TEST CODE MAKES DEPENDENCY ON PHYSICSSIM CLASS IF NEEDED ADD AGAIN
	//PhysicsManagerPointer physicsmgr = mGame->GetPhysicsManager();
	////Update position of anchor point in mousejoint
	//if(mDragging)
	//{
	//	b2MouseJoint* mousejoint = dynamic_cast<b2MouseJoint*>(physicsmgr->GetJoint(PhysicsManager::MouseJointName));
	//	if(mousejoint)
	//	{
	//		//Mouse joint found, so update target anchor point
	//		float unitx = static_cast<float32>(mPosition.x);
	//		float unity = static_cast<float32>(mPosition.y);
	//		mousejoint->SetTarget(b2Vec2(unitx,unity));
	//	}	
	//		
	//}
	//if(input->OnMouseButtonPress(IND_MBUTTON_LEFT))
	//{
	//	//Query for bodies in the mouse place ("units" coordinates)
	//	b2Vec2 position(static_cast<float32>(mPosition.x),
	//				 static_cast<float32>(mPosition.y));
	//	
	//	b2Body* foundbody = physicsmgr->QueryforBodies(position);
	//	//If a body was found, create an anchor point there (mousejoint) to move it
	//	if(foundbody)
	//	{
	//		//Create the mouse joint definition
	//		b2MouseJointDef md;
	//		md.body2 = foundbody;
	//		md.target = position;
	//		md.maxForce = 1000.0f * foundbody->GetMass();
	//		physicsmgr->CreateMouseJoint(&md);
	//		//Wake up body to move it if it was idle
	//		foundbody->WakeUp();
	//		mDragging = true;
	//	}
	//	
	//}
	//if(input->OnMouseButtonRelease(IND_MBUTTON_LEFT))
	//{
	//	//If it was dragging, destroy the mouse joint
	//	if(mDragging)
	//	{
	//		physicsmgr->DestroyMouseJoint();	
	//		mDragging = false;
	//	}
	//	
	//}
#endif
//**********************
}
	
//Render necessary elements
void GameMouse::Render()
{
	//Render mouse is made by entities...
}

void GameMouse::_updatePosition(float newx,float newy)
{
	//-----Put in position of mouse (pixels) to render in overlay camera-----------
	//relative to overlay position in screen center
	mPositionPix.x = newx - mResX/2;
	mPositionPix.y = newy - mResY/2;

	//-------Update position in world coordinates----------------------------------
	//Get necessary values from camera "general"
	float zoom = SingletonIndieLib::Instance()->GetCamera("General")->GetZoom(); //Zoom of "general" camera
	Vector2 camerapos = SingletonIndieLib::Instance()->GetCamera("General")->GetPosition();		//Position of "general" camera
	//Transform position of mouse to world coordinates
	mPosition.x = (newx) / (mScaleFactor * zoom);
	mPosition.y = (mResY - newy) / (mScaleFactor * zoom);
	
	//Transform position to screen center (in world coordinates)
	Vector2 offset(mResX / (mScaleFactor * zoom),mResY / (mScaleFactor * zoom)); 
	mPosition -= offset/2; 
	//Offset position to real camera position
	mPosition += camerapos;	

	//--------------Update mouse display position---------------------------------
	if(mMouseGFX)
		mMouseGFX->SetPosition(static_cast<float>(mPositionPix.x),
							   static_cast<float>(mPositionPix.y),
							   0);
}