/*
	Filename: SpriteBuilder.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a parametric sprite of IndieLib from XML file
				 It also saves to xml file the parameters of sprite
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _SPRITEBUILDER
#define _SPRITEBUILDER

//Library dependencies	
#include "boost\shared_ptr.hpp"  //Smart pointer facilites - not using std because it cant go inside an 
								 //STL container as we do in event manager...

#include <string>
//Class dependencies
#include "XMLParser.h"
#include "LogManager.h"
#include "GenericException.h"
#include "GFXDefs.h"


class SpriteBuilder
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	SpriteBuilder()
	{
	}
	~SpriteBuilder()
	{
	}
	//----- GET/SET FUNCTIONS -----
	ContainedSprite GetCreatedSprite() { return mSprite; }  //Returns A COPY of the created sprite
	//----- OTHER FUNCTIONS -----
	void LoadSprite(const ticpp::Element* xmlelement);   //Read a part of xml info and create a blob object
	void SaveSprite(const SpritePointer thesprite, ticpp::Element& xmlelement); //Save parameters of sprite in an xml element
protected:
	//----- INTERNAL VARIABLES -----
	ContainedSprite mSprite;
	//----- INTERNAL FUNCTIONS -----
};

#endif