/*
	Filename: LevelBuilder.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a parametric level from an XML file
				 It also saves to xml file the parameters of level
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
#include "LevelBuilder.h"
#include "PhysicsSim.h"
#include "EditorLogic.h"
#include "ResourceManager.h"
#include "AgentsManager.h"
#include "IndieLibManager.h"
#include "PhysicsManager.h"
#include "BlobBuilder.h"
#include "SpriteBuilder.h"
#include "Camera2D.h"
#include "GameLogicDefs.h"

//Load a level given the level name
void LevelBuilder::LoadLevel(const std::string& filepath, const std::string& levelname, bool assetsonly)
{

	//***********LOAD ALL LEVEL ENTITIES FROM LEVEL FILE******************
	//Load all entities from file
	/*Expected configuration file
	Element:    - Parallax: Atts: MaxZoom(number), MinZoom(number)
	                    Element: LAYER
				- Entities
						Element: SPRITE 
 						Element: BODY
						Element: JOINT
						Element: PLAYER
						Element: AI
						Element: COLLECTABLE
						.
						.
						.
	*/
	
	assert(!mLevelPointer); //Assertion not to use the same builder and ovewrite level data!!!
	assert(
		   ((((mGameLogicptr && !mEditorLogicptr)
		   ||
		   (!mGameLogicptr && mEditorLogicptr)))
		   &&
		   !assetsonly)
		   ||
		   assetsonly
		  );  //Assure only one pointer is used (in-game or editor modes, not the same!)

	assert((assetsonly && !mGameLogicptr && !mEditorLogicptr)
		   ||
		   !assetsonly);  //Only assets mode preceded by good builder constructed

	//Open and load document
	ticpp::Document configdoc(filepath);
	configdoc.LoadFile();	//Parsing

	//Creation of level object
	mLevelPointer = GameLevelPointer(new GameLevel(levelname));
	
	//Creation of level - Handle exceptions!
	try
	{
		int tocollect(0);
		if(!assetsonly)
		{
			//----------Get attributes for level-----------
			ticpp::Element* levelsection =  configdoc.FirstChildElement("Level");
			levelsection->GetAttribute("ToCollect",&tocollect);
			//Checkings
			if(tocollect < 0)
				throw GenericException("Failure while reading '" + filepath + "' Attributes for 'Level' are incorrect",GenericException::FILE_CONFIG_INCORRECT);

			//Save parameters to level
			mLevelPointer->mDropsToCollect = tocollect;
				
			//----------Parallax layers creation-----------
			//Get Parallax section
			ticpp::Element* parallaxsection =  configdoc.FirstChildElement("Parallax");


			//Query properties of parallax scroll
			float maxzoom, minzoom, startzoom;
			parallaxsection->GetAttribute("MaxZoom",&maxzoom);
			parallaxsection->GetAttribute("MinZoom",&minzoom);
			parallaxsection->GetAttribute("StartZoom",&startzoom);
			//Check correctness
			if(maxzoom < minzoom || startzoom < minzoom || startzoom > maxzoom || maxzoom < 0 || minzoom < 0 || startzoom < 0 )
				throw GenericException("Failure while reading '" + filepath + "'Parameters MaxZoom, MinZoom, StartZoom,  need to be coherent (max>startzoom>min)! in Parallax section",GenericException::FILE_CONFIG_INCORRECT);
			
			//Set general camera properties for this parallax in level
			SingletonIndieLib::Instance()->GetCamera("General")->SetMaxMinZoom(maxzoom,minzoom);
			SingletonIndieLib::Instance()->GetCamera("General")->SetZoom(startzoom);

			//Query for elements in parallax section
			ticpp::Iterator<ticpp::Element> parallaxitr;  //TiCpp iterator
			//LOOP - Get Layers from XML
			for(parallaxitr = parallaxitr.begin(parallaxsection);parallaxitr != parallaxitr.end();parallaxitr++)
			{
				//Get the type of element
				std::string type;
				parallaxitr->GetValue(&type);
				//Process types of elements in section
				if(type == "Layer") //Parallax Layer type
					_processParallaxLayer(parallaxitr,filepath);	

			}//LOOP END	

			//Once layers are created, sort them by layer number
			GameLevel::ParallaxCompareClass comparefcn;
			mLevelPointer->mParallaxList.sort(comparefcn);
		}
		//----------Entities creation-------------------
		//Control variables
		int collectelements(0);
		//Query for elements in entities sections
		ticpp::Element* entitiessection =  configdoc.FirstChildElement("Entities");
		ticpp::Iterator<ticpp::Element> entsitr;  //TiCpp iterator
		//LOOP - Add entities from XML
		for(entsitr = entsitr.begin(entitiessection);entsitr != entsitr.end();entsitr++)
		{

			//Get the type of entity
			std::string entitytype;
			entsitr->GetValue(&entitytype);
			//IF - Ingame creation
			if(mGameLogicptr)
			{
				//Process differently depending on type
				if(entitytype == "Sprite") //Sprite loading
					_processSpriteEntity(entsitr,filepath);
				else if(entitytype == "Body") //Physic body loading
					_processBodyEntity(entsitr,filepath);
				else if(entitytype == "Joint") //Joint Loading
					_processJointEntity(entsitr,filepath);
				else if(entitytype == "AI") //AI agent Loading
					_processAIEntity(entsitr,filepath);
				else if(entitytype == "Player") //Player agent Loading
					_processPlayerEntity(entsitr,filepath);
				else if(entitytype == "Collectable")  //Collectable agent loading
				{
					_processCollectableEntity(entsitr,filepath);
					collectelements++;
				}
			}//ELSE - Editor creation
			else if(mEditorLogicptr)
			{
				//Only load sprites and bodies to render in editor
				if(entitytype == "Sprite") //Sprite loading
					_processSpriteEntity(entsitr,filepath);
				else if(entitytype == "Body") //Physic body loading
					_processBodyEntity(entsitr,filepath);
			}//ELSE - Assets only
			else if(assetsonly)
			{
				//Only load sprites (level acts as assets containter)
				if(entitytype == "Sprite") //Sprite loading
					_processSpriteEntity(entsitr,filepath);			
			}//ELSE - Should never happen!
			else
			{
				assert(false);   //Something bad happened!
			}
		}//LOOP END
			
		//Level definition checkings
		//IF - Checking of coherence in "to collect" elements created and defined
		if(mGameLogicptr 
		   && 
		   (tocollect != collectelements)
		   &&
		   !assetsonly)
		{
			throw GenericException("Failure while reading '" + filepath + "' Defined elements to collect and created collectable agents mistmatch!! - Review file",GenericException::FILE_CONFIG_INCORRECT);
		}
	}
	catch (std::exception& e) //Catch exceptions
	{
		_handleException();  //Delete in-process created data for builder
		throw(GenericException(e.what(),GenericException::FILE_CONFIG_INCORRECT)); //Rethrow exception
	}
	//**************LEVEL LOADED******************************************
}

