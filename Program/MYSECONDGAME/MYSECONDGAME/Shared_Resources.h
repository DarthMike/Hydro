/*
	Filename: Shared_Resources.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Shared resources (shared_ptr) typedefs
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/


#ifndef _GAMESHAREDRES
#define _GAMESHAREDRES

#include <boost\shared_ptr.hpp>  //Smart pointer facilites - not using std because it cant go inside an 
								 //STL container as we do in event manager...

//Graphics resources
class IND_Entity2d;
typedef boost::shared_ptr<IND_Entity2d> SpritePointer;

class IND_Surface;
typedef boost::shared_ptr<IND_Surface> SurfacePointer;

class IND_Font;
typedef boost::shared_ptr<IND_Font> FontPointer;

class IND_Animation;
typedef boost::shared_ptr<IND_Animation> AnimationPointer;

//Sound resource
class SoundResource;
typedef boost::shared_ptr<SoundResource> SoundResourcePointer;

//Level
class GameLevel;
typedef boost::shared_ptr<GameLevel> GameLevelPointer;

//Camera
class Camera2D;
typedef boost::shared_ptr<Camera2D> Camera2DPointer;

//Agents and Physics Managers
class AgentsManager;
typedef boost::shared_ptr<AgentsManager> AgentsManagerPointer;

class PhysicsManager;
typedef boost::shared_ptr<PhysicsManager> PhysicsManagerPointer;

#endif