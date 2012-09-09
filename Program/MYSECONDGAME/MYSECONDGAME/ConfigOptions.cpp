/*
	Filename: ConfigOptions.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class wich reads a file and holds configuration options for game
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "ConfigOptions.h"
#include <windows.h>

//Path for file definition
const std::string ConfigOptions::mFileName("GameData\\Config\\Settings.xml");

//Get the 
void ConfigOptions::SetupWorkingPath()
{
	//Save working path
	//Compute from system the working path till scripts file
	char filename[MAX_PATH];
	::GetModuleFileName(NULL,filename,MAX_PATH);
	std::string workingdir(filename,MAX_PATH);
	size_t pos = workingdir.find_last_of("\\");
	if(pos == std::string.npos)
		throw GenericException("Working directory could not be resolved. Did you delete some files while playing?",GenericException::INVALIDPARAMS);
		
	workingdir.erase(pos);
	size_t appdirpos2 = workingdir.find_last_of("\\");
	size_t appdirpos1 = workingdir.find_last_of("\\",workingdir.size()-2);
	if(appdirpos1 == std::string.npos || appdirpos2 == std::string.npos)
		throw GenericException("Working directory could not be resolved. Did you delete some files while playing?",GenericException::INVALIDPARAMS);
		
	workingdir.erase(appdirpos1+1,appdirpos2);

	mWorkingPath = workingdir;
	mScriptsPath = mWorkingPath + std::string("GameData\\Scripts\\");
	mPathLoaded = true;
}

//Parsing of options from xml file
void ConfigOptions::_parseConfigOptions()
{
	//****************GENERAL CONFIG FROM XML*******************************
	/*Expected configuration file
	Element: GFX Atts: ResX(number) ResY(number) Fullscreeen(number)
	Element: Physics Atts: 	TimeStepInv(number)	Iterations(number) GravityX(number) GravityY(number)
							AABBxmax(number) AABBymax(number) AABBxmin(number) AABBymin(number) UnitScaling(number)    
	*/
	
	//Open and load document
	ticpp::Document configdoc(mWorkingPath + mFileName);
	configdoc.LoadFile();	//Parsing
	
	//---------------------------Graphics config-----------------------------
	{	
	ticpp::Element* gfxsection = configdoc.FirstChildElement("GFX");

	int resx,resy;
	bool fullscreen, vsync;
	gfxsection->GetAttribute("ResX",&resx);
	gfxsection->GetAttribute("ResY",&resy);
	gfxsection->GetAttribute("Fullscreen",&fullscreen);
	gfxsection->GetAttribute("VSync",&vsync);
	//TODO: CHECK VALUES OF RESOLUTION TO VALID ONES
	//Check values
	if(fullscreen != 1 && fullscreen != 0)
		throw(GenericException("Error reading file '" + mFileName +"' Bad values of Graphics configuration",GenericException::FILE_CONFIG_INCORRECT));

	//Copy values to structure
	mGraphicsConfig.resx = resx;
	mGraphicsConfig.resy = resy;
	mGraphicsConfig.fullscreen = fullscreen;
	mGraphicsConfig.vsync = vsync;
	}
	//---------------------------Physics config-----------------------------
	{
	ticpp::Element* physicssection = configdoc.FirstChildElement("Physics");
	//Timestep
	float timestepinv, timestep;
	physicssection->GetAttribute("TimeStepInv",&timestepinv);
	if(timestepinv > 300.0f || timestepinv <= 5.0f)
		throw GenericException("Failure while reading '" + mFileName + "' bad attributes detected in physics!",GenericException::FILE_CONFIG_INCORRECT);
	timestep = 1.0f / timestepinv;
	//Iterations
	int	iterations;
	physicssection->GetAttribute("Iterations",&iterations);
	if (iterations <= 0 || iterations > 20)
		throw GenericException("Failure while reading '" + mFileName + "' bad attributes detected in physics!",GenericException::FILE_CONFIG_INCORRECT);
	//Gravity
	b2Vec2 gravity;
	physicssection->GetAttribute("GravityX",&gravity.x);
	physicssection->GetAttribute("GravityY",&gravity.y);
	if(gravity.x > 100.0f || gravity.x < -100.0f || gravity.y > 100.0f || gravity.y < -100.0f ) 
		throw GenericException("Failure while reading '" + mFileName + "' bad attributes detected in physics!",GenericException::FILE_CONFIG_INCORRECT);
	//World AABB
	b2Vec2 aabbmax;
	b2Vec2 aabbmin;
	physicssection->GetAttribute("AABBxmax",&aabbmax.x);
	physicssection->GetAttribute("AABBymax",&aabbmax.y);
	physicssection->GetAttribute("AABBxmin",&aabbmin.x);
	physicssection->GetAttribute("AABBymin",&aabbmin.y);
	if(aabbmax.x < aabbmin.x || aabbmax.y < aabbmin.y)
		throw GenericException("Failure while reading '" + mFileName + "' bad attributes detected in physics!",GenericException::FILE_CONFIG_INCORRECT); 
	//Global scaling
	float scale;
	physicssection->GetAttribute("UnitScaling",&scale);
	if(scale < 0.01f || scale > 200.0f)
		throw(GenericException("Error reading file '" + mFileName +"' Bad value of Unit scaling",GenericException::FILE_CONFIG_INCORRECT));
	
	//Copy values to structure
	mPhysicsConfig.iterations = iterations;
	mPhysicsConfig.timestep = timestep;
	mPhysicsConfig.gravity = gravity;
	mPhysicsConfig.worldaabbmax = aabbmax;
	mPhysicsConfig.worldaabbmin = aabbmin;
	mPhysicsConfig.globalscale = scale;
	}
	//**********************************************************************
}