//Save a level to file, given the pointer
void LevelBuilder::SaveLevel(const std::string& filepath, GameLevelPointer thelevel)
{
	//***********SAVE ALL LEVEL ENTITIES FROM LEVEL FILE******************
	//Save all entities to file
	/*Expected configuration file
	Element:    - Parallax: Atts: MaxZoom(number), MinZoom(number)
	                    Element: LAYER
	Element: Entities - TYPES: BODY, JOINT, SPRITE, AI, COLLECTABLE, PLAYER
			 Element: SPRITE 
 			 (NOT NOW:) Element: BODY
			 (NOT NOW:) Element: JOINT
			 (NOT NOW:) Element: PLAYER
			 (NOT NOW:) Element: AI
			 (NOT NOW:) Element: COLLECTABLE
						.
						.
						.
	*/

	assert(thelevel);  //Assertion level pointer is not empty

	//Create document object to hold all data
	ticpp::Document thedoc(filepath);
	bool newfile(false);
	//Check if level exists or if level is new file
	try
	{
		thedoc.LoadFile();
	}
	catch(ticpp::Exception&)
	{
		newfile = true;
	}

	//IF - New file creation 
	if(newfile)
	{
		//Append the necessary root elements:
		//----------Header---------------------------
		ticpp::Declaration header("1.0","utf-8","");
		thedoc.InsertEndChild(header);
		//----------Level----------------------------
		ticpp::Element levelsection("Level");
		levelsection.SetAttribute("ToCollect",0);
		//Add this element to document
		thedoc.InsertEndChild(levelsection);
		//----------Parallax layers saving-----------
		//Create new element to hold section
		ticpp::Element parallaxsection("Parallax");

		//Set properties of parallax scroll
		float maxzoom, minzoom;
		SingletonIndieLib::Instance()->GetCamera("General")->GetMaxMinZoom(maxzoom,minzoom);
		parallaxsection.SetAttribute("MaxZoom",maxzoom);
		parallaxsection.SetAttribute("MinZoom",minzoom);

		//Save all elements in parallax section
		GameLevel::ParallaxListIterator itr;
		//LOOP - Get Layers from XML
		for(itr = thelevel->mParallaxList.begin();itr != thelevel->mParallaxList.end();++itr)
		{
			_saveParallaxLayer(itr,&parallaxsection);	
		}//LOOP END	

		//Add this element to document
		thedoc.InsertEndChild(parallaxsection);
	}//IF
		

	//TODO: Entities UPDATING if not new file
	//----------Entities saving-------------------
	//Save elements in entities section
	ticpp::Element* entitiessection;
	if(newfile)
		entitiessection = new ticpp::Element("Entities");
	else
		entitiessection = thedoc.FirstChildElement("Entities");

	GameLevel::EntitiesMapIterator entsitr;
	//LOOP - Add entities to XML
	for(entsitr = thelevel->mEntitiesMap.begin();entsitr != thelevel->mEntitiesMap.end();entsitr++)
	{
		//Save this entity to xml section (animation or image)
		_saveSpriteEntity(entsitr,entitiessection,newfile);
	}//LOOP END
	
	//Search for created physical bodies
	EditorLogic::BodiesList::const_iterator boditr;
	const EditorLogic::BodiesList& bodieslist = mEditorLogicptr->GetCreatedBodiesList();
	//LOOP - Add physical bodies to XML
	for(boditr = bodieslist.begin(); boditr != bodieslist.end(); boditr++)
	{
		//Save this entity to xml section (body)
		_saveBodyEntity((*boditr),entitiessection,newfile);
	}//LOOP END

	//Add this element to document
	if(newfile)
		thedoc.InsertEndChild(*entitiessection);

	//Finally, save and close the file when exiting function
	thedoc.SaveFile();
	//**************LEVEL SAVED*******************************************
}

void LevelBuilder::_processSpriteEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath)
{
	//Assure Id assigned is correct
	//Id
	std::string Id = theentity->GetAttribute("Id");
	if(
		Id == ""
	    ||
	    mLevelPointer->mEntitiesMap.find(Id) != mLevelPointer->mEntitiesMap.end()
	 )
	 throw GenericException("Failure while reading '" + filepath + "'Id '" + Id + "' not correct (repeated or empty)!",GenericException::FILE_CONFIG_INCORRECT);

	//Create builder object for this type and call construction
	SpriteBuilder thebuilder;
	//It can throw exceptions - manage them!
	try
	{
		//Call creation of object
		thebuilder.LoadSprite(theentity.Get());
	}
	catch(GenericException& e)
	{
		//For now, just throw it back with file info
		throw(GenericException("Error loading File '" + filepath +"' :" +e.What_String(),e.GetCode()));
	}		

	//Now add created sprite to container
	mLevelPointer->mEntitiesMap[Id] = thebuilder.GetCreatedSprite().gfxentity;
}

