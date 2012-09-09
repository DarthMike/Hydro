/*
	Filename: LevelBuilder.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a parametric level from an XML file
				 It also saves to xml file the parameters of level
				 Can act as an assets-only builder (level acts only as container)
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _LEVELBUILDER
#define _LEVELBUILDER

//Library dependencies	
#include "boost\shared_ptr.hpp"  //Smart pointer facilites - not using std because it cant go inside an 
								 //STL container as we do in event manager...

#include <string>
#include <sstream>
//Class dependencies
#include "XMLParser.h"
#include "LogManager.h"
#include "GenericException.h"
#include "Shared_Resources.h"
#include "GameLevel.h"

//Forward declarations
class PhysicsSim;  //Main game logic container class
class EditorLogic;	//Editor logic container class

class LevelBuilder
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	//Construct in in-game mode
	LevelBuilder(PhysicsSim* thegamelogic):
    mGameLogicptr(thegamelogic),
	mEditorLogicptr(NULL)
	{}
	//Construct in editor mode
	LevelBuilder(EditorLogic* editorlogic):
    mEditorLogicptr(editorlogic),
	mGameLogicptr(NULL)
	{}
	//Construct as assets only container (no arguments)
	LevelBuilder():
	mEditorLogicptr(NULL),
	mGameLogicptr(NULL)
	{}

	~LevelBuilder()
	{}
	//----- GET/SET FUNCTIONS -----
	GameLevelPointer GetCreatedLevel() { return mLevelPointer; }  //Returns A COPY of the created sprite
	//----- OTHER FUNCTIONS -----
	void LoadLevel(const std::string& filepath, const std::string& levelname, bool assetsonly = false);   //Load a level given the level name
	void SaveLevel(const std::string& filepath, GameLevelPointer thelevel);			//Save a level to file, given the pointer
protected:
	//----- INTERNAL VARIABLES -----
	GameLevelPointer mLevelPointer;//The level pointer
	PhysicsSim* mGameLogicptr;    //Pointer to game logic
	EditorLogic* mEditorLogicptr; //Pointer to editor logic 
	//----- INTERNAL FUNCTIONS -----
	//Builder functions
	void _processParallaxLayer(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath);
	void _processSpriteEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath);
	void _processBodyEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath);
	void _processJointEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath);
	void _processAIEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath);
	void _processPlayerEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath);
	void _processCollectableEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath);
	//Saver functions
	void _saveParallaxLayer(GameLevel::ParallaxListIterator parallaxitr, ticpp::Element* parentxmlnode);
	void _saveSpriteEntity(GameLevel::EntitiesMapIterator entitiesitr, ticpp::Element* parentxmlnode, bool newfile);
	void _saveBodyEntity(const std::string& bodyname, ticpp::Element* parentxmlnode, bool newfile);
	//void _saveJointEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath);
	//void _saveAIEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath);
	//void _savePlayerEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath);
	//void _saveCollectableEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath);
	//Helpers
	bool _getVerticesData(const std::string &verticesdata,float &xvalue, float &yvalue, int vertex);  
	int _getNumberofVertices(const std::string &verticesdata);
	//Exception handling
	void _handleException();
};

#endif