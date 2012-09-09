/*
	Filename: ResourceManager.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which encapsulates resource loading and managing
	Comments: Initial version - no cache, preloading or strange things; it is a class to contain
			  loading of an xml file in one place, to be able to load parts of it anywhere in code... to be
			  extended to real resource manager
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "ResourceManager.h"
//Dependency with managers 
#include "IndieLibManager.h"
#include "SoundManager.h"
#include "ConfigOptions.h"

const std::string ResourceManager::mFileName = "Resources.xml";
const std::string ResourceManager::mDefaultLevelName = "Common";

//Global config options
extern ConfigOptions g_ConfigOptions;

//Get all resources loaded by level
ResourceManager::LevelResourcesPointer ResourceManager::GetResourcesOfLevel(const std::string& levelname)
{
	//Local variables
	LevelResourcesVec::iterator levelitr;

	std::string leveltofind = levelname;

	//IF - Asked for common resources
	if(levelname == "" )
	{
		leveltofind = mDefaultLevelName;
	}//IF

	//IF - Level was not loaded
	if(!_isLevelLoaded(leveltofind,levelitr))
	{
		//FAILURE - It was not loaded!
		SingletonLogMgr::Instance()->AddNewLine("ResourceManager::GetResourcesOfLevel","Resources for level '" + levelname + "' are not loaded and were consulted!",LOGEXCEPTION);
		return (LevelResourcesPointer());
	}
	return(*levelitr); //Return a copy of the pointer
		
}

SurfacePointer ResourceManager::GetSurfaceResource(const std::string& name)
{
	//LOOP - Search for asked SURFACE 
	for(LevelResourcesVec::iterator itr = mLevelResources.begin();
		itr != mLevelResources.end(); itr++)
	{
		SurfacesMapIterator sfit = (*itr)->surfacesMap.find(name);
		if( sfit != (*itr)->surfacesMap.end())
		{
			return sfit->second;
		}		
	}//LOOP END

	return(SurfacePointer());
}
	
FontPointer ResourceManager::GetFontResource(const std::string& name)
{
	//LOOP - Search for asked FONT 
	for(LevelResourcesVec::iterator itr = mLevelResources.begin();
		itr != mLevelResources.end(); itr++)
	{
		FontsMapIterator fit = (*itr)->fontsMap.find(name);
		if( fit != (*itr)->fontsMap.end())
		{
			return fit->second;
		}		
	}//LOOP END

	return(FontPointer());
}
	
AnimationPointer ResourceManager::GetAnimationResource(const std::string& name)
{
	//LOOP - Search for asked ANIMATION 
	for(LevelResourcesVec::iterator itr = mLevelResources.begin();
		itr != mLevelResources.end(); itr++)
	{
		AnimationsMapIterator ait = (*itr)->animationsMap.find(name);
		if( ait != (*itr)->animationsMap.end())
		{
			return ait->second;
		}		
	}//LOOP END

	return(AnimationPointer());
}

//Gets the id of a given surface
std::string ResourceManager::GetSurfaceId(const IND_Surface* surface)
{
	//LOOP - Search inside each level resources loaded
	for(LevelResourcesVec::iterator itr = mLevelResources.begin();
		itr != mLevelResources.end(); itr++)
	{
		SurfacesMapIterator sfit;
		//LOOP - Search inside the level for asked SURFACE
		for(sfit = (*itr)->surfacesMap.begin(); sfit != (*itr)->surfacesMap.end(); sfit++)
		{
			//IF - Resource pointer is the same
			if( (*sfit).second.get() == surface)
			{
				return (*sfit).first;
			}	
		}//LOOP END
	}//LOOP END

	return(std::string("SURFACENOTFOUND"));
}
//Gets the id of a given animation
std::string ResourceManager::GetAnimationId(const IND_Animation* animation) 
{
	//LOOP - Search inside each level resources loaded
	for(LevelResourcesVec::iterator itr = mLevelResources.begin();
		itr != mLevelResources.end(); itr++)
	{
		AnimationsMapIterator ait;
		//LOOP - Search inside the level for asked ANIMATION
		for(ait = (*itr)->animationsMap.begin(); ait != (*itr)->animationsMap.end(); ait++)
		{
			//IF - Resource pointer is the same
			if( (*ait).second.get() == animation)
			{
				return (*ait).first;
			}	
		}//LOOP END
	}//LOOP END
	return(std::string("ANIMATIONNOTFOUND"));
}
//Gets the id of a given font	
std::string ResourceManager::GetFontId(const IND_Font* font)
{
	//LOOP - Search inside each level resources loaded
	for(LevelResourcesVec::iterator itr = mLevelResources.begin();
		itr != mLevelResources.end(); itr++)
	{
		FontsMapIterator fit;
		//LOOP - Search inside the level for asked ANIMATION
		for(fit = (*itr)->fontsMap.begin(); fit != (*itr)->fontsMap.end(); fit++)
		{
			//IF - Resource pointer is the same
			if( (*fit).second.get() == font)
			{
				return (*fit).first;
			}	
		}//LOOP END
	}//LOOP END
	return(std::string("FONTNOTFOUND"));
}

//Load all resources for a given level
bool ResourceManager::LoadLevelResources(const std::string& levelname)
{
	//Local variables
	LevelResourcesVec::iterator levelitr;

	//Asserts
	assert(mCurrentLevel == mDefaultLevelName
		   ||
		   mCurrentLevel == "");  //There shouldnt be any level loaded!

	//IF - Level was loaded
	if(_isLevelLoaded(levelname,levelitr))
	{
		//FAILURE - Shouldnt be loaded!
		SingletonLogMgr::Instance()->AddNewLine("ResourceManager::LoadLevelResources","Resources for level '" + levelname + "' intended to load more than once!",LOGEXCEPTION);
		return false;
	}

	SingletonLogMgr::Instance()->AddNewLine("ResourceManager::LoadLevelResources","Loading resources for level '" + levelname + "'...",LOGNORMAL);
	//Load resources - It can throw exceptions, handle them!
	try
	{
		//Parse XML file for resources of this level
		_parseLevelResources(levelname);
	}
	catch (GenericException& e)
	{
		_handleException();  //Internal exception handling
		//Re-throw exception when handling is complete
		throw(e);
	}
	
	mCurrentLevel = levelname; //Update current level tracking

	return true;
}

//Release all resources for a given level
bool ResourceManager::ReleaseLevelResources(const std::string& levelname)
{
	SingletonLogMgr::Instance()->AddNewLine("ResourceManager::ReleaseLevelResources","Releasing resources for level '" + levelname + "'...",LOGNORMAL);
	//Local variables
	LevelResourcesVec::iterator levelitr;

	//IF - Level was loaded (store in levelitr the level)
	if(_isLevelLoaded(levelname,levelitr))
	{
		if(_releaseLevelResources(levelitr))
		{
			mLevelResources.erase(levelitr);  //Clear all memory (shared pointer)
			if(mDefaultLevelName !=  levelname)
				mCurrentLevel = mDefaultLevelName;  //Tracking of level loaded (just default loaded)
			return true;
		}
		else
		{
			SingletonLogMgr::Instance()->AddNewLine("ResourceManager::ReleaseLevelResources","Failure when deleting resources for level '" + levelname + "'",LOGEXCEPTION);
			return false;
		}		
	}
	else
	{
		SingletonLogMgr::Instance()->AddNewLine("ResourceManager::ReleaseLevelResources","Couldnt release resources for Level '" + levelname + "': Not loaded before!",LOGEXCEPTION);
		return false;
	}
}

//Parse all resources reading an xml file
void ResourceManager::_parseLevelResources(const std::string& levelname)
{
/*Expected resources file
Element: Level Atts: Id(string)
	Element: Images
			 [Element: StaticImage Atts: Id(string) Path(string) KeyR(number) KeyG(number) KeyB(number)]
			 [Element: Animation Atts: Id(string) Path(string) KeyR(number) KeyG(number) KeyB(number)]
			 .
			 .
			 .
	
	Element: Fonts
			 [Element: Font Atts: Id(string) ImagePath(string) ConfigPath(string)]
			 .
			 .
			 .

	Element: Audio
			 [Element: FX Atts: Id(string) Path(string)]
			 [Element: Music Atts: Id(string) Path(string)]
			 .
			 .
			 .	
Element: Level Atts: Id(string)
	.
	.
	.
*/
	ticpp::Iterator<ticpp::Element> levelsitr;
	bool levelfound = false;
	//LOOP - Find first the level asked for
	for(levelsitr = levelsitr.begin(&mFileDocument); levelsitr != levelsitr.end(); levelsitr++)
	{
		if(levelsitr->GetAttribute("Id") == levelname)
		{
			levelfound = true;
			break;
		}
	}//LOOP END

	//Throw exception if not found
	if(!levelfound)
		throw GenericException("Error while loading '" + mFileName + "'. Level "+ levelname + "Doesnt exist in resources file",GenericException::FILE_CONFIG_INCORRECT);

	//Level found, create an internal structure to hold all resources loaded for it
	LevelResourcesPointer levelres(new LevelResources(levelname));
	mLevelResources.push_back(levelres);

	ticpp::Element* section;
	//***************************LOADING OF IMAGES - ANIMATIONS*****************************************
	section = levelsitr->FirstChildElement("Images");

	ticpp::Iterator<ticpp::Element> imagesitr;
	//LOOP - TiCpp way - Get all elements created in file
	for(imagesitr = imagesitr.begin(section); imagesitr != imagesitr.end(); imagesitr++)
	{	
		//For each element get attributes and create a new surface (IND_Surface)
		//Get the type of image (static or animation)
		std::string type;
		imagesitr->GetValue(&type);
		//IF - FOUND AN STATIC IMAGE
		if( type == "StaticImage")
		{
			//ID
			std::string id = imagesitr->GetAttribute("Id");
			if(
				id == ""
				||
				levelres->surfacesMap.find(id) != levelres->surfacesMap.end()
				)
				throw GenericException("Error while loading '" + mFileName + "'. Id incorrect in an element",GenericException::FILE_CONFIG_INCORRECT);

			//Resource path
			std::string path = imagesitr->GetAttribute("Path");
			if(path == "")
				throw GenericException("Error while loading '" + mFileName + "'.Element '" + id + "': Path non-existent for an element",GenericException::FILE_CONFIG_INCORRECT);
			
			//Color key
			int keyr(255),keyg(255),keyb(255);
			bool keyed (true);
			//Only way to "catch" non existent attribute...
			try
			{
				imagesitr->GetAttribute("KeyR",&keyr);
				imagesitr->GetAttribute("KeyG",&keyg);
				imagesitr->GetAttribute("KeyB",&keyb);			
			}
			catch (ticpp::Exception&)
			{
				keyed = false;
			}
			if(keyed)
			{
				if(keyr < 0 || keyr > 255 || keyg < 0 || keyg > 255 || keyb < 0 || keyb > 255)
					throw GenericException("Error while loading '" + path + "'.Element '" + id + "': Incorrect colorkey values",GenericException::FILE_CONFIG_INCORRECT);
			}
			
			//Creation of Surface
			SurfacePointer newsurface(new IND_Surface());                          //BY DEFAULT - LOADING WITH ALPHA AND MAX QUALITY
			bool success(false);
			if(keyed)
			{
				success = SingletonIndieLib::Instance()->SurfaceManager->Add(newsurface.get(),
																  const_cast<char*>(path.c_str()), //std::String problems with INDIELIB
																  IND_ALPHA,
																  IND_32,
																  static_cast<byte>(keyr),
																  static_cast<byte>(keyg),
																  static_cast<byte>(keyb));
			}
			else
			{
				success = SingletonIndieLib::Instance()->SurfaceManager->Add(newsurface.get(),
																  const_cast<char*>(path.c_str()), //std::String problems with INDIELIB
																  IND_ALPHA,
																  IND_32);
			}

			if (!success) 
				throw GenericException("Error while loading '" + mFileName + "'.Element '" + id + "' could not be created",GenericException::FILE_CONFIG_INCORRECT);
			
			//Storing in container
			levelres->surfacesMap[id] = SurfacePointer(newsurface);
			SingletonLogMgr::Instance()->AddNewLine("ResourceManager::_parselevelResources","Resource '" + id + "' loaded",LOGDEBUG);
		}//ELSE IF - FOUND AN ANIMATION
		else if(type ==  "Animation")
		{
			//ID
			std::string id = imagesitr->GetAttribute("Id");
			if(
				id == ""
				||
				levelres->animationsMap.find(id) != levelres->animationsMap.end()
				)
				throw GenericException("Error while loading '" + mFileName + "'. Id incorrect in an element",GenericException::FILE_CONFIG_INCORRECT);
			
			//Resource path
			std::string path = imagesitr->GetAttribute("Path");
			if(path == "")
				throw GenericException("Error while loading '" + mFileName + "'.Element '" + id + "': Path non-existent for an element",GenericException::FILE_CONFIG_INCORRECT);
			
			//Color key
			int keyr(255),keyg(255),keyb(255);
			bool keyed (true);
			//Only way to "catch" non existent attribute...
			try
			{
				imagesitr->GetAttribute("KeyR",&keyr);
				imagesitr->GetAttribute("KeyG",&keyg);
				imagesitr->GetAttribute("KeyB",&keyb);			
			}
			catch (ticpp::Exception&)
			{
				keyed = false;
			}
			if(keyed)
			{
				if(keyr < 0 || keyr > 255 || keyg < 0 || keyg > 255 || keyb < 0 || keyb > 255)
					throw GenericException("Error while loading '" + path + "'.Element '" + id + "': Incorrect colorkey values",GenericException::FILE_CONFIG_INCORRECT);
			}
			

			//Creation of Animation
			AnimationPointer newanimation(new IND_Animation());                          //BY DEFAULT - LOADING WITH ALPHA AND MAX QUALITY

			bool success(false);
			if(keyed)
			{
			   success = SingletonIndieLib::Instance()->AnimationManager->AddToSurface(newanimation.get(),
																			 const_cast<char*>(path.c_str()), //std::String problems with INDIELIB
																			 IND_ALPHA,
																			 IND_32,
																			 static_cast<byte>(keyr),
																			 static_cast<byte>(keyg),
																			 static_cast<byte>(keyb));
			}
			else
			{
			   success = SingletonIndieLib::Instance()->AnimationManager->AddToSurface(newanimation.get(),
																			 const_cast<char*>(path.c_str()), //std::String problems with INDIELIB
																			 IND_ALPHA,
																			 IND_32);
			}

			if(!success)
				throw GenericException("Error while loading '" + mFileName + "'.Element '" + id + "' could not be created",GenericException::FILE_CONFIG_INCORRECT);
			
			//Storing in container
			levelres->animationsMap[id] = AnimationPointer(newanimation);

			SingletonLogMgr::Instance()->AddNewLine("ResourceManager::_parselevelResources","Resource '" + id + "' loaded",LOGDEBUG);
		}//ELSE - INCOHERENT TYPE
		else
		{
			throw GenericException("Error while loading '" + mFileName +"' type '" + type + "' not valid",GenericException::FILE_CONFIG_INCORRECT);
		}	
		//IF
		
	}//LOOP END

	//***************************LOADING OF FONTS**********************************************
	section = levelsitr->FirstChildElement("Fonts");

	ticpp::Iterator<ticpp::Element> fontsitr;
	//LOOP - TiCpp way - Get all elements created in file
	for(fontsitr = fontsitr.begin(section); fontsitr != fontsitr.end(); fontsitr++)
	{	
		//For each element get attributes and create a new surface (IND_Surface)
		//Get the type (only font)
		std::string type;
		fontsitr->GetValue(&type);
		//IF - FOUND A FONT
		if( type == "Font")
		{
			//ID
			std::string id = fontsitr->GetAttribute("Id");
			if(
				id == ""
				||
				levelres->fontsMap.find(id) != levelres->fontsMap.end()
				)
				throw GenericException("Error while loading '" + mFileName + "'. Id incorrect in a font element",GenericException::FILE_CONFIG_INCORRECT);

			//Resource path
			std::string imgpath = fontsitr->GetAttribute("ImagePath");
			std::string confpath = fontsitr->GetAttribute("ConfigPath");
			if(imgpath == ""
				||
			   confpath == "")
				throw GenericException("Error while loading '" + mFileName + "'.Element '" + id + "': Path non-existent for an element",GenericException::FILE_CONFIG_INCORRECT);
			
			//Creation of font
			FontPointer newfont(new IND_Font());            //BY DEFAULT - LOADING WITH ALPHA AND MAX QUALITY
			if(!SingletonIndieLib::Instance()->FontManager->Add(newfont.get(),
																  const_cast<char*>(imgpath.c_str()), //std::String problems with INDIELIB
																  const_cast<char*>(confpath.c_str()), //std::String problems with INDIELIB
																  IND_ALPHA,
																  IND_32))
				throw GenericException("Error while loading '" + mFileName + "'.Element '" + id + "' could not be created",GenericException::FILE_CONFIG_INCORRECT);
			//Storing in container
			levelres->fontsMap[id] = FontPointer(newfont);
			SingletonLogMgr::Instance()->AddNewLine("ResourceManager::_parselevelResources","Resource '" + id + "' loaded",LOGDEBUG);
		}//ELSE - INCOHERENT TYPE
		else
		{
			throw GenericException("Error while loading '" + mFileName +"' type '" + type + "' not valid",GenericException::FILE_CONFIG_INCORRECT);
		}	
		//IF
	}//LOOP END

	//***************************LOADING OF AUDIO**********************************************
	section = levelsitr->FirstChildElement("Audio");
	
	ticpp::Iterator<ticpp::Element> audioitr;
	//LOOP - TiCpp way - Get all elements created in file
	for(audioitr = audioitr.begin(section); audioitr != audioitr.end(); audioitr++)
	{	
		//For each element get attributes and create a new surface (IND_Surface)
		//Get the type of audio (FX or Music)
		std::string type;
		audioitr->GetValue(&type);
		//IF - FOUND A FX (Not streamed audio)
		if( type == "FX")
		{
			//ID
			std::string id = audioitr->GetAttribute("Id");
			//LOOP - Check valid new Id
			bool idfound = false;
			for(AudioresVectorIterator it = levelres->audiovector.begin();
				it != levelres->audiovector.end(); it++)
			{
				if((*it)->GetName() == id)
				{
					idfound = true;
					break;
				}
			}//LOOP END
			
			if(
				id == ""
				||
				idfound
				)
				throw GenericException("Error while loading '" + mFileName + "'. Id incorrect in an audio element",GenericException::FILE_CONFIG_INCORRECT);

			//Resource path
			std::string audiopath = audioitr->GetAttribute("Path");
			if(audiopath == "")
				throw GenericException("Error while loading '" + mFileName + "'.Element '" + id + "': Path non-existent for an element",GenericException::FILE_CONFIG_INCORRECT);
			
			//Loading of sound in sound manager
			if(!SingletonSoundMgr::Instance()->LoadSound(audiopath,id))
				throw GenericException("Error while loading '" + mFileName + "'.Element '" + id + "' could not be loaded",GenericException::FILE_CONFIG_INCORRECT);
			
			//Create a sound resource reference - stream is false
			levelres->audiovector.push_back(SoundResourcePointer(new SoundResource(id,false)));
			
			SingletonLogMgr::Instance()->AddNewLine("ResourceManager::_parselevelResources","Resource '" + id + "' loaded",LOGDEBUG);
		}//ELSE IF - FOUND A STREAM (Streamed audio)
		else if ( type == "Music")
		{

			//ID
			std::string id = audioitr->GetAttribute("Id");
			//LOOP - Check valid new Id
			bool idfound = false;
			for(AudioresVectorIterator it = levelres->audiovector.begin();
				it != levelres->audiovector.end(); it++)
			{
				if((*it)->GetName() == id)
				{
					idfound = true;
					break;
				}
			}//LOOP END
			
			if(
				id == ""
				||
				idfound
				)
				throw GenericException("Error while loading '" + mFileName + "'. Id incorrect in an audio element",GenericException::FILE_CONFIG_INCORRECT);

			//Resource path
			std::string audiopath = audioitr->GetAttribute("Path");
			if(audiopath == "")
				throw GenericException("Error while loading '" + mFileName + "'.Element '" + id + "': Path non-existent for an element",GenericException::FILE_CONFIG_INCORRECT);
			
			//Loading of sound in sound manager
			if(!SingletonSoundMgr::Instance()->LoadSoundStream(audiopath,id))
				throw GenericException("Error while loading '" + mFileName + "'.Element '" + id + "' could not be loaded",GenericException::FILE_CONFIG_INCORRECT);
			
			//Create a sound resource reference - stream is true
			levelres->audiovector.push_back(SoundResourcePointer(new SoundResource(id,true)));
			
			SingletonLogMgr::Instance()->AddNewLine("ResourceManager::_parselevelResources","Resource '" + id + "' loaded",LOGDEBUG);
		}//ELSE - INCOHERENT TYPE
		else
		{
			throw GenericException("Error while loading '" + mFileName +"' type '" + type + "' not valid",GenericException::FILE_CONFIG_INCORRECT);
		}	
		//IF
	}//LOOP END
}