void LevelBuilder::_processBodyEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath)
{
	/*BODY ELEMENT
 	Element: BODY Atts: Id (string) x(number) y(number) h(number) w(number) Rotation(number) Transp(number) FlipX(number) FlipY(number) 
	                    Layer(number) LinDamping(number) AngDamping(number) IsStatic(number) 
						[Material(string)] [Wetness(number)]
			  [Element: Sprite]
			 [Elements(more than one): PolygonShape Atts: Order(number) Density(number) Friction(number) Restitution(number)]
				Value : Comma separated list of points (2xOrder elements) example-> order 4: "0,0,0,5,5,5,5,0"
			 [Elements(more than one): CircleShape Atts: Radius(number) Density(number) Friction(number) Restitution(number)]
				Value : Comma separated list of CENTER POINT  example-> order 4: "0,0"
	*/
	
	//-------Get attributes from entity---------
	//Id
	std::string entId = theentity->GetAttribute("Id");	  //NOTE: Not checking, as it is done in physics Manager
	//Position / size
	float x,y,rotation;
	float lindamping,angdamping;
	bool isstatic;
	std::string material;  //Optional params
	float wetness(0.0f);
	theentity->GetAttribute("x",&x);
	theentity->GetAttribute("y",&y);
	theentity->GetAttribute("Rotation",&rotation);
	theentity->GetAttribute("LinDamping",&lindamping);
	theentity->GetAttribute("AngDamping",&angdamping);
	theentity->GetAttribute("IsStatic",&isstatic);
	theentity->GetAttribute("Material",&material,false);  //Optional parameter
	theentity->GetAttribute("WetNess",&wetness,false);	  //Optional parameter
	if(lindamping < 0 || lindamping > 1 || angdamping < 0 || angdamping > 1 || (isstatic != 0 && isstatic != 1))
		throw GenericException("Failure while reading '" + filepath + "' bad attributes detected! entity'" + entId + "' Out of bounds values",GenericException::FILE_CONFIG_INCORRECT);

	if(material != "" && material != "Wood" && material != "Metal" && material != "Stone")
		throw GenericException("Failure while reading '" + filepath + "' bad material attribute for'" + entId + "' (Must be empty, or: 'Metal', 'Wood', 'Stone'",GenericException::FILE_CONFIG_INCORRECT);

	if(material != "" && (wetness > 1.0f || wetness < 0.0f)) 
		throw GenericException("Failure while reading '" + filepath + "' bad wetness attribute for'" + entId + "' Value in range [0,1]",GenericException::FILE_CONFIG_INCORRECT);
	

	//A BODY entity is quite complicated; it can internally have many sprites, and
	//it can have many shapes associated, and additionally, it MUST HAVE physical properties.
	//First create a new body
	b2BodyDef bodydefinition;
	bodydefinition.allowSleep = true;
	bodydefinition.position = b2Vec2(x,y);  //Position data
	bodydefinition.angle = static_cast<float32>(rotation);
	bodydefinition.isBullet = false;			
	bodydefinition.linearDamping = static_cast<float32>(lindamping);
	bodydefinition.angularDamping = static_cast<float32>(angdamping);
	b2Body* body;
	if(mGameLogicptr)
		body = mGameLogicptr->GetPhysicsManager()->CreateBody(&bodydefinition,entId);
	else if (mEditorLogicptr) 
		body = mEditorLogicptr->GetPhysicsManager()->CreateBody(&bodydefinition,entId);
	else
		body = NULL;

	if(body == NULL)
		throw GenericException("Failure while reading '" + filepath + "' Id '"+ entId +"' not correct! (Repeated or empty)",GenericException::FILE_CONFIG_INCORRECT);
	//------Get elements associated to entity-------
	//Pre-create body params (to store created values in xml)
	SolidBodyPar bodyagentparams;
	ticpp::Iterator <ticpp::Element> entelement;
	//LOOP - Get elements of entity
	for(entelement = entelement.begin(theentity.Get());entelement != entelement.end();entelement++)
	{
		//IF - Entity type is a polygon
		if(entelement->Value() == "PolygonShape")
		{

			//Density, restitution, and Friction
			float32 density, restitution, friction;
			entelement->GetAttribute("Density",&density);
			entelement->GetAttribute("Restitution",&restitution);
			entelement->GetAttribute("Friction",&friction);
			//Order
			int32 order;
			entelement->GetAttribute("Order",&order);
			//Vertices data
			std::string verticesdata = entelement->GetText();
			int parsedvertices = _getNumberofVertices(verticesdata);
			//Checkings
			if(order > b2_maxPolygonVertices || order < 3  || order != parsedvertices/2 || density < 0 || restitution < 0 || friction < 0)
				throw GenericException("Failure while reading '" + filepath + "' bad attributes for PolygonShape element",GenericException::FILE_CONFIG_INCORRECT);

			b2PolygonDef newpolygondef;
			newpolygondef.density = density;
			newpolygondef.restitution = restitution;
			newpolygondef.friction = friction;
			newpolygondef.vertexCount = order;

			//LOOP - Set vertices data of polygon
			for(int i = 0; i<order; i++)
			{
				float x,y;
				if(!_getVerticesData(verticesdata,x,y,i))
					throw GenericException("Error while loading '" + filepath +"' Data invalid for polygon shape definition in entity " + entId,GenericException::FILE_CONFIG_INCORRECT);
				
				newpolygondef.vertices[i].Set(x,y);
			}//LOOP

			//Once all polygon has been define, attach it to body
			if(mGameLogicptr)
				mGameLogicptr->GetPhysicsManager()->CreatePolygonShape(&newpolygondef,entId);
			else if (mEditorLogicptr) 
				mEditorLogicptr->GetPhysicsManager()->CreatePolygonShape(&newpolygondef,entId);

		}//ELSE - FOUND CIRCULAR SHAPE
		else if(entelement->Value() == "CircleShape")
		{
			//Density, restitution, and Friction, and Radius
			float32 density, restitution, friction, radius;
			entelement->GetAttribute("Density",&density);
			entelement->GetAttribute("Restitution",&restitution);
			entelement->GetAttribute("Friction",&friction);
			entelement->GetAttribute("Radius",&radius);

			//Vertices data - IN THIS CASE IS THE CENTER OF CIRCLE IN LOCAL COORDS
			std::string verticesdata = entelement->GetText();
			int parsedvertices = _getNumberofVertices(verticesdata);
			//Checkings
			if( parsedvertices != 2 || density < 0 || restitution < 0 || friction < 0)
				throw GenericException("Failure while reading '" + filepath + "' bad attributes for CircleShape element",GenericException::FILE_CONFIG_INCORRECT);

			b2CircleDef newcircledef;
			newcircledef.density = density;
			newcircledef.restitution = restitution;
			newcircledef.friction = friction;
			newcircledef.radius = radius;
			
			float32 x,y;
			_getVerticesData(verticesdata,x,y,0);
			b2Vec2 pos(x,y);
			newcircledef.localPosition = pos;
			

			//Once all polygon has been define, attach it to body
			if(mGameLogicptr)
				mGameLogicptr->GetPhysicsManager()->CreateCircleShape(&newcircledef,entId);	
			else if (mEditorLogicptr) 
				mEditorLogicptr->GetPhysicsManager()->CreateCircleShape(&newcircledef,entId);	
		
		}//ELSE - FOUND SPRITE ELEMENT
		else if(entelement->Value() == "Sprite")
		{
			//IF - In-game Mode
			if(mGameLogicptr)
			{
				//Build the sprite data from XML
				SpriteBuilder spbuilder;
				std::string spId = entelement->GetAttribute("Id");
				if(
					spId == ""
					||
					mLevelPointer->mEntitiesMap.find(spId) != mLevelPointer->mEntitiesMap.end()
				)
					throw GenericException("Failure while reading '" + filepath + "' Id '"+ spId +"' not correct! (Repeated or empty)",GenericException::FILE_CONFIG_INCORRECT);
				
				spbuilder.LoadSprite(entelement.Get());  //Load sprite
				bodyagentparams.gfxentities.push_front(spbuilder.GetCreatedSprite());  //Store it in body agent definition
			}//IF
		}//ELSE - INCOHERENT TYPE
		else
		{	//Not throwing of exeptions to allow builder to be used in other places!
			/*if(isimage || isfont || isanimation)
				throw GenericException("Error while loading '" + filepath +"' type '" + entelement->Value() + "' repeated! Only one type of this element per entity allowed!",GenericException::FILE_CONFIG_INCORRECT);
			else
				throw GenericException("Error while loading '" + filepath +"' type '" + entelement->Value() + "' not valid",GenericException::FILE_CONFIG_INCORRECT);*/
		}	
		//IF
		
		
	}//LOOP END
	
	//When entity was created, compute mass from shapes if it is a movable body
	if(!isstatic && mGameLogicptr)
		mGameLogicptr->GetPhysicsManager()->GetBody(entId)->SetMassFromShapes();
	else if(!isstatic && mEditorLogicptr)
		mEditorLogicptr->GetPhysicsManager()->GetBody(entId)->SetMassFromShapes();

	//Only in in-game an agent gets created
	if(mGameLogicptr)
	{
		//Finally, create an associated agent to manage this data
		bodyagentparams.physicbody = body;
		bodyagentparams.position = Vector2(x,y);
		bodyagentparams.rotation = rotation;
		bodyagentparams.wetness = wetness;
		//Material
		if(material == "Wood")
			bodyagentparams.material = WOODISH;
		else if(material == "Metal")
			bodyagentparams.material = METALLIC;
		else if(material == "Stone")
			bodyagentparams.material = STONE;
		else
			bodyagentparams.material = GENERIC;
		IAgent* thenewagent = mGameLogicptr->GetAgentsManager()->CreateNewAgent(entId,&bodyagentparams);
		//Double-reference this body to the agent
		body->SetUserData(thenewagent);   //NOTE: CAREFUL WHEN CASTING BACK THIS! IT HAS TO BE STATIC_CAST TO SOLIDBODYAGENT*
		
		SingletonLogMgr::Instance()->AddNewLine("LevelBuilder::_init","Body Agent '" + entId + "' created",LOGDEBUG);
	}//ELSE - Editor mode
	else if(mEditorLogicptr)
	{
		//Append to internal list of editor logic (MORE HACKS)
		mEditorLogicptr->AppendBody(entId); 
	}
}

