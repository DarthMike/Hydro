/*
	Filename: EditorLogic.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Editor for my game
	Comments: Quite "not user friendly" but it is for internal use and speed up development, also for learning
	Attribution: Based on Javier Lopez Lopez and his great work in gametuto.com (tutorial of C++ editor)
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _EDITORLOGIC
#define _EDITORLOGIC
/*NOTE:
	I DIDNT WANT TO USE EVENTS WITH EDITOR BECAUSE YOU NEED TO CREATE CLASSES AND STRUCTURES TO
	STORE SENT MESSAGES. I DIDNT WANT TO "POLLUTE" EVENT CLASSES AND STRUCTURES WITH EDITOR-RELATED
	ONES. INSTEAD, ALL COMMANDS FROM INPUTS ARE HANDLED THE 'TRADITIONAL' WAY WITH PUBLIC METHODS AND 
	INTERNAL BOOLEAN VARIABLES. IT IS CLEANER THIS WAY FOR ME TO SEPARATE AS MUCH AS POSSIBLE EDITOR AND
	IN-GAME CODE.

	ANOTHER HACK: USE OF SPECIAL CONTAINER TO TAKE NAMES OF BODIES IN A WAY THAT FOR THE BUILDER OBJECT 
	IT IS EASY TO SAVE. IT IS DONE LIKE THAT TO GET THE WORK DONE,AS MY PRIMARY CONCERN IS NOT TO "POLLUTE"
	GAME CODE WITH EDITOR CODE, AND HAVING EDITING AND GAME MODES EVERYWHERE. IT ONLY BRINGS PROBLEMS FOR
	THE SMALL EDITOR I WANT TO MAKE. THE PHYSICS MANAGER SHOULD DELIVER THE BODIES LIST OR MAP, BUT
	I DONT WANT TO ADD IT THERE JUST FOR EDITOR. THIS EDITOR CODE IS QUITE HACKY...
*/

//Library dependencies
#include <sstream>
//Class dependencies
#include "General_Resources.h"
#include "Shared_Resources.h"

#include "ResourceManager.h"
#include "GameLevel.h"

//Forward declarations
class EditorMouse;
class EditorKeyBoard;
class EditorLogicListener;
class b2Body;

class EditorLogic
{
	//Definitions
public:
	typedef struct EditBrush  //Edition brush - Where we store the to-insert info
	{
		//CONSTRUCTOR
		EditBrush():
		mInsertionType(0),
		spriteId("Image0"),
		resourceId(""),
		startdefpoint(0,0),
		enddefpoint(0,0),
		shapetype(0),
		addtobody(false),
		currentbody("")
		{}

		int mInsertionType;  //Type to insert (0 - Surface, 1 - Animation, 2 - Body)
		//Information about a sprite
		SpritePointer currentsprite;
		std::string spriteId;
		std::string resourceId;
		//Information about a physic body
		Vector2 startdefpoint;
		Vector2 enddefpoint;
		int shapetype;		//Shape for body to insert: (0 - Square)
		bool addtobody;
		std::string currentbody;
	}EditBrush;

	typedef struct EditedElement //Edited sprite information
	{
		//Sprite information
		SpritePointer sprite;
		//Body information
		b2Body* body;
		std::string name;
		SpritePointer infotextsprite;
		std::string infotext;
	}EditedElement;

	typedef std::list<std::string> BodiesList;    //A container of created bodies (only names)
	typedef BodiesList::iterator	BodiesListIterator;

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	EditorLogic():
	  mEditorMouse(NULL), 
	  mEditorKeyBoard(NULL),
	  mListener(NULL),
	  mCurrentEditingLayer(10),
	  mInitialPosX(0.0f),
	  mInitialPosY(0.0f),
	  mFullInfoText(false),
	  mShowSpInfoText(false),
	  mChangeLayer(false),
	  mMovement(false),
	  mZoomIn(false),
	  mZoomOut(false),
	  mIncreaseTileSurf(false),
	  mDecreaseTileSurf(false),
	  mChangeInsertionType(false),
	  mMoveTile(false),
	  mMoveTileExact(false),
	  mRotateCW(false),
	  mScaleTileUp(false),
	  mScaleTileDown(false),
	  mScaleTileX(false),
	  mScaleTileY(false),
	  mVertFlip(false),
	  mHorFlip(false),
	  mIncreaseTransp(false),
	  mDecreaseTransp(false),
	  mIncreaseZ(false),
	  mDecreaseZ(false),
	  mResetRotation(false),
	  mResetScale(false),
	  mToggleWrap (false),
	  mWrapTileUp(false),
	  mWrapTileDown(false),
	  mWrapTileX(false),
	  mWrapTileY(false),
	  mInsertTile(false),
	  mDeleteTile(false),
	  mCloneTile(false),
	  mDropCloned(false),
	  mDefineBody(false),
	  mDefiningBody(false),
	  mToggleBodiesEditMode(false),
	  mChangeAddingBody(false),
	  mChangeInsertShape(false),
	  mDrawPhysicsElements(false)
	{
		_init();
	}
	~EditorLogic()
	{
		_release();
	}