//Release resources of a given level
bool ResourceManager::_releaseLevelResources(const LevelResourcesVec::iterator& leveliterator)
{
	//Get the level info reference (temp copy to facilitate program reading)
	LevelResourcesPointer levelresinfo = (*leveliterator);
	
	//Get Managers pointers
	IndieLibManager *ILib =  SingletonIndieLib::Instance();

	//Dynamic allocated memory release for level
	//Animations resources
	AnimationsMapIterator ait;
	for(ait =levelresinfo->animationsMap.begin(); ait!=levelresinfo->animationsMap.end();ait++)
	{
		if((*ait).second)
		{
			if(!ILib->AnimationManager->Delete((*ait).second.get()))
			{
				SingletonLogMgr::Instance()->AddNewLine("ResourceManager::_releaselevelresources","Failure while deleting resource '" + (*ait).first + "'",LOGEXCEPTION);
			}
			else
			{
				SingletonLogMgr::Instance()->AddNewLine("ResourceManager::_releaselevelresources","Resource '" + (*ait).first + "' released",LOGDEBUG);
			}
		}
	}

	//Surfaces resources (images)
	SurfacesMapIterator sit;
	for(sit = levelresinfo->surfacesMap.begin(); sit!=levelresinfo->surfacesMap.end();sit++)
	{
		if((*sit).second)
		{
			if(!ILib->SurfaceManager->Delete((*sit).second.get()))
			{
				SingletonLogMgr::Instance()->AddNewLine("ResourceManager::_releaselevelresources","Failure while deleting resource '" + (*sit).first + "'",LOGEXCEPTION);
			}
			else
			{
				SingletonLogMgr::Instance()->AddNewLine("ResourceManager::_releaselevelresources","Resource '" + (*sit).first + "' released",LOGDEBUG);
			}
		}
	}

	//Fonts resources
	FontsMapIterator fit;
	for(fit =levelresinfo->fontsMap.begin(); fit!=levelresinfo->fontsMap.end();fit++)
	{
		if((*fit).second)
		{
			if(!ILib->FontManager->Delete((*fit).second.get()))
			{
				SingletonLogMgr::Instance()->AddNewLine("ResourceManager::_releaselevelresources","Failure while deleting resource '" + (*fit).first + "'",LOGEXCEPTION);
			}
			else
			{
				SingletonLogMgr::Instance()->AddNewLine("ResourceManager::_releaselevelresources","Resource '" + (*fit).first + "' released",LOGDEBUG);
			}
		}
	}

	//Audio resources
	AudioresVectorIterator audioit;
	bool failure(false);
	for(audioit =levelresinfo->audiovector.begin(); audioit!=levelresinfo->audiovector.end();audioit++)
	{
		SoundResourcePointer thesoundres = (*audioit);
		if(thesoundres->IsStream())
		{
			//Delete stream
			failure = !SingletonSoundMgr::Instance()->DeleteSoundStream(thesoundres->GetName());
		}
		else
		{
			//Delete sound not streamed
			failure = !SingletonSoundMgr::Instance()->DeleteSound(thesoundres->GetName());

		}

		//Log operation
		if(failure)
		{
			SingletonLogMgr::Instance()->AddNewLine("ResourceManager::_releaselevelresources","Failure while deleting resource '" + (*audioit)->GetName() + "'",LOGEXCEPTION);
		}
		else
		{
			SingletonLogMgr::Instance()->AddNewLine("ResourceManager::_releaselevelresources","Resource '" + (*audioit)->GetName() + "' released",LOGDEBUG);
		}
	}

	return !failure;
}