//Utility function to get the vertices data in float format from a string
void LevelBuilder::_processJointEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath)
{
/*BODY ELEMENT
 	Element: JOINT Atts: Id (string) Type(string) Body1(string) Body2(string) AllowCollision(number) LimitsOn(number)
			 Element Coordinates: 
				Value : Comma separated list of joint points in local body coordinates (body1x, body1y, body2x,body2y)(4 points) example-> "0,0,0,5"
	*/
	
	//-------Get attributes from entity---------
	//Id
	std::string entId = theentity->GetAttribute("Id");
	if(
		entId == ""
	    ||
	    mLevelPointer->mEntitiesMap.find(entId) != mLevelPointer->mEntitiesMap.end()
	 )
		throw GenericException("Failure while reading '" + filepath + "' Id '"+ entId +"' not correct! (Repeated or empty)",GenericException::FILE_CONFIG_INCORRECT);
	//Attached bodies and type
	std::string body1, body2;
	std::string type;
	//Collision allowance
	bool allowcollision;
	//Type and other attributes
	type = theentity->GetAttribute("Type");
	body1 = theentity->GetAttribute("Body1");
	body2 = theentity->GetAttribute("Body2");
	theentity->GetAttribute("AllowCollision",&allowcollision);
	
	
	ticpp::Element* coordselem = theentity->FirstChildElement("Coordinates");
	std::string coords = coordselem->GetText();
	int parsedcoords = _getNumberofVertices(coords);
	//Checkings
	if ((parsedcoords != 4 && type != "Revolute" && type != "Prismatic")||(parsedcoords != 2 && (type == "Revolute" || type == "Prismatic")) )
		throw GenericException("Failure while reading '" + filepath + "' coordinates for joint '"+entId +"' incorrect",GenericException::FILE_CONFIG_INCORRECT);
	if((allowcollision != 0 && allowcollision != 1))
		throw GenericException("Failure while reading '" + filepath + "' bad joint attributes! - Out of bounds values",GenericException::FILE_CONFIG_INCORRECT);		

	//Values for coordinates declaration
	float xb1,xb2,yb1,yb2;
	//Create the joint
	//IF - Distance Joint
	if(type == "Distance")
	{
		//Get additional data
		_getVerticesData(coords,xb1,yb1,0);
		_getVerticesData(coords,xb2,yb2,1);
		b2Vec2 vecp1(xb1,yb1);
		b2Vec2 vecp2(xb2,yb2);
		
		//Joint definition
		b2DistanceJointDef newjointdef;
		newjointdef.collideConnected = allowcollision;
		
		//Joint creation
		if(!mGameLogicptr->GetPhysicsManager()->CreateDistanceJoint(&newjointdef,entId,body1,body2,vecp1,vecp2))
			throw GenericException("Failure while reading '" + filepath + "' joint '"+entId+"' connects non-existing bodies!" ,GenericException::FILE_CONFIG_INCORRECT);		
	}//ELSE - Revolute Joint
	else if(type == "Revolute")
	{	
		//Get additional data
		_getVerticesData(coords,xb1,yb1,0);
		b2Vec2 vecp(xb1,yb1);
		float maxangle,minangle;
		bool limited;
		theentity->GetAttribute("LimitsOn",&limited);
		theentity->GetAttribute("MaxAngle",&maxangle,limited);  //Optional
		theentity->GetAttribute("MinAngle",&minangle,limited);  //Optional
		//Checkings
		if(maxangle == 0 || minangle == 0)
			throw GenericException("Failure while reading '" + filepath + "' joint '"+entId+"' has invalid angle restrictions" ,GenericException::FILE_CONFIG_INCORRECT);	
		
		//Joint definition
		b2RevoluteJointDef newjointdef;
		newjointdef.collideConnected = allowcollision;
		newjointdef.upperAngle = static_cast<float32>(SingletonMath::Instance()->AngleToRadians(maxangle));
		newjointdef.lowerAngle = static_cast<float32>(SingletonMath::Instance()->AngleToRadians(minangle));
		newjointdef.enableLimit = limited;
		//Joint creation
		if(!mGameLogicptr->GetPhysicsManager()->CreateRevoluteJoint(&newjointdef,entId,body1,body2,vecp))
			throw GenericException("Failure while reading '" + filepath + "' joint '"+entId+"' connects non-existing bodies!" ,GenericException::FILE_CONFIG_INCORRECT);	
	}//ELSE - Prismatic Joint
	else if(type == "Prismatic")
	{
		//Get additional data
		ticpp::Element* axis = theentity->FirstChildElement("Axis");
		std::string axisdata = axis->GetText();
		int numcoords = _getNumberofVertices(axisdata);
		//Checkings
		if (numcoords != 2)
			throw GenericException("Failure while reading '" + filepath + "' axis for joint '"+entId +"' incorrect",GenericException::FILE_CONFIG_INCORRECT);
		float axisx,axisy;
		_getVerticesData(axisdata,axisx,axisy,0);
		b2Vec2 vecaxis(axisx,axisy);
		
		//Joint definition
		b2PrismaticJointDef newjointdef;
		newjointdef.collideConnected = allowcollision;
		_getVerticesData(coords,xb1,yb1,0);
		b2Vec2 vecp(xb1,yb1);

		//Joint creation
		if(!mGameLogicptr->GetPhysicsManager()->CreatePrismaticJoint(&newjointdef,entId,body1,body2,vecp,vecaxis))
			throw GenericException("Failure while reading '" + filepath + "' joint '"+entId+"' connects non-existing bodies!" ,GenericException::FILE_CONFIG_INCORRECT);	
	}
	else //ELSE - NOT GOOD TYPE OF JOINT
	{
		throw GenericException("Failure while reading '" + filepath + "' type attribute for joint incorrect!",GenericException::FILE_CONFIG_INCORRECT);
	}

	//Create entity
	/*IND_Entity2d* newentity = new IND_Entity2d;
	SingletonIndieLib::Instance()->Entity2dManager->Add(layer,newentity);
	mLevelPointer->mEntitiesMap[entId] = newentity;*/
	
	//Finally, create an associated agent to manage this data
	/*SolidBodyPar bodyagentparams;
	bodyagentparams.gfxentity = newentity;
	bodyagentparams.physicbody = body;
	bodyagentparams.position = Vector2(x,y);
	bodyagentparams.rotation = rotation;
	mGameLogicptr->GetAgentsManager()->CreateNewAgent(entId,&bodyagentparams);*/

	SingletonLogMgr::Instance()->AddNewLine("LevelBuilder::_init","Joint '" + entId + "' created",LOGDEBUG);
}

