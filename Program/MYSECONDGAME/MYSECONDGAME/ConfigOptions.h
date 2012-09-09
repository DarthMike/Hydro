/*
	Filename: ConfigOptions.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class wich reads a file and holds configuration options for game
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _CONFIGOPTIONS
#define _CONFIGOPTIONS

//Library dependencies
#include <string>

//Class dependencies
#include "XMLParser.h"
#include "PhysicsManager.h"

//Definitions
//Config values related to graphics
typedef struct GFXConfig
{
	//Default values constructor
	GFXConfig():
	resx(800),
	resy(600),
	fullscreen(false),
	vsync(false)
	{}
	//Generic constructor
	GFXConfig(int rx,int ry,bool fs,bool vs):
	resx(rx),
	resy(ry),
	fullscreen(fs),
	vsync(vs)
	{}
	int resx;
	int resy;
	bool fullscreen;
	bool vsync;
}GFXConfig;

//Config values related to physics
typedef struct PhysicsConfig
{
	//Default values constructor
	PhysicsConfig():
	timestep(0.2f),
	iterations(10),
	gravity(b2Vec2(0.0f,-10.0f)),
	worldaabbmax(b2Vec2(10.0f,10.0f)),
	worldaabbmin(b2Vec2(-10.0f,-10.0f)),
	globalscale(10.0f)
	{}
	//Generic constructor
	PhysicsConfig(float tstep,int iter,const b2Vec2& grav,const b2Vec2& aabbmax,const b2Vec2& aabbmin,float scale):
	timestep(tstep),
	iterations(iter),
	gravity(grav),
	worldaabbmax(aabbmax),
	worldaabbmin(aabbmin),
	globalscale(scale)
	{}
	float timestep;
	int	iterations;
	b2Vec2 gravity;
	b2Vec2 worldaabbmax;
	b2Vec2 worldaabbmin;
	float globalscale;
}PhysicsConfig;

class ConfigOptions
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	ConfigOptions():
    mPathLoaded(false)
	{
	}
	~ConfigOptions()
	{
	}
	//----- GET/SET FUNCTIONS -----
	const GFXConfig& GetGFXConfiguration() { assert(mPathLoaded); return mGraphicsConfig; }
	const PhysicsConfig& GetPhysicsConfiguration() { assert(mPathLoaded); return mPhysicsConfig; }
	const std::string& GetScriptsPath() { assert(mPathLoaded); return mScriptsPath; }
	const std::string& GetWorkingPath() { assert(mPathLoaded); return mWorkingPath; }
	//----- OTHER FUNCTIONS -----
	void SetupWorkingPath();  
	void ReadConfigOptions() { 	assert(mPathLoaded); _parseConfigOptions(); }
	//----- PUBLIC VARIABLES ------
protected:
	//----- INTERNAL VARIABLES -----
	bool mPathLoaded;							//System path loaded tracking
	GFXConfig mGraphicsConfig;					//Options for graphics
	PhysicsConfig mPhysicsConfig;				//Options for physics
	static const std::string mFileName;			//Name of file with resources definition - inside it is divided by levels
	std::string mScriptsPath;					//Scripts folder path
	std::string mWorkingPath;					//Working path
	std::string mExePath;						//Exe path
	//----- INTERNAL FUNCTIONS -----
	void _parseConfigOptions();
};

#endif
