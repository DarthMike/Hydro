/*****************************************************************************************
/* Desc: Singleton class for parsing collision files
/*****************************************************************************************/

// ----- Includes -----

#include "Global.h"
#include "..\Dependencies\TinyXml\tinyxml.h"
#include "CollisionParser.h"


// --------------------------------------------------------------------------------
//							   Singleton initialization
// --------------------------------------------------------------------------------

/*
==================
Init singleton
==================
*/
CollisionParser* CollisionParser::pinstance = 0;// initialize pointer
CollisionParser* CollisionParser::Instance () 
{
	if (pinstance == 0)  // is it the first call?
	{  
		pinstance = new CollisionParser; // create sole instance
	}
	return pinstance; // address of sole instance
}


// --------------------------------------------------------------------------------
//								 Public methods
// --------------------------------------------------------------------------------

/*
==================
Bounding triangle
==================
*/
void CollisionParser::SetBoundingTriangle	(list <BOUNDING_COLLISION*> *pBList, char *pId, int pAx, int pAy, int pBx, int pBy, int pCx, int pCy)
{	
	BOUNDING_COLLISION *mB = new BOUNDING_COLLISION;

	mB->mType	= 0;
	mB->mId		= pId;
	mB->mAx		= pAx;
	mB->mAy		= pAy;
	mB->mBx		= pBx;
	mB->mBy		= pBy;
	mB->mCx		= pCx;
	mB->mCy		= pCy;

	pBList->push_back (mB);
}


/*
==================
Bounding circle
==================
*/
void CollisionParser::SetBoundingCircle	(list <BOUNDING_COLLISION*> *pBList, char *pId, int pOffsetX, int pOffsetY, int pRadius)
{
	BOUNDING_COLLISION *mB = new BOUNDING_COLLISION;

	mB->mType		= 1;
	mB->mId			= pId;
	mB->mPosX		= pOffsetX;
	mB->mPosY		= pOffsetY;
	mB->mRadius		= pRadius;

	pBList->push_back (mB);
}


/*
==================
Bounding rectangle
==================
*/
void CollisionParser::SetBoundingRectangle	(list <BOUNDING_COLLISION*> *pBList, char *pId, int pOffsetX, int pOffsetY, int pWidth, int pHeight)
{
	// First triangle
	BOUNDING_COLLISION *mB1 = new BOUNDING_COLLISION;

	mB1->mType	= 0;
	mB1->mId	= pId;
	mB1->mAx	= pOffsetX;
	mB1->mAy	= pOffsetY;
	mB1->mBx	= pOffsetX + pWidth;
	mB1->mBy	= pOffsetY;
	mB1->mCx	= pOffsetX;
	mB1->mCy	= pOffsetY + pHeight;

	pBList->push_back (mB1);

	// Second triangle
	BOUNDING_COLLISION *mB2 = new BOUNDING_COLLISION;

	mB2->mType	= 0;
	mB2->mId	= pId;
	mB2->mAx	= pOffsetX + pWidth;
	mB2->mAy	= pOffsetY;
	mB2->mBx	= pOffsetX + pWidth;
	mB2->mBy	= pOffsetY + pHeight;
	mB2->mCx	= pOffsetX;
	mB2->mCy	= pOffsetY + pHeight;

	pBList->push_back (mB2);
}