void LevelBuilder::_processAIEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath)
{
	/*AI ELEMENT
 	Element: AI Atts: Type (string) Id (string) x(number) y(number) h(number) w(number) Rotation(number) Transp(number) FlipX(number) FlipY(number) Layer(number) LinDamping(number) 
					  AngDamping(number) IsStatic(number) MaxSpeed(number) MaxSteerForce(number)
			 [Element: Image Atts: Id(string) [CollisionPath(string)]]
			 [Element: Font Atts: Id(string) [LineSpacing(number)] [CharSpacing(number)] [Align(string)] Text(string)]
			 [Element: Animation Atts: Id(string)] 
			 [Elements(more than one): PolygonShape Atts: Order(number) Density(number) Friction(number) Restitution(number)]
				Value : Comma separated list of points (2xOrder elements) example-> order 4: "0,0,0,5,5,5,5,0"
			 [Elements(more than one): CircleShape Atts: Radius(number) Density(number) Friction(number) Restitution(number)]
				Value : Comma separated list of CENTER POINT  example-> order 2: "0,0"
	*/
	
	//-------Get attributes from entity---------
	//Id
	std::string entId = theentity->GetAttribute("Id");
	if(
		entId == ""
	    ||
	    mLevelPointer->mEntitiesMap.find(entId) != mLevelPointer->mEntitiesMap.end()
	 )
		throw GenericException("Failure while reading '" + filepath + "' bad Id for AI element",GenericException::FILE_CONFIG_INCORRECT);
	//Type
	std::string aitype = theentity->GetAttribute("Type");
	if(
		aitype == ""
	 )
		throw GenericException("Failure while reading '" + filepath + "' type of AI incorrect in an AI element",GenericException::FILE_CONFIG_INCORRECT);

	//Position / size
	float x,y,h,w,rotation;
	float lindamping,angdamping;
	int layer;
	bool isstatic;
	float maxspeed,steerforce;
	theentity->GetAttribute("x",&x);
	theentity->GetAttribute("y",&y);
	theentity->GetAttribute("h",&h);
	theentity->GetAttribute("w",&w);
	theentity->GetAttribute("Rotation",&rotation);
	theentity->GetAttribute("Layer",&layer);
	theentity->GetAttribute("LinDamping",&lindamping);
	theentity->GetAttribute("AngDamping",&angdamping);
	theentity->GetAttribute("IsStatic",&isstatic);
	theentity->GetAttribute("MaxSpeed",&maxspeed);
	theentity->GetAttribute("MaxSteerForce",&steerforce);
	if(h<=0 || w<=0 || layer<0 || layer > 63 || lindamping < 0 || lindamping > 1 || angdamping < 0 || angdamping > 1 || (isstatic != 0 && isstatic != 1))
		throw GenericException("Failure while reading '" + filepath + "' bad attributes detected! entity'" + entId + "'Out of bounds values",GenericException::FILE_CONFIG_INCORRECT);

	//Build the sprite related data from XML
	SpriteBuilder spbuilder;
	spbuilder.LoadSprite(theentity.Get());
	//mLevelPointer->mEntitiesMap[entId] = spbuilder.GetCreatedSprite();

	//An AI entity is like a body entity, it is quite complicated; as a sprite, it can have ONLY 1 animation, image or font associated, but
	//it can have many shapes associated, and additionally, it MUST HAVE physical properties.
	//First create a new body
	b2BodyDef bodydefinition;
	bodydefinition.allowSleep = true;
	bodydefinition.position = b2Vec2(x,y);  //Position data
	bodydefinition.angle = static_cast<float32>(rotation);
	bodydefinition.linearDamping = static_cast<float32>(lindamping);
	bodydefinition.angularDamping = static_cast<float32>(angdamping);
	bodydefinition.fixedRotation = true;		//AI rotation is controlled, not simulated
	b2Body* body = mGameLogicptr->GetPhysicsManager()->CreateBody(&bodydefinition,entId);

	//------Get elements associated to entity-------
	bool isimage,isfont,isanimation;
	isimage = isfont = isanimation = false; //Trackings for coherent data
	ticpp::Iterator <ticpp::Element> entelement;
	//LOOP - Get elements of entity
	for(entelement = entelement.begin(theentity.Get());entelement != entelement.end();entelement++)
	{
		//IF - Entity type is a polygon
		if(entelement->Value() == "PolygonShape")
		{

			//Density, restitution, and Friction
			float32 density, restitution, friction;
			entelement->GetAttribute("Density",&density);
			entelement->GetAttribute("Restitution",&restitution);
			entelement->GetAttribute("Friction",&friction);
			//Order
			int32 order;
			entelement->GetAttribute("Order",&order);
			//Vertices data
			std::string verticesdata = entelement->GetText();
			int parsedvertices = _getNumberofVertices(verticesdata);
			//Checkings
			if(order > b2_maxPolygonVertices || order < 3  || order != parsedvertices/2 || density <= 0 || restitution < 0 || friction < 0)
				throw GenericException("Failure while reading '" + filepath + "' bad attributes for PolygonShape element",GenericException::FILE_CONFIG_INCORRECT);

			b2PolygonDef newpolygondef;
			newpolygondef.density = density;
			newpolygondef.restitution = restitution;
			newpolygondef.friction = friction;
			newpolygondef.vertexCount = order;
			
			//LOOP - Set vertices data of polygon
			for(int i = 0; i<order; i++)
			{
				float x,y;
				if(!_getVerticesData(verticesdata,x,y,i))
					throw GenericException("Error while loading '" + filepath +"' Data invalid for polygon shape definition in entity " + entId,GenericException::FILE_CONFIG_INCORRECT);
				
				newpolygondef.vertices[i].Set(x,y);
			}//LOOP

			//Once all polygon has been define, attach it to body
			mGameLogicptr->GetPhysicsManager()->CreatePolygonShape(&newpolygondef,entId);
		
		}//ELSE - FOUND CIRCULAR SHAPE
		else if(entelement->Value() == "CircleShape")
		{
			//Density, restitution, and Friction, and Radius
			float32 density, restitution, friction, radius;
			entelement->GetAttribute("Density",&density);
			entelement->GetAttribute("Restitution",&restitution);
			entelement->GetAttribute("Friction",&friction);
			entelement->GetAttribute("Radius",&radius);

			//Vertices data - IN THIS CASE IS THE CENTER OF CIRCLE IN LOCAL COORDS
			std::string verticesdata = entelement->GetText();
			int parsedvertices = _getNumberofVertices(verticesdata);
			//Checkings
			if( parsedvertices != 2 || density <= 0 || restitution < 0 || friction < 0)
				throw GenericException("Failure while reading '" + filepath + "' bad attributes for PolygonShape element",GenericException::FILE_CONFIG_INCORRECT);

			b2CircleDef newcircledef;
			newcircledef.density = density;
			newcircledef.restitution = restitution;
			newcircledef.friction = friction;
			newcircledef.radius = radius;
			
			float32 x,y;
			_getVerticesData(verticesdata,x,y,0);
			b2Vec2 pos(x,y);
			newcircledef.localPosition = pos;
			

			//Once all polygon has been define, attach it to body
			mGameLogicptr->GetPhysicsManager()->CreateCircleShape(&newcircledef,entId);		
		}//ELSE - INCOHERENT TYPE
		else
		{
			//Not throwing of exeptions to allow builder to be used in other places!
			/*if(isimage || isfont || isanimation)
				throw GenericException("Error while loading '" + filepath +"' type '" + entelement->Value() + "' repeated! Only one type of this element per entity allowed!",GenericException::FILE_CONFIG_INCORRECT);
			else
				throw GenericException("Error while loading '" + filepath +"' type '" + entelement->Value() + "' not valid",GenericException::FILE_CONFIG_INCORRECT);*/
		}	
		//IF
		
		
	}//LOOP END
	
	//When entity was created, compute mass from shapes if it is a movable body
	if(!isstatic)
		mGameLogicptr->GetPhysicsManager()->GetBody(entId)->SetMassFromShapes();

	//Finally, create an associated agent to manage this data
	AIAgentPar aiagentparams;
	aiagentparams.gfxentities.push_front(spbuilder.GetCreatedSprite());
	aiagentparams.physicbody = body;
	aiagentparams.position = Vector2(x,y);
	aiagentparams.rotation = rotation;
	aiagentparams.agentAI = aitype;
	aiagentparams.maxlinearvelocity = maxspeed;
	aiagentparams.maxsteerforce = steerforce;
	IAgent* thenewagent = mGameLogicptr->GetAgentsManager()->CreateNewAgent(entId,&aiagentparams);
	//Double-reference this body to the agent
	body->SetUserData(thenewagent);   //NOTE: CAREFUL WHEN CASTING BACK THIS! IT HAS TO BE STATIC_CAST TO IAGENT*

	SingletonLogMgr::Instance()->AddNewLine("LevelBuilder::_init","AI Agent '" + entId + "' created",LOGDEBUG);
}