//Returns if a level was loaded with resources / it also returns position of level resources, if found
bool ResourceManager::_isLevelLoaded(const std::string& levelname, LevelResourcesVec::iterator& foundlevelitr)
{
	bool levelfound = false;
	LevelResourcesVec::iterator itr;
	//LOOP - Search for asked level
	for(itr = mLevelResources.begin(); itr != mLevelResources.end(); itr++)
	{
		if((*itr)->levelname == levelname)
		{
			levelfound = true;
			break;
		}		
	}//LOOP END
	
	//IF - Found resources loaded for level
	if(!levelfound)
	{
		return false;
	}
	else
	{
		//Output the found position of the level inside container
		foundlevelitr = itr;
		return true;
	}
}

//Exception handling
void ResourceManager::_handleException()
{
	//When exception is thrown, it is necessary to unload all resources from IndieLib properly
	SingletonLogMgr::Instance()->AddNewLine("ResourceManager","Exception when loading resources/ Unloading all resources loaded so far...",LOGEXCEPTION);
	_release();
}

//Init resources
void ResourceManager::_init()
{
	//Make sure in construction that WHOLE resources XML file is correct
	std::string filename = g_ConfigOptions.GetScriptsPath() + mFileName;
	mFileDocument.LoadFile(filename);
	//Load initial common resources
	LoadLevelResources(mDefaultLevelName);
}
//Release resources
void ResourceManager::_release()
{
	LevelResourcesVec::iterator levelit;

	//LOOP - Search all levels
	for(levelit = mLevelResources.begin(); levelit != mLevelResources.end(); levelit++)
	{
		//Call internal function to delete resources from level
		_releaseLevelResources(levelit);
	}//LOOP END
}