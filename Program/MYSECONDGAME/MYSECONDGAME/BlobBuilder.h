/*
	Filename: BlobBuilder.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a blob (type of soft body simulation with rigid bodies engine)
				 given some parameters. It creates a new group of bodies, associated to a game agent
				 provided by the client of the class
	Comments: Uses Box2D and its created class to manage it. Many resources from the net used...
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _BLOBBUILDER
#define _BLOBBUILDER

//Library dependencies	
#include <string>
#include <sstream>
//Class dependencies
#include "XMLParser.h"
#include "LogManager.h"
#include "GenericException.h"
#include "Shared_Resources.h"
#include "BlobController.h"

//Forward declarations
class IAgent;
class b2Body;

class BlobBuilder
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	BlobBuilder(PhysicsManagerPointer physicsmgrptr, IAgent* agentptr):
	  mPhysicsMgr(physicsmgrptr),
	  mRelatedAgent(agentptr)
	{
		assert(mRelatedAgent);
	}
	~BlobBuilder()
	{
	}
	//----- GET/SET FUNCTIONS -----
	BlobControllerPointer GetBlobController() { return mBlobControllerptr; }
	//----- OTHER FUNCTIONS -----
	void LoadBlob(const ticpp::Element* xmlelement);   //Read a part of xml info and create a blob object
	void LoadBlob(const BlobParameters& params);		 //Create a blob with some parameters

protected:
	//----- INTERNAL VARIABLES -----
	static int mBlobsCreated;
	PhysicsManagerPointer mPhysicsMgr; 
	IAgent* mRelatedAgent;

	BlobControllerPointer mBlobControllerptr;
	//----- INTERNAL FUNCTIONS -----
	void BlobBuilder::_load(const BlobParameters& creationparams);   //Internal creation function
};

#endif