void LevelBuilder::_processPlayerEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath)
{	/* 
	XML DEFINITION EXPECTED FOR PLAYER ELEMENT 
 	Element: Player Atts: MaxControlForce, MaxSpeed, DamageForce, ThrowingMaxForce (numbers)
					Elements: Blob
						      Sprite
	*/

	//Read attributes
	float maxcontrolforce, maxspeed, destructionspeed, damageforce, throwingforce, damageratio;
	int drawr,drawg,drawb;
	theentity->GetAttribute("MaxControlForce",&maxcontrolforce);
	theentity->GetAttribute("MaxSpeed",&maxspeed);
	theentity->GetAttribute("DestructionSpeed",&destructionspeed);
	theentity->GetAttribute("DamageForce",&damageforce);
	theentity->GetAttribute("ThrowingMaxForce",&throwingforce);
	theentity->GetAttribute("DamageScaleRatio",&damageratio);
	theentity->GetAttribute("DrawColorR",&drawr);
	theentity->GetAttribute("DrawColorG",&drawg);
	theentity->GetAttribute("DrawColorB",&drawb);
	ColorRGBA drawcolor (drawr,drawg,drawb,255);
	
	//Find Sprite and build it
	ticpp::Element* child = theentity->FirstChildElement("Sprite"); 

	//Assure Id assigned is correct
	//Id
	std::string spriteId = child->GetAttribute("Id");
	if(
		spriteId == ""
	    ||
	    mLevelPointer->mEntitiesMap.find(spriteId) != mLevelPointer->mEntitiesMap.end()
	 )
	 throw GenericException("Failure while reading '" + filepath + "'Id '" + spriteId + "' not correct (repeated or empty)!",GenericException::FILE_CONFIG_INCORRECT);
	
	SpriteBuilder thespritebuilder;
	//It can throw exceptions - manage them!
	try
	{	
		//Call creation of object
		thespritebuilder.LoadSprite(child);
	}
	catch(GenericException& e)
	{
		//For now, just throw it back with file info
		throw(GenericException("Error loading File '" + filepath +"' :" +e.What_String(),e.GetCode()));
	}

	//Add sprite to entities map
	//mLevelPointer->mEntitiesMap[spriteId] = thespritebuilder.GetCreatedSprite();

	//Create the player agent for this blob
	PlayerAgentPar playeragentparams;
	playeragentparams.position = Vector2(0,0);
	playeragentparams.rotation = 0.0f;
	playeragentparams.sprite = thespritebuilder.GetCreatedSprite();
	playeragentparams.maxcontrolforce = maxcontrolforce;
	playeragentparams.maxspeed =  maxspeed;
	playeragentparams.destructionspeed = destructionspeed;
	playeragentparams.damageforce = damageforce;
	playeragentparams.throwingforce = throwingforce;
	playeragentparams.damageratio = damageratio;
	playeragentparams.drawcolor = SingletonIndieLib::Instance()->FromRGBToHSL(drawcolor);
	playeragentparams.originaldrawcolor = SingletonIndieLib::Instance()->FromRGBToHSL(drawcolor);
	IAgent* thenewagent = mGameLogicptr->GetAgentsManager()->CreateNewAgent("Player",&playeragentparams);

	//Find Blob
	child = theentity->FirstChildElement("Blob");
	PhysicsManagerPointer physicsptr = mGameLogicptr->GetPhysicsManager();
	BlobBuilder theblobbuilder(physicsptr,thenewagent);

	//It can throw exceptions - manage them!
	try
	{
		//Call creation of object
		theblobbuilder.LoadBlob(child);
	}
	catch(GenericException& e)
	{
		//For now, just throw it back with file info
		throw(GenericException("Error loading File '" + filepath +"' :" +e.What_String(),e.GetCode()));
	}			

	//Now merge both elements together
	//Nasty dynamic cast workaround!
	PlayerAgent* theplayeragent = dynamic_cast<PlayerAgent*>(thenewagent);
	theplayeragent->SetBlobController(theblobbuilder.GetBlobController());
}
	
void LevelBuilder::_processCollectableEntity(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath)
{
	/*COLLECTABLE ELEMENT
 	Element: COLLECTABLE Atts: Id (string) x(number) y(number) h(number) w(number) Rotation(number) Transp(number) FlipX(number) FlipY(number) Layer(number)
			 [Element: Image Atts: Id(string) [CollisionPath(string)]]
			 [Element: Font Atts: Id(string) [LineSpacing(number)] [CharSpacing(number)] [Align(string)] Text(string)]
			 [Element: Animation Atts: Id(string)] 
			 Element: CircleShape Atts: Radius(number) Density(number) Friction(number) Restitution(number)
				Value : Comma separated list of CENTER POINT  example-> order 4: "0,0"
	*/
	
	//-------Get attributes from entity---------
	//Id
	std::string entId = theentity->GetAttribute("Id");
	if(
		entId == ""
	    ||
	    mLevelPointer->mEntitiesMap.find(entId) != mLevelPointer->mEntitiesMap.end()
	 )
		throw GenericException("Failure while reading '" + filepath + "'Id '" + entId + "' not correct (repeated or empty)!",GenericException::FILE_CONFIG_INCORRECT);
	
	//Position / size
	float x,y,h,w,rotation;
	int layer;
	theentity->GetAttribute("x",&x);
	theentity->GetAttribute("y",&y);
	theentity->GetAttribute("h",&h);
	theentity->GetAttribute("w",&w);
	theentity->GetAttribute("Rotation",&rotation);
	theentity->GetAttribute("Layer",&layer);
	if(h<=0 || w<=0 || layer<0 || layer > 63)
		throw GenericException("Failure while reading '" + filepath + "' bad attributes detected! entity'" + entId + "' Out of bounds values",GenericException::FILE_CONFIG_INCORRECT);

	//Build the sprite data from XML
	SpriteBuilder spbuilder;
	//IF -  There is any sprite loaded
	if(theentity->FirstChildElement("Image",false)
	   ||
	   theentity->FirstChildElement("Animation",false)
	   ||
	   theentity->FirstChildElement("Font",false))
	{
		spbuilder.LoadSprite(theentity.Get());
	}
	else
		GenericException("Failure while reading '" + filepath + " Element '" + entId + "' Should have an Animation, Image or Font associated!",GenericException::FILE_CONFIG_INCORRECT);
	
	//A COLLECTABLE is like a body, but it has no shapes but a bounding circle or rectangle, which is sensor
	b2BodyDef bodydefinition;
	bodydefinition.allowSleep = true;
	bodydefinition.position = b2Vec2(x,y);  //Position data
	bodydefinition.angle = static_cast<float32>(rotation);
	b2Body* body = mGameLogicptr->GetPhysicsManager()->CreateBody(&bodydefinition,entId);

	//------Get elements associated to entity-------
	ticpp::Iterator <ticpp::Element> entelement;
	bool iscircle, ispolygon;
	iscircle = ispolygon = false;
	//LOOP - Get elements of entity
	for(entelement = entelement.begin(theentity.Get());entelement != entelement.end();entelement++)
	{
		//IF - Entity type is a polygon
		if(entelement->Value() == "PolygonShape" && !iscircle &&!ispolygon)
		{

			//Density, restitution, and Friction - NOT READ (SENSOR)
			//float32 density, restitution, friction;
			//entelement->GetAttribute("Density",&density);
			//entelement->GetAttribute("Restitution",&restitution);
			//entelement->GetAttribute("Friction",&friction);
			//Order
			int32 order;
			entelement->GetAttribute("Order",&order);
			//Vertices data
			std::string verticesdata = entelement->GetText();
			int parsedvertices = _getNumberofVertices(verticesdata);
			//Checkings
			//if(order > b2_maxPolygonVertices || order < 3  || order != parsedvertices/2 || density <= 0 || restitution < 0 || friction < 0)
			if(order > b2_maxPolygonVertices || order < 3  || order != parsedvertices/2)
				throw GenericException("Failure while reading '" + filepath + "' bad attributes for PolygonShape element",GenericException::FILE_CONFIG_INCORRECT);

			b2PolygonDef newpolygondef;
			//newpolygondef.density = density;
			//newpolygondef.restitution = restitution;
			//newpolygondef.friction = friction;
			newpolygondef.vertexCount = order;
			newpolygondef.isSensor =  true;			//NOTE THAT IS SENSOR
			
			//LOOP - Set vertices data of polygon
			for(int i = 0; i<order; i++)
			{
				float x,y;
				if(!_getVerticesData(verticesdata,x,y,i))
					throw GenericException("Error while loading '" + filepath +"' Data invalid for polygon shape definition in entity " + entId,GenericException::FILE_CONFIG_INCORRECT);
				
				newpolygondef.vertices[i].Set(x,y);
			}//LOOP

			//Once all polygon has been define, attach it to body
			mGameLogicptr->GetPhysicsManager()->CreatePolygonShape(&newpolygondef,entId);

			ispolygon = true;
		
		}//ELSE - FOUND CIRCULAR SHAPE
		else if(entelement->Value() == "CircleShape"&& !iscircle &&!ispolygon)
		{
			//Density, restitution, and Friction, and Radius - NOT READ (SENSOR)
			//float32 density, restitution, friction;
			float32 radius;
			//entelement->GetAttribute("Density",&density);
			//entelement->GetAttribute("Restitution",&restitution);
			//entelement->GetAttribute("Friction",&friction);
			entelement->GetAttribute("Radius",&radius);

			//Vertices data - IN THIS CASE IS THE CENTER OF CIRCLE IN LOCAL COORDS
			std::string verticesdata = entelement->GetText();
			int parsedvertices = _getNumberofVertices(verticesdata);
			//Checkings
			//if( parsedvertices != 2 || density <= 0 || restitution < 0 || friction < 0)
			if( parsedvertices != 2)
				throw GenericException("Failure while reading '" + filepath + "' bad attributes for PolygonShape element",GenericException::FILE_CONFIG_INCORRECT);

			b2CircleDef newcircledef;
			//newcircledef.density = density;
			//newcircledef.restitution = restitution;
			//newcircledef.friction = friction;
			newcircledef.radius = radius;
			newcircledef.isSensor =  true;			//NOTE THAT IS SENSOR
			
			float32 x,y;
			_getVerticesData(verticesdata,x,y,0);
			b2Vec2 pos(x,y);
			newcircledef.localPosition = pos;
			
			//Once all polygon has been define, attach it to body
			mGameLogicptr->GetPhysicsManager()->CreateCircleShape(&newcircledef,entId);	

			iscircle = true;
		
		}//ELSE - INCOHERENT TYPE
		else
		{	//Not throwing of exeptions to allow builder to be used in other places!
			/*if(isimage || isfont || isanimation)
				throw GenericException("Error while loading '" + filepath +"' type '" + entelement->Value() + "' repeated! Only one type of this element per entity allowed!",GenericException::FILE_CONFIG_INCORRECT);
			else
				throw GenericException("Error while loading '" + filepath +"' type '" + entelement->Value() + "' not valid",GenericException::FILE_CONFIG_INCORRECT);*/
		}	
		//IF
		
		
	}//LOOP END
	
	//When entity was created, compute mass from shapes if it is a movable body
	//A collectable agent IS STATIC

	//Finally, create an associated agent to manage this data
	CollectableAgentPar collectableagentparams;
	collectableagentparams.sprite = spbuilder.GetCreatedSprite();  //NOTE: It can be empty!!
	collectableagentparams.physicbody = body;
	collectableagentparams.position = Vector2(x,y);
	collectableagentparams.rotation = rotation;
	IAgent* thenewagent = mGameLogicptr->GetAgentsManager()->CreateNewAgent(entId,&collectableagentparams);
	//Double-reference this body to the agent
	body->SetUserData(thenewagent);   //NOTE: CAREFUL WHEN CASTING BACK THIS! IT HAS TO BE STATIC_CAST TO SOLIDBODYAGENT*

	SingletonLogMgr::Instance()->AddNewLine("LevelBuilder::_init","Collectable Agent '" + entId + "' created",LOGDEBUG);
}