/*
==================
Parses and XML collision file
Uses Tinyxml
==================
*/
bool CollisionParser::ParseCollision (list <BOUNDING_COLLISION*> *pBList, char *pFile)
{
	TiXmlDocument	*mXmlDoc = new TiXmlDocument (pFile);

	// Fatal error, cannot load
	if (!mXmlDoc->LoadFile()) return 0;

	// Document root
	TiXmlElement *mXBoundingAreas = 0;
	mXBoundingAreas = mXmlDoc->FirstChildElement("bounding_areas");

	if (!mXBoundingAreas)
	{
		Debug->Header ("Invalid name for document root, should be <bounding_areas>", 2);
		mXmlDoc->Clear();
		delete mXmlDoc;
		return 0;
	}

	// ----- Triangle -----
	TiXmlElement *mXTriangle = 0;
	mXTriangle = mXBoundingAreas->FirstChildElement("triangle");

	while (mXTriangle)
	{
		if (mXTriangle->Attribute("id") && 
			mXTriangle->Attribute("ax") && 
			mXTriangle->Attribute("ay") && 
			mXTriangle->Attribute("bx") && 
			mXTriangle->Attribute("by") && 
			mXTriangle->Attribute("cx") && 
			mXTriangle->Attribute("cy"))
		{
			char *mId = new char [strlen (mXTriangle->Attribute("id"))];
			strcpy (mId, mXTriangle->Attribute("id"));
			SetBoundingTriangle	(pBList,
								mId,
								atoi (mXTriangle->Attribute("ax")),
								atoi (mXTriangle->Attribute("ay")),
								atoi (mXTriangle->Attribute("bx")),
								atoi (mXTriangle->Attribute("by")),
								atoi (mXTriangle->Attribute("cx")),
								atoi (mXTriangle->Attribute("cy")));
		}
		else
		{
			Debug->Header ("The triangle doesn't have all the attributes", 2);
			mXmlDoc->Clear();
			delete mXmlDoc;
			return 0;
		}

		// Move to the next element
		mXTriangle = mXTriangle->NextSiblingElement("triangle");	
	}

	// ----- Circle -----
	TiXmlElement *mXCircle = 0;
	mXCircle = mXBoundingAreas->FirstChildElement("circle");

	while (mXCircle)
	{
		if (mXCircle->Attribute("id") && 
			mXCircle->Attribute("x") && 
			mXCircle->Attribute("y") && 
			mXCircle->Attribute("radius"))
		{
			char *mId = new char [strlen (mXCircle->Attribute("id"))];
			strcpy (mId, mXCircle->Attribute("id"));
			SetBoundingCircle	(pBList,
								mId,
								atoi (mXCircle->Attribute("x")),
								atoi (mXCircle->Attribute("y")),
								atoi (mXCircle->Attribute("radius")));
		}
		else
		{
			Debug->Header ("The circle doesn't have all the attributes", 2);
			mXmlDoc->Clear();
			delete mXmlDoc;
			return 0;
		}

		// Move to the next element
		mXCircle = mXCircle->NextSiblingElement("circle");
	}

	// ----- Rectangle -----
	TiXmlElement *mXRectangle= 0;
	mXRectangle = mXBoundingAreas->FirstChildElement("rectangle");

	while (mXRectangle)
	{
		if (mXRectangle->Attribute("id") && 
			mXRectangle->Attribute("x") && 
			mXRectangle->Attribute("y") && 
			mXRectangle->Attribute("width") &&
			mXRectangle->Attribute("height"))
		{
			char *mId = new char [strlen (mXRectangle->Attribute("id"))];
			strcpy (mId, mXRectangle->Attribute("id"));
			SetBoundingRectangle	(pBList,
									mId,
									atoi (mXRectangle->Attribute("x")),
									atoi (mXRectangle->Attribute("y")),
									atoi (mXRectangle->Attribute("width")),
									atoi (mXRectangle->Attribute("height")));
		}
		else
		{
			Debug->Header ("The rectangle doesn't have all the attributes", 2);
			mXmlDoc->Clear();
			delete mXmlDoc;
			return 0;
		}

		// Move to the next element
		mXRectangle = mXRectangle->NextSiblingElement("rectangle");
	}

	// Delete our allocated document and return success ;)
  	mXmlDoc->Clear();
	delete mXmlDoc;

	return 1;
}


/*
==================
Delete all the bounding areas with a certain id. "*" will delete all the areas.
==================
*/
void CollisionParser::DeleteBoundingAreas (list <BOUNDING_COLLISION*> *pBList, char *pId)
{
	bool mExit = 0;

	while (!mExit)
	{
		mExit = 1;
		list <BOUNDING_COLLISION*>::iterator mListIter;
		for (mListIter  = pBList->begin();
			 mListIter  != pBList->end();
			 mListIter++)
		{
			// Check the id, if it's equal, delete the area
			if (!strcmp ((*mListIter)->mId, pId) || !strcmp (pId, "*"))
			{
				Dispose ((*mListIter));
				pBList->remove ((*mListIter));
				mExit = 0;
				break;
			}
		}
	}
}
