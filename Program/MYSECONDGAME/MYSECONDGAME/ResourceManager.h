/*
	Filename: ResourceManager.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which encapsulates resource loading and managing
	Comments: Initial version - no cache, preloading or strange things; it is a class to contain
			  loading of an xml file in one place, to be able to load parts of it anywhere in code... to be
			  extended to real resource manager
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

/*
	NOTE:
		THERE IS ALSO A SMALL ISSUE IN SOUND MANAGER, AS IT STORES ALL RESOURCES NEEDED INTERNALLY, BUT TO KEEP
		INTERFACE TO CREATE RESOURCES BY RESOURCE MANAGER, ANOTHER REFERENCE TO THE SAME INFORMATION IS ADDED IN
		THIS CLASS... MESSY... BUT FOR NOW IT WILL WORK.

		INDIELIB WORKS WITH NAKED POINTERS, SO WHEN EXCEPTION (WHEN LOADING) OCCURS, ALL RESOURCES ARE
		RELEASED FROM INDIELIB, EVEN THOUGH SHARED POINTERS ARE USED FOR MEMORY ALLOCATION.. IT IS NOT 
		NICE BUT IT WAS THE FASTER SAFE DEVELOPMENT WAY.
*/

#ifndef _RESOURCEMANAGER
#define _RESOURCEMANAGER

//Library dependencies
#include "boost\shared_ptr.hpp"  //Smart pointer facilites - not using std because it cant go inside an 
								 //STL container as we do in event manager...
#include <string>
#include <vector>
#include <map>

//Class dependencies
#include "Singleton_Template.h"
#include "LogManager.h"
#include "GenericException.h"
#include "XMLParser.h"
#include "Shared_Resources.h"


class ResourceManager : public MeyersSingleton<ResourceManager>
{
	//Definitions - Generic resources map
public:
	//Map of string->surface
	typedef std::map<std::string,SurfacePointer> SurfacesMap;
	typedef SurfacesMap::iterator SurfacesMapIterator;
	//Map of string->font
	typedef std::map<std::string,FontPointer> FontsMap;
	typedef FontsMap::iterator FontsMapIterator;
	//Map of string->animation
	typedef std::map<std::string,AnimationPointer> AnimationsMap;
	typedef AnimationsMap::iterator AnimationsMapIterator;
	//Vector of string names for buffers
	typedef std::vector<SoundResourcePointer> AudioresVector;
	typedef AudioresVector::iterator AudioresVectorIterator;

	//Definitions - Level Resources info
public:
	typedef struct LevelResources
	{
		LevelResources(const std::string& name):
			levelname(name)
		{}	
		std::string levelname;			   //Level name
		SurfacesMap surfacesMap;          //Loaded resources
		FontsMap fontsMap;
		AnimationsMap animationsMap;
		AudioresVector audiovector;
	}LevelResources;  //Level resources info

	typedef boost::shared_ptr<LevelResources> LevelResourcesPointer;
	//Definitions - Level resources info container
public:
	typedef std::vector<LevelResourcesPointer> LevelResourcesVec;
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	ResourceManager():
	  mCurrentLevel(""),
	  mFileDocument(mFileName)
	{
		_init();
		SingletonLogMgr::Instance()->AddNewLine("ResourceManager","Initial Resources Loaded and Resource Manager Ready",LOGNORMAL);
	}
	~ResourceManager()
	{
		//All resources are automatically freed by shared pointers!!!
		//But, IndieLib needs to be notified of resource unloading
		_release();
		SingletonLogMgr::Instance()->AddNewLine("ResourceManager","All resources freed and Resource Manager Destroyed",LOGNORMAL);
	}
	//----- GET/SET FUNCTIONS -----
	LevelResourcesPointer GetResourcesOfLevel(const std::string& levelname);	//Get all resources loaded by level
	SurfacePointer GetSurfaceResource(const std::string& name); //Get surface resource pointer
	FontPointer GetFontResource(const std::string& name);		  //Get font resource pointer
	AnimationPointer GetAnimationResource(const std::string& name); //Get animation resource pointer
	//Note: Audio resources are referred by name directly
	//Special gets
	std::string GetSurfaceId(const IND_Surface* surface);  //Gets the id of a given surface
	std::string GetAnimationId(const IND_Animation* animation);  //Gets the id of a given animation
	std::string GetFontId(const IND_Font* font);  //Gets the id of a given font	
	//----- OTHER FUNCTIONS -----
	//Resources loading/unloading for levels
	bool LoadLevelResources(const std::string& levelname);
	bool ReleaseLevelResources(const std::string& levelname);
	
	//----- PUBLIC VARIABLES ------

protected:
	//----- INTERNAL VARIABLES -----
	LevelResourcesVec mLevelResources;			//A container of resources by level
	std::string mCurrentLevel;					//Current level where resources are loaded
	static const std::string mFileName;			//Name of file with resources definition - inside it is divided by levels
	static const std::string mDefaultLevelName;	//Default level name
	ticpp::Document mFileDocument;				//TiCpp object to manage xml files
	//----- INTERNAL FUNCTIONS -----
	bool _releaseLevelResources(const LevelResourcesVec::iterator& leveliterator);
	void _parseLevelResources(const std::string& levelname);
	bool _isLevelLoaded(const std::string& levelname, LevelResourcesVec::iterator& foundlevelitr);
	void _init();					//Init resources
	void _release();				//Release resources
	void _handleException();		//Exception handling
};

typedef ResourceManager SingletonResourceMgr;

#endif