void LevelBuilder::_processParallaxLayer(ticpp::Iterator<ticpp::Element> theentity, const std::string &filepath)
{
	/*LAYER:
		Layer: Atts: MaxZoom(number), MinZoom(number)
						  Element : Layer Atts: Number(number) SpeedPercentX, SpeedPercentY, ZoomPercent (numbers) 
												StartPosX(number), StartPosY(number)	
										  Element: SPRITE
	*/

	//Get attributes from parallax layer
	//Id
	int layernumber;
	float speedpercentx,speedpercenty;
	float startx, starty;
	float zoompercent;
	theentity->GetAttribute("Number",&layernumber);
	theentity->GetAttribute("SpeedPercentX",&speedpercentx);
	theentity->GetAttribute("SpeedPercentY",&speedpercenty);
	theentity->GetAttribute("ZoomPercent",&zoompercent);
	theentity->GetAttribute("StartPosX",&startx);
	theentity->GetAttribute("StartPosY",&starty);

	//Check correctness of input parameters (we will suppose startx and starty are reasonable...)
	if(speedpercentx < 0.0f || speedpercenty < 0.0f || layernumber < 0)
		throw GenericException("Failure while reading '" + filepath + "' An attribute in a parallax layer is incorrect",GenericException::FILE_CONFIG_INCORRECT);
	
	//See if this layer number was defined before in this level
	GameLevel::ParallaxListIterator it;
	//LOOP - Check there is no repetition of layer
	for(it = mLevelPointer->mParallaxList.begin(); it != mLevelPointer->mParallaxList.end(); it++)
	{
		if(	(*it).thecamera->GetLayer() == layernumber )
			throw GenericException("Failure while reading '" + filepath + "' Layer numbers repeated in parallax definition!!",GenericException::FILE_CONFIG_INCORRECT);
	}//LOOP END
	
	//-----Create camera----
	std::stringstream cameraname;
	cameraname<<"Layer"<<layernumber;
	Camera2DPointer thecamera = SingletonIndieLib::Instance()->RegisterCamera(cameraname.str(),1.0,Vector2(startx,starty),layernumber);
	thecamera->SetLimitingMoves(false);		//Dont limit moves by AABB (it is a parallax, only visual)
	//Create parallax info
	GameLevel::ParallaxInfo params;
	params.relvelocity = Vector2(speedpercentx,speedpercenty) / 100;
	params.relzoom = zoompercent;
	params.thecamera = thecamera;
	params.startpos =  Vector2(startx,starty);

	//Insert it at the end of list and re-sort it by layer number
	mLevelPointer->mParallaxList.push_front(params);
}


void LevelBuilder::_saveParallaxLayer(GameLevel::ParallaxListIterator parallaxitr, ticpp::Element* parentxmlnode)
{
	/*LAYER:
		Layer: Atts: MaxZoom(number), MinZoom(number)
						  Element : Layer Atts: Number(number) SpeedPercentX, SpeedPercentY, ZoomPercent (numbers) 
												StartPosX(number), StartPosY(number)	
										  Element: SPRITE
	*/

	//Create a new layer element
	ticpp::Element thelayer("Layer");

	//Set attributes from parallax layer
	int layernumber = (*parallaxitr).thecamera->GetLayer();
	Vector2 speedpercent = (*parallaxitr).relvelocity;
	float zoompercent = (*parallaxitr).relzoom;
	Vector2 startpos = (*parallaxitr).startpos;
	thelayer.SetAttribute("Number",layernumber);
	thelayer.SetAttribute("SpeedPercentX",speedpercent.x * 100);
	thelayer.SetAttribute("SpeedPercentY",speedpercent.y * 100);
	thelayer.SetAttribute("ZoomPercent",zoompercent);
	thelayer.SetAttribute("StartPosX",startpos.x);
	thelayer.SetAttribute("StartPosY",startpos.y);
	
	//Finally, append all info to parent node
	parentxmlnode->InsertEndChild(thelayer);
}

