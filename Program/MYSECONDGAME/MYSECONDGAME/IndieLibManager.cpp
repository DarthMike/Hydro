/*
	Filename: IndielibManager.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Manager class to wrap INDIELIB library
	Comments: 
	Attribution: IndieLib creator of course! - Javier López López (javilop.com)
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "IndieLibManager.h"
#include "TCHAR.h"  //For windows...
#include "ConfigOptions.h"
#include "GFXEffects.h"
#include "GFXDefs.h"
#include "IndieBox2DRender.h"
#include "IndieCamera2D.h"
#include "OverlayCamera2D.h"
#include "GameEventManager.h"
#include "GameEvents.h"

//Global config options declaration
extern ConfigOptions g_ConfigOptions;  //Global properties of game

//Render loop periodically
void IndieLibManager::RenderRoutine()
{
	static bool cameraserrorlogged = false;  //Local variable to log errors in entity-camera assignment by layers (it is easy to mistake)
	//------ Render loop ------
	//IT NEEDS BEGINSCENE BEFORE!
	//**LAYER RENDERING**
	//GAME LAYERS
	//Game layers have related 1 and only 1 camera. So, if there is a camera registered
	//for this layer, this layer is rendered. The code creates obligatory 2 cameras
	//by default, at layers 10 and 63 (general camera and overlay)
	//General camera is the master one, where all rendered sprites should be put. If
	//Cameras are put below this level, the contents will be rendered IN PARALLAX SCROLLING.
	//If cameras are put above this level, they will be rendered on top, but in correct positions
	//Note that cameras are ordered ALLWAYS, so the loop can manipulate using this consideration
	//(last camera will be in layer 63)
	CamerasListIterator itr = mCamerasList.begin(); //Begin the list
	CamerasListIterator previtr = itr;
	assert(mCamerasList.size() >= 2); //There are at least overlay and general cameras
	//Set the first camera as current to render all layers there if there
	//are no other cameras
	(*itr)->SetAsCurrent();
	//LOOP - Render entities in all layers by cameras
	for(int i = 0;i < 64;i++)
	{
		//IF - Correct layer for camera
		if((*itr)->GetLayer() == i)
		{
			//Set as current
			(*itr)->SetAsCurrent();
			previtr = itr; 
			itr++;
			//IF - Next camera is OVERLAY
			if(itr == mCamerasList.end() && i != 63)
			{
				//Just store previous camera (dont render in overlay)
				itr == previtr;
			}//IF
		}//ELSE - Layer doesnt correspond
		else 
		{
			//Assign general camera
			GetCamera("General")->SetAsCurrent();  
			//IF - Was not logged before
			if(!cameraserrorlogged)
			{
				//Just log it once per program execution (no to overhead log)
				SingletonLogMgr::Instance()->AddNewLine("IndieLibManager::RenderRoutine","There are some entities assigned a layer without camera on it! Rendered with General Camera (layer 10)",LOGNORMAL);
				cameraserrorlogged = true;
			}//IF
		}

		//Allways render entities (even without camera)
		Entity2dManager->RenderEntities2d (i);

		//Render grid and collision areas (controlled individually with "show" methods in entities)
		Entity2dManager->RenderCollisionAreas(i, 255, 0, 0, 255);
		Entity2dManager->RenderGridAreas(i, 255, 255, 255, 255);

		//Trigger event to render additionals throghout the game
		//Create event data and send it - NOTE: Use of Shared pointers, not need to delete
		RenderInLayerInfo data(i); //Layer info in event
		SingletonGameEventMgr::Instance()->TriggerEvent(
							EventDataPointer (new RenderInLayerXEvent(Event_RenderInLayer,data))
							);
	}//LOOP
	//OVERLAY LAYERS - ON TOP OF EVERYTHING
	//**LAYER RENDERING END**

	#ifdef _DEBUGGING //ONLY DEBUG MODE
	//Show FPS
	Render->ShowFpsInWindowTitle();
	#endif

	//IT NEEDS ENDSCENE AFTER!
}

void IndieLibManager::UpdateCameras(float dt)
{
	CamerasListIterator itr;
	//LOOP - Delete cameras
	for(itr = mCamerasList.begin();itr!=mCamerasList.end();++itr)
	{
		(*itr)->Update(dt);
	}//LOOP END	
}
//Get a camera to manipulate it
Camera2DPointer IndieLibManager::GetCamera(const std::string &name)
{
	CamerasListIterator itr;
	//LOOP - Find camera
	for(itr = mCamerasList.begin(); itr != mCamerasList.end(); itr++)
	{
		//IF - Camera found 
		if((*itr)->GetName() == name)
		{
			//Return the camera
			return (*itr);
		}//IF
	}//LOOP END


	//ERROR
	SingletonLogMgr::Instance()->AddNewLine("IndieLibManager::GetCamera","Error attempting to access nonexistent camera '"+ name + "'",LOGEXCEPTION);
	return (Camera2DPointer ());
}

//Adds a camera to IndieLib and sets it as current 
Camera2DPointer IndieLibManager::RegisterCamera(const std::string &name, float zoomfactor, const Vector2 &position, int layer)
{
	//Check if camera exists
	CamerasListIterator itr;
	bool found = false;
	//LOOP - Find camera
	for(itr = mCamerasList.begin(); itr != mCamerasList.end(); itr++)
	{
		//IF - Camera found 
		if((*itr)->GetName() == name)
		{
			found = true;
			break;
		}//IF
	}//LOOP END
	
	//IF - Camera not found (OK)
	if(!found)
	{
		//Creation of camera
		
		float windowheight = static_cast<float>(Window->GetHeight());
		float windowwidth = static_cast<float>(Window->GetWidth());
		Camera2DPointer newcamera (new IndieCamera2D(position,zoomfactor,mGlobalScale,layer,windowwidth,windowheight,name,mMaxAABB,mMinAABB));
		//Add to container
		mCamerasList.push_back(newcamera);
		//Sort cameras by layer
		CamerasCompareClass comparefunc;
		mCamerasList.sort(comparefunc);

		return(newcamera);
	}
	else
	{
		//Throw exception (incoherence in cameras is not tolerant fault)
		throw GenericException("Cameras creation incoherence. Program bug or bad files config! Camera '" + name + "' was registered twice!",GenericException::INVALIDPARAMS);
	}
	//NO RETURN BECAUSE OF EXCEPTION ;)
}

//Deregisters a camera to IndieLib and sets it as current 
void IndieLibManager::DeRegisterCamera(const std::string &name)
{
	//Check if camera exists
	CamerasListIterator itr;
	bool found = false;
	assert(name != "General" && name!= "Overlay"); //Assert no deregistering of default cameras
	
	//LOOP - Find camera
	for(itr = mCamerasList.begin(); itr != mCamerasList.end(); itr++)
	{
		//IF - Camera found 
		if((*itr)->GetName() == name)
		{
			mCamerasList.erase(itr);
			found = true;
			break;
		}//IF
	}//LOOP END
	
	//IF - Not found
	if(!found)
	{
		SingletonLogMgr::Instance()->AddNewLine("IndieLibManager::DeRegisterCamera","ERROR: Intent to deregister non-existent camera '" + name + "'",LOGEXCEPTION);
	}//IF
}
	
//Helper function to convert color spaces
ColorRGBA IndieLibManager::FromHSLToRGB(const ColorHSLA& incolor)
{
	//Converts RGB: given in values 0-255
	//		   HSL: Hue in degrees(0-360) lightness and saturation (0-255)
	
	int r(0),g(0),b(0);

	if (incolor.saturation == 0.0) // Grauton, einfacher Fall
    {
		r = g = b = static_cast<int>(incolor.lightness * 255.0);
    }
    else
    {
      float rm1, rm2;
         
	  if (incolor.lightness <= 0.5f) 
		  rm2 = incolor.lightness + incolor.lightness * incolor.saturation;  
	  else                     
		  rm2 = incolor.lightness + incolor.saturation - incolor.lightness * incolor.saturation;
	  rm1 = 2.0f * incolor.lightness - rm2;   
	  r = _toRGB1(rm1, rm2, static_cast<float>(incolor.hue) + 120.0f);   
	  g = _toRGB1(rm1, rm2, static_cast<float>(incolor.hue));
	  b = _toRGB1(rm1, rm2, static_cast<float>(incolor.hue) - 120.0f);
    }

	ColorRGBA returncolor(r,g,b,incolor.alpha);

	return(returncolor);

}

//Helper function to convert color spaces
ColorHSLA IndieLibManager::FromRGBToHSL(const ColorRGBA& incolor)
{
	//Converts RGB: given in values 0-255
	//		   HSL: Hue in degrees(0-360) lightness and saturation (0-255)
		//Values of conversion
	float h(0.0f),s(0.0f),l(0.0f);

	int minval = min(incolor.red, min(incolor.green, incolor.blue));
    int maxval = max(incolor.red, max(incolor.green, incolor.blue));
    float diff  = float(maxval) - float(minval);
    float sum   = float(maxval) + float(minval);
   
	l = sum / 510.0f;

    if (maxval == minval) 
    {
      s = 0.0f;
      h = 0.0f; 
    }   
    else 
    { 
		float rnorm = (maxval - incolor.red  ) / diff;      
		float gnorm = (maxval - incolor.green) / diff;
		float bnorm = (maxval - incolor.blue ) / diff;   

		s = (l <= 0.5f) ? (diff / sum) : (diff / (510.0f - sum));

		if (incolor.red  == maxval) h = 60.0f * (6.0f + bnorm - gnorm);
		if (incolor.green == maxval) h = 60.0f * (2.0f + rnorm - bnorm);
		if (incolor.blue  == maxval) h = 60.0f * (4.0f + gnorm - rnorm);
		if (h > 360.0f) h -= 360.0f;
    }
	ColorHSLA returncolor (static_cast<int>(h),
					   s,
					   l,
					   incolor.alpha);
	return(returncolor);
}

int IndieLibManager::_toRGB1(float rm1, float rm2, float rh)
{
  if      (rh > 360.0f) rh -= 360.0f;
  else if (rh <   0.0f) rh += 360.0f;
 
  if      (rh <  60.0f) rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;   
  else if (rh < 180.0f) rm1 = rm2;
  else if (rh < 240.0f) rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;      
                   
  return static_cast<int>(rm1 * 255);
}
//Helper function to transform coordinates from global to pixels
Vector2 IndieLibManager::FromCoordToPix(const Vector2& coords)
{
	return(Vector2(coords.x * mGlobalScale,
				   Window->GetHeight() - coords.y * mGlobalScale));
}

//Helper function to transform coordinates from pixel to globals
Vector2 IndieLibManager::FromPixToCoord(const Vector2& coords)
{
	return(Vector2(coords.x / mGlobalScale,
				   (Window->GetHeight() - coords.y) / mGlobalScale));
}

void IndieLibManager::ScaleToFit(IND_Entity2d* entity, IND_Surface* originsurface,int finalx, int finaly)
{
	//Get original surface dimensions
	int originx = originsurface->GetWidth();
	int originy = originsurface->GetHeight();
	//Call subfunction
	ScaleToFit(entity, originx, originy, finalx, finaly);
}

void IndieLibManager::ScaleToFit(IND_Entity2d* entity, IND_Animation* originalanimation,int finalx, int finaly)
{
	//An animation can have many sequences, and can have many heights-widths
	//as default, scaling is done to BIGGER frame of ALL SEQUENCES
	int sequences = originalanimation->GetNumSequences();
	int biggerx = 0;
	int biggery = 0;
	//LOOP  Find bigger x and y to scale
	for(int i = 0;i<sequences;i++)
	{
		assert(i-1 < sequences);
		int animwidth = originalanimation->GetHighWidth(i);  //Bigger width of this sequence
		int animheight = originalanimation->GetHighHeight(i); //Bigger heigth of this sequence

		//Assignment if it is bigger
		if(animwidth > biggerx)
			biggerx =  animwidth;
		if(animheight > biggery)
			biggery =  animheight;
	}//LOOP END

	//Call subfunction
	ScaleToFit(entity, biggerx, biggery, finalx, finaly);
}

void IndieLibManager::ScaleToFit(IND_Entity2d* entity, int originalx, int originaly,int finalx, int finaly)
{
	//Get ratio to scale entity
	float ratiox =  static_cast<float>( finalx )/ static_cast<float>(originalx);
	float ratioy = static_cast<float>( finaly )/ static_cast<float>(originaly);

	//Scale the entity
	entity->SetScale(ratiox,ratioy);
}

void IndieLibManager::SetDebugDraw(bool activate)
{
	mDebugDraw = activate;
	//IF - Activate debug draw
	if(mDebugDraw)
	{
		//Debug draw config for Box2d
		uint32 flagsDraw = IndieBox2DRender::e_shapeBit|IndieBox2DRender::e_jointBit | IndieBox2DRender::e_centerOfMassBit | IndieBox2DRender::e_aabbBit;
		Box2DDebugRender->SetFlags(flagsDraw);
	}
	else
	{
		//Debug draw config for Box2d
		uint32 flagsDraw = 0; //Nothing
		Box2DDebugRender->SetFlags(flagsDraw);
	}	

}

//IndieLib Init
void IndieLibManager::_init()
{
	//Init protection 
	if(!mSystemInitialized)
	{
		_resetCurrentDirectory_W();

		// IndieLib Initialization, a debug.log file will be created.
		IndieLib::Init (IND_DEBUG_MODE);
			
		//Read values for configuration of game
		const GFXConfig graphicsconf = g_ConfigOptions.GetGFXConfiguration();  //GFX
		const PhysicsConfig physicsconf = g_ConfigOptions.GetPhysicsConfiguration(); //Physics

		mGlobalScale = physicsconf.globalscale;  //Store physics global scaling
		mMaxAABB = Vector2(static_cast<double>(physicsconf.worldaabbmax.x),
				  static_cast<double>(physicsconf.worldaabbmax.y));	 //Store AABB max for world
		mMinAABB = Vector2(static_cast<double>(physicsconf.worldaabbmin.x),
				    static_cast<double>(physicsconf.worldaabbmin.y));	 //Store AABB min for world

		//*********Creation of internal managers***********************
		Input				=   new		IND_Input;
		Window				=	new		IND_Window;
		Render				=	new		IND_Render;
		LightManager		=	new		IND_LightManager;
		ImageManager		=	new		IND_ImageManager;
		SurfaceManager		=	new		IND_SurfaceManager;
		MeshManager			=	new		IND_3dMeshManager;
		AnimationManager	=	new		IND_AnimationManager;
		FontManager			=	new		IND_FontManager;
		Entity2dManager		=	new		IND_Entity2dManager;
		Entity3dManager		=	new		IND_Entity3dManager;
		Math				=	new		IND_Math;

		if (!Window				->Init ("HYDRO", graphicsconf.resx, graphicsconf.resy, 32, graphicsconf.vsync, graphicsconf.fullscreen))
			throw(GenericException("Error initializing Window of Indielib",GenericException::LIBRARY_ERROR));

		if (!Render				->Init (Window))
			throw(GenericException("Error initializing Renderer of Indielib",GenericException::LIBRARY_ERROR));
		if (!LightManager		->Init (Render))									
			throw(GenericException("Error initializing light manager of Indielib",GenericException::LIBRARY_ERROR));
		if (!ImageManager		->Init ())											
			throw(GenericException("Error initializing image manager of Indielib",GenericException::LIBRARY_ERROR));
		if (!SurfaceManager		->Init (ImageManager, Render))						
			throw(GenericException("Error initializing surface manager of Indielib",GenericException::LIBRARY_ERROR));
		if (!AnimationManager	->Init (ImageManager, SurfaceManager))				
			throw(GenericException("Error initializing animation manager of Indielib",GenericException::LIBRARY_ERROR));
		if (!FontManager		->Init (ImageManager, SurfaceManager))				
			throw(GenericException("Error initializing font  manager of Indielib",GenericException::LIBRARY_ERROR));
		if (!Entity2dManager	->Init (Render))									
			throw(GenericException("Error initializing entity2d manager of Indielib",GenericException::LIBRARY_ERROR));
		if (!Entity3dManager	->Init (Render))									
			throw(GenericException("Error initializing entity3d manager of Indielib",GenericException::LIBRARY_ERROR));
		if (!MeshManager		->Init (Render))									
			throw(GenericException("Error initializing mesh manager of Indielib",GenericException::LIBRARY_ERROR));
		if (!Input				->Init (Render))									
			throw(GenericException("Error initializing input manager of Indielib",GenericException::LIBRARY_ERROR));
		if (!Math				->Init ())											
			throw(GenericException("Error initializing math manager of Indielib",GenericException::LIBRARY_ERROR));

		GFXEffectsManager   =   new     GFXEffects(this);
		Box2DDebugRender   =   new     IndieBox2DRender(this,mGlobalScale,static_cast<float>(Window->GetHeight()));
		//***************************************************************

		//*********Cameras setup*****************************************
		//Creation of camera - GENERAL CAMERA (Allways present at system)
		//This camera applies, with zoom, general scaling to images. It is done to make zoom work, and to make other
		//libraries correspond correctly with this one, like Box2D.
		Vector2 position(0,0);
		Camera2DPointer generalcam = RegisterCamera("General",1.0f,position,10); //Layer 10 (Not conflict with other)	
		//Creation of camera - OVERLAY CAMERA (Allways present at system)
		//This is a camera that never moves (renders a fixed overlay)
		Camera2DPointer overlaycam(new OverlayCamera2D(Vector2(0,0),mGlobalScale,63,"Overlay",mMaxAABB,mMinAABB));  //Layer 63 (in top)
		mCamerasList.push_back(overlaycam); //Insert camera in last position (highest layer)
		//***************************************************************

		//*********Debug draw configuration options**********************
		//Flags for debug draw will be put explicitly by user
		Box2DDebugRender->SetScaleFactor(mGlobalScale);
		//***************************************************************

		mSystemInitialized = true;
	}
}

//Release of resources
void IndieLibManager::_release()
{
	// ----- Freeing objects -----

	Math				->End();
	MeshManager			->End();
	Input				->End();
	Entity2dManager		->End();	
	Entity3dManager		->End();
	FontManager			->End();
	AnimationManager	->End();
	SurfaceManager		->End();
	ImageManager		->End();
	LightManager		->End();
	Render				->End();
	Window				->End();

	delete Box2DDebugRender;
	delete GFXEffectsManager;
	delete Math;
	delete Entity3dManager;
	delete Entity2dManager;
	delete FontManager;
	delete AnimationManager;
	delete MeshManager;
	delete SurfaceManager;
	delete ImageManager;
	delete LightManager;
	delete Render;
	delete Window;
	delete Input;

	IndieLib::End ();

	//Delete dynamic memory (shared pointers)
	mCamerasList.clear();

	mSystemInitialized = false;
}


//Sets working path to the directory of exe file (Windows)
void IndieLibManager::_resetCurrentDirectory_W( void )
{
	TCHAR app_path[MAX_PATH] = _T("");
	DWORD size_in_tchars = sizeof(app_path)/sizeof(TCHAR);
	// get full app path with the exe filename
	GetModuleFileName( 0, app_path, size_in_tchars-1 );
	// making app_path to end on dir char ('\\')
	// _tcsrchr - search for char from the string's end
	TCHAR* app_dir = _tcsrchr(app_path, _T('\\'));
	if (app_dir) {
		app_dir += 1;
		if (app_dir) { 
			*app_dir = 0; 
			SetCurrentDirectory(app_path);
			return;
		}
	}
	// TODO. inform somehow that func is failed.
}

//Sorting function for containments of cameras by layer
bool IndieLibManager::CamerasCompareClass::operator()(const Camera2DPointer& firstcamera, const Camera2DPointer& secondcamera)
{
	int layer1 = firstcamera->GetLayer();
	int layer2 = secondcamera->GetLayer();

	//IF - Not irreflexive and intransitive (ejem... weak ordering means elements CANT be equal...)
	if(layer1 == layer2)
		throw GenericException("Camera '" + firstcamera->GetName() + "' and '" + secondcamera->GetName() +"' Have the same layer assigned! REVIEW XML FILES",GenericException::INVALIDPARAMS);

	return(layer1 < layer2);
}