	//----- GET/SET FUNCTIONS -----
	Camera2DPointer GetCurrentRenderCamera() { return mCurrentCamera; }  //Get current editing camera pointer
	SpritePointer GetEditBrush() { return mEditingBrush.currentsprite; } //Get editing brush pointer
	SpritePointer GetCollidedInfo() { return mCurrentCollided.infotextsprite; } //Get collided info sprite pointer
	const BodiesList& GetCreatedBodiesList() { return mBodiesList; }			//Get created bodies list (names)
	PhysicsManagerPointer GetPhysicsManager() { return mPhysicsMgr; }  //Get physics manager
	//----- OTHER FUNCTIONS -----
	//Periodic Update and render
	void UpdateState(float dt);
	void RenderScene();
	
	//Creation helper function
	void AppendBody(const std::string& name);

	//Handle functions for input to call
	void LoadLevelCommand();	//Open - New - Load Levels commands
	bool SaveLevelCommand();
	void NewLevelCommand();

	void ToggleInfoCommand();		//Toggle all commands info
	void ToggleSpriteInfoCommand(); //Toggle sprite info command

	void IncreaseEditLayerCommand(); //Current layer command
	void DecreaseEditLayerCommand();
	
	void ZoomInCommand();	//Movement and Zoom Commands
	void ZoomOutCommand();
	void MoveCommand(const Vector2& direction);

	void IncreaseDropTileCommand();	//Brush commands 
	void DecreaseDropTileCommand();	
	void ChangeInsertionTypeCommand(int type); //Change insertion type

	void MoveTileCommand(bool activate); //Tile manipulation commands
	void MoveTileExactCommand(const Vector2& direction);  
	void RotateTileCommand(bool rotate); //Note: it is also used for bodies
	void ScaleTileCommand(bool updown, bool active, bool x=true, bool y=true);
	void FlipTileHorCommand();			
	void FlipTileVerCommand();
	void IncreaseTileTranspCommand();
	void DecreaseTileTranspCommand();
	void IncreaseTileZCommand();
	void DecreaseTileZCommand();
	void ResetTileRotationCommand(); //Note: it is also used for bodies
	void ResetTileScaleCommand();
	void ToggleWrapCommand();
	void WrapSectionChangeCommand(bool updown, bool active,bool x=true, bool y=true);
	void DropTileCommand();				//Note: it is also used for bodies
	void CloneTileCommand();			//Note: it is also used for bodies
	void DropClonedTileCommand();		//Note: it is also used for bodies
	void DeleteTileCommand();			//Note: it is also used for bodies

	void DefineBodyCommand(bool activate);//Physics body manipulation commands
	void ToggleShapeInsertionMode();
	void ChangeInsertionShape();
	void ChangeBodyToAddShapesTo();
	void ToggleDrawPhysics();

private:
	//----- INTERNAL VARIABLES -----
	//Edited level and editor entities (another level class)
	GameLevelPointer mLevelEditorEntities; //A level class, only to store editor-specific assets
	GameLevelPointer mEditedLevel;		   //The edited level pointer	
	BodiesList mBodiesList;					//As a level data only contains graphical entities, (agents and physics bodies are contained
											//in respective managers). A created bodies list is added just for editing mode.
	//Controllers for input
	EditorMouse* mEditorMouse;          //Controller pointer (game mouse)
	EditorKeyBoard* mEditorKeyBoard;	//Controller pointer (game keyboard)

	//Objects managers
	PhysicsManagerPointer mPhysicsMgr;	//Physics

