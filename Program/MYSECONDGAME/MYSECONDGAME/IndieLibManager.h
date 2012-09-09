/*
	Filename: IndielibManager.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Manager class to wrap INDIELIB library
	Comments: 
	Attribution: IndieLib creator of course! - Javier López López (javilop.com)
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*CHANGELOG
	- Added concept of camera management to this manager
	- Added debug draw of Box2D to this manager, wrapper to drawing callbacks with this library
	- Added GFXEffects to this manager, wrapper to higher level effects
	- There were mem leaks not deleting global managers of indielib created dynamically!
	- Added utility functions to scale-to-fit a 2d entity (it can be image, surface or animation)

*/
/*TODO:
    - IT WOULD BE NICE TO IMPLEMENT MANAGERS IN SHARED POINTERS
	- IT WOULD BE NICER EVEN TO MODIFY INDIELIB TO WORK WITH RESOURCES STORED IN SHARED POINTERS
	  BUT IT IS A BIG WORK AND I DONT HAVE TIME FOR NOW...
*/
#ifndef _INDIELIBWRAPPER
#define _INDIELIBWRAPPER

//Library dependencies
#include "IndieLib\Common\LibHeaders\Indie.h"
#include <list>

//Class dependencies
#include "Singleton_Template.h"
#include "GenericException.h"
#include "LogManager.h"
#include "Vector2.h"
#include "IndieBox2DRender.h"
#include "Shared_Resources.h"

//Forward declarations
class GFXEffects;
struct ColorRGBA;
struct ColorHSLA;

class IndieLibManager : public MeyersSingleton<IndieLibManager>
{
	//Definitions
private:
	typedef std::list<Camera2DPointer> CamerasList;
	typedef CamerasList::iterator CamerasListIterator;

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	IndieLibManager():
	  mSystemInitialized(false),
	  GFXEffectsManager(NULL),
	  Box2DDebugRender(NULL),
	  mGlobalScale(1.0f),
	  mDebugDraw(false)
	{
		_init();
	}
	~IndieLibManager()
	{
		_release();
	}
	//----- GET/SET FUNCTIONS -----
	void SetGeneralScale(float scale) {mGlobalScale = scale;} 	//Set General scale
	float GetGeneralScale() { return mGlobalScale; }			//Get General scale
	Camera2DPointer GetCamera(const std::string &name);				//Get a camera to manipulate it
	void SetDebugDraw(bool activate);
	//----- OTHER FUNCTIONS -----
	//Render loop
	void RenderRoutine();
	//Cameras manipulation
	Camera2DPointer RegisterCamera(const std::string &name, float zoomfactor, const Vector2 &position, int layer);  //Register a new camera
	void DeRegisterCamera(const std::string &name);
	void UpdateCameras(float dt);

	//GFX Utilites
	void ScaleToFit(IND_Entity2d* entity, IND_Surface* originsurface,int finalx, int finaly);
	void ScaleToFit(IND_Entity2d* entity, IND_Animation* originalanimation,int finalx, int finaly);
	void ScaleToFit(IND_Entity2d* entity, int originalx, int originaly,int finalx, int finaly);
	Vector2 FromCoordToPix(const Vector2& coords);
	Vector2 FromPixToCoord(const Vector2& coords);
	ColorRGBA FromHSLToRGB(const ColorHSLA& incolor);
	ColorHSLA FromRGBToHSL(const ColorRGBA& incolor);

	//----- PUBLIC VARIABLES ------
	// ----- IndieLib objects -----
	IND_3dMeshManager		*MeshManager;
	IND_Input				*Input;
	IND_Window				*Window;
	IND_Render				*Render;
	IND_LightManager		*LightManager;
	IND_ImageManager		*ImageManager; 
	IND_SurfaceManager		*SurfaceManager;
	IND_AnimationManager	*AnimationManager;
	IND_FontManager			*FontManager;
	IND_Entity2dManager		*Entity2dManager;
	IND_Entity3dManager		*Entity3dManager;
	IND_Math				*Math;
	GFXEffects				*GFXEffectsManager;
	IndieBox2DRender		*Box2DDebugRender; 

private:
	//----- INTERNAL VARIABLES -----
	bool mSystemInitialized;				//System init tracking
	CamerasList mCamerasList;					//Internal cameras container
	float mGlobalScale;						//Global scale from "units" to "pixels" 
	Vector2 mMaxAABB;						//AABB max
	Vector2 mMinAABB;						//AABB min
	bool mDebugDraw;						//Set or not debug draw display
	//----- INTERNAL FUNCTIONS -----
	void _resetCurrentDirectory_W( void );
	void _init();
	void _release();
	int _toRGB1(float rm1, float rm2, float rh);
	//Sorting function class for containments of cameras by layer
	class CamerasCompareClass
	{
	public:
		bool operator()(const Camera2DPointer& firstcamera, const Camera2DPointer& secondcamera);
	};
};

//SINGLETON INDIELIB MANAGER
//typedef MeyersSingleton<IndieLibManager> SingletonIndieLib;
typedef IndieLibManager SingletonIndieLib;

#endif