void LevelBuilder::_saveSpriteEntity(GameLevel::EntitiesMapIterator entitiesitr, ticpp::Element* parentxmlnode, bool newfile)
{	
	//Get element Id
	std::string id = (*entitiesitr).first;
	
	//IF - Not New file
	if(!newfile)	
	{
		//Search for this element in file, and remove it if it exists
		//Query for elements in entities sections
		ticpp::Iterator<ticpp::Element> itr;  //TiCpp iterator
		//LOOP - Search for an element with same ID
		for(itr = itr.begin(parentxmlnode);itr != itr.end();++itr)
		{
			//Get type (value) of entity
			std::string type;
			(*itr).GetValue(&type);
			//IF - Id and type are the same: Entity found
			if((*itr).GetAttribute("Id") == id
				&&
				type == "Sprite")
			{
				//Remove found node
				parentxmlnode->RemoveChild(&(*itr));
				break;
			}//IF
		}//LOOP END
	}//IF

	ticpp::Element spriteelement("Sprite");
	spriteelement.SetAttribute("Id",id);
	
	//Use builder object to save parameters of the given sprite
	SpriteBuilder spbuilder;
	spbuilder.SaveSprite((*entitiesitr).second,spriteelement);

	//Append it to layer element (If new element or new file)
	parentxmlnode->InsertEndChild(spriteelement);
}

void LevelBuilder::_saveBodyEntity(const std::string& bodyname, ticpp::Element* parentxmlnode, bool newfile)
{
	//IF - Not New file
	if(!newfile)	
	{
		//Search for this element in file, and remove it if it exists
		//Query for elements in entities sections
		ticpp::Iterator<ticpp::Element> itr;  //TiCpp iterator
		//LOOP - Search for an element with same ID
		for(itr = itr.begin(parentxmlnode);itr != itr.end();++itr)
		{
			//Get type (value) of entity
			std::string type;
			(*itr).GetValue(&type);
			//IF - Id and type are the same: Entity found
			if((*itr).GetAttribute("Id") == bodyname
				&&
				type == "Body")
			{
				//Remove found node
				parentxmlnode->RemoveChild(&(*itr));
				break;
			}//IF
		}//LOOP END
	}//IF

	ticpp::Element bodyelement("Body");
	//Set Id
	bodyelement.SetAttribute("Id",bodyname);
	//Get the body from physics manager by name
	PhysicsManagerPointer physicsptr = mEditorLogicptr->GetPhysicsManager();
	b2Body* body = physicsptr->GetBody(bodyname);
	
	//Store attributes relevant to body
	b2Vec2 coordpos = body->GetPosition();
	float32 angle = body->GetAngle();
	bool isstatic = body->IsStatic();
	float angdamping = body->GetAngularDamping();
	float lindamping = body->GetLinearDamping();
	bodyelement.SetAttribute("x",coordpos.x);
	bodyelement.SetAttribute("y",coordpos.y);
	bodyelement.SetAttribute("Rotation",angle);
	bodyelement.SetAttribute("LinDamping",lindamping);
	bodyelement.SetAttribute("AngDamping",angdamping);
	bodyelement.SetAttribute("IsStatic",isstatic);
	
	//Store shape values
	b2Shape* relatedshape = body->GetShapeList();
	assert(relatedshape);   //At least one shape!
	//LOOP - Search all shapes of body
	while(relatedshape)
	{
		float density = relatedshape->GetDensity();
		float friction = relatedshape->GetFriction();
		float restitution = relatedshape->GetRestitution();

		//Dissert between shape types to store values
		switch(relatedshape->GetType())
		{
			case(e_polygonShape):
				{
					//Read remaining values of data
					b2PolygonShape* polygonshape = static_cast<b2PolygonShape*>(relatedshape);
					int32 vertexcount = polygonshape->GetVertexCount();
					const b2Vec2* vertices = polygonshape->GetVertices();
					std::stringstream ss;
					//LOOP - Store vertex data in string
					for (int32 i = 0; i < vertexcount; i++)
					{
						if(i < vertexcount - 1)
							ss<<vertices[i].x<<","<<vertices[i].y<<",";
						else	
							ss<<vertices[i].x<<","<<vertices[i].y;
					}//LOOP 

					//Create ticpp element and relate values and text
					ticpp::Element polshapeelement("PolygonShape");
					polshapeelement.SetAttribute("Order",vertexcount);
					polshapeelement.SetAttribute("Density",density);
					polshapeelement.SetAttribute("Friction",friction);
					polshapeelement.SetAttribute("Restitution",restitution);
					polshapeelement.SetText(ss.str());
					//Append shape
					bodyelement.InsertEndChild(polshapeelement);
				}
				break;
			case(e_circleShape):
				{
					//Read remaining values of data
					b2CircleShape* circleshape = static_cast<b2CircleShape*>(relatedshape);
					float32 radius = circleshape->GetRadius();
					b2Vec2 localpos = circleshape->GetLocalPosition();
					std::stringstream ss;
					ss<<localpos.x<<","<<localpos.y;
					//Create ticpp element and relate values and text
					ticpp::Element circshapeelement("CircleShape");
					circshapeelement.SetAttribute("Radius",radius);
					circshapeelement.SetAttribute("Density",density);
					circshapeelement.SetAttribute("Friction",friction);
					circshapeelement.SetAttribute("Restitution",restitution);
					circshapeelement.SetText(ss.str());
					//Append shape
					bodyelement.InsertEndChild(circshapeelement);
				}
				break;
			default:
				{
					assert(false);		//NEVER!
				}
				break;
		}//SWITCH

		//Get next shape
		relatedshape = relatedshape->GetNext();
	}//LOOP END
	
	//Append body to parent element
	parentxmlnode->InsertEndChild(bodyelement);	
}

//Utility function to get the vertices data in float format from a string
bool LevelBuilder::_getVerticesData(const std::string &verticesdata,float &xvalue, float &yvalue, int vertex)
{
	int number = 0;
	unsigned int startposition = 0;
	unsigned int nextposition = 0;
	unsigned int position = 0;

	//IF - First vertex
	if(vertex == 0)
	{
		startposition = 0;
		nextposition = verticesdata.find(",");
		//Check found
		if(nextposition == std::string::npos)
			return false;
		else
			nextposition++;
	}//ELSE - Not first vertex
	else
	{
		//LOOP - SEARCH FOR SEPARATOR CHARACTER
		while(position < verticesdata.size())
		{
			//Find an ocurrence of separator character in whole string remaining
			position = verticesdata.find(",",position+1,1);
			//Check found
			if(position != std::string::npos)
				number ++; //Increment num of vertices found
 			else
				return false;

			//IF - Found the vertex asked for (x coord)
			if(static_cast<float>(number/2) == static_cast<float>(vertex))
			//if(number%vertex == 0)
			{
				startposition = position+1;
				nextposition = verticesdata.find(",",startposition,1);
				nextposition++;
				break;
			}
		}//LOOP END
	}
	
	//Check vertices where found
	if(startposition == 0 && nextposition == 0)
		return false;

	//DEBUG ASSERT
	assert(startposition < verticesdata.size() && nextposition < verticesdata.size());

	//From positions inside c-strings, get float data of container data
	xvalue = static_cast<float>(atof(verticesdata.substr(startposition,nextposition-1-startposition).c_str()));
	yvalue = static_cast<float>(atof(verticesdata.substr(nextposition).c_str()));

	return true;
}

//Utility function to parse string and find separator characters to see how many numbers are stored in between
int LevelBuilder::_getNumberofVertices(const std::string &verticesdata)
{
	int number = 0;
	unsigned int position = 0;

	//LOOP - SEARCH A STRING FOR SEPARATOR CHARACTER, UPDATE COUNT OF FOUND
	while(position < verticesdata.size() && position != std::string::npos)
	{
		//Find an ocurrence of separator character in whole string remaining
		position = verticesdata.find(",",position+1,1);
		if(position != std::string::npos)
			number ++;
	}//LOOP END

	return(number + 1);
}

void LevelBuilder::_handleException()
{
	//When exception is thrown, it is necessary to unload all resources from IndieLib properly
	SingletonLogMgr::Instance()->AddNewLine("LevelBuilder","Exception when loading level/ Unloading all entities loaded so far...",LOGEXCEPTION);
	if(mLevelPointer)
		mLevelPointer->_release(); //Call release of level
}