	//Events listener
	EditorLogicListener* mListener;		//Events listener class
	
	//Internal for various purposes
	static const std::string mEditorEntitiesFileName;  //Filename of assets for level editor
	static const std::string mEmptyLevelFileName;		//Empty level filename
	static int mEditedSprites;							//Number of edited levels
	static int mEditedBodies;							//Number of edited bodies
	SpritePointer mInfoTextSprite;						//Infotext sprite to show messages to user
	std::string mInfoText;								//The info text string to show in messages through sprite
	Vector2 mMovingSpeed;								//Speed to move camera around level
	float mInitialPosX;									//Memories to move sprites around
	float mInitialPosY;
	Vector2 mInitialBodyPos;							//Memories to move bodies around
		
	//Commands tracking
	bool mFullInfoText;									//Show all available commands or not
	bool mShowSpInfoText;								//Show sprite info text
	bool mChangeLayer;									//Change editing layer
	bool mMovement;										//Move camera around
	bool mZoomIn;										//Zoom in and out command
	bool mZoomOut;
	bool mIncreaseTileSurf;								//Change tile up/down
	bool mDecreaseTileSurf;
	bool mChangeInsertionType;							//Change insertion type
	bool mMoveTile;										//Move by dragging command
	bool mMoveTileExact;								//Move by direction 1 unit or pixel
	Vector2 mMoveExactDirection;						//Direction of moving
	bool mRotateCW;										//Tile rotation command	
	bool mScaleTileUp;									//Scale tile command (up)
	bool mScaleTileDown;								//Scale tile command (down)
	bool mScaleTileX;									//Scale tile command in X
	bool mScaleTileY;									//Scale tile command in Y
	bool mVertFlip;										//Vertical flipping
	bool mHorFlip;										//Horizontal flipping
	bool mIncreaseTransp;								//Increase transparency
	bool mDecreaseTransp;								//Decrease transparency
	bool mIncreaseZ;									//Increase Z order
	bool mDecreaseZ;									//Decrease Z order
	bool mResetRotation;								//Reset rotations
	bool mResetScale;									//Reset scales
	bool mToggleWrap;									//Tile wrapping variables
	bool mWrapTileUp;
	bool mWrapTileDown;
	bool mWrapTileX;
	bool mWrapTileY;
	bool mInsertTile;									//Insert new tile
	bool mDeleteTile;									//Delete selected tile
	bool mCloneTile;									//Clone selected tile
	bool mDropCloned;									//Drop cloned tile
	bool mDefineBody;									//Define a new body
	bool mDefiningBody;									//Memory when dragging to define a body
	bool mToggleBodiesEditMode;							//Change insertion mode for shapes (add to body, or insert with new body)
	bool mChangeAddingBody;								//Change body to add shapes to
	bool mChangeInsertShape;							//Change insertion shape type for new body
	bool mDrawPhysicsElements;							//Command to draw physics elements (hidden)

	//Brush element (editing surface)
	ResourceManager::LevelResourcesPointer mAvailableResources;  //Container with all available resources to edit from
	ResourceManager::SurfacesMapIterator mCurrentBackdropSurface;
	ResourceManager::AnimationsMapIterator mCurrentBackdropAnimation;
	EditBrush mEditingBrush;									//The brush which will be drawn to insert new elements
	EditBrush mCloningBrush;									//The brush which holds cloned entity
	int mCurrentEditingLayer;									//Current editing layer
	Camera2DPointer mCurrentCamera;								//Current related camera to layer
	Camera2DPointer mEditLayerCam;								//Editing layer camera (to follow current camera)
	EditedElement mCurrentCollided;								//Current collided sprite to edit

	//----- INTERNAL FUNCTIONS -----
	//Init and release
	void _init();
	void _release();
	//Reset commands
	void _resetCommands();
	//Windows open/save file dialog
	std::string _fileDialog (char *filter, bool open, char *title);
	//Default attributes for brush when changing it (IndieLib resets them)
	void _putBrushAttributes();
	//Copy attribues from sprite
	void _cloneSprite(const EditedElement& toclone);
	//Update showing info for edited sprite
	void _updateSpriteInfo();
	//Check collisions with editing brush (returns a valid SpritePointer if collided)
	SpritePointer EditorLogic::_checkCollisionswithBrush(std::string &collidedname);
};

#endif