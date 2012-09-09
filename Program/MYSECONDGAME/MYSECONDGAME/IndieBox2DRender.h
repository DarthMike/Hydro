/*
	Filename: IndieBox2DRender.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: A class which manages rendering (in debug mode) of debug info of Box2D physics library
	Comments: It uses main application renderer, that is, mpILib. The connection between drawing and the real
			  physics calculation is done via callbacks, so no hand-made connection needs to be done; in other
			  words, if no world is created for Box2d, no drawing is made automatically.
	Attribution: Based in class made by user "ehudros" from IndieLib forums (http://www.mpILib.com/forum/viewtopic.php?t=240&sid=a976f5258c860f6635957434187fc857) GOOD WORK!!!
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _INDIEBOX2DDEBUGRENDER
#define _INDIEBOX2DDEBUGRENDER

/*NOTE: There are some ugly static casts to integers, as Indielib uses ints generally, for pixels values, but in physics and
other in-game calculation, normally there are floating points. I didnt find other nice way to do it, as I think indielib will be faster
with ints for rendering and storing of pixel positions, and anyway... space between 2 pixels cant be rendered so...
Maybe it is a TODO: if speed concerns in future...
*/
//Library dependencies
#include <iostream>
#include <vector>
#include "Box2D\Box2D.h"

//Class dependencies
#include "LogManager.h"

//Forward declarations
class IndieLibManager;
struct b2AABB;

class IndieBox2DRender : public b2DebugDraw
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	IndieBox2DRender(IndieLibManager* ilibptr, float scale, float windowheight):mpILib(ilibptr),mScale(scale),mOffsetY(windowheight)
	{
		SingletonLogMgr::Instance()->AddNewLine("IndieBox2DRender","Debug Draw for Box2D initialized",LOGDEBUG);
	}
	~IndieBox2DRender()
	{
		SingletonLogMgr::Instance()->AddNewLine("IndieBox2DRender","Debug Draw for Box2D destroyed",LOGDEBUG);
	}
	//----- GET/SET FUNCTIONS -----
	void SetScaleFactor(float factor) { mScale = factor; }
	//----- OTHER FUNCTIONS -----
	//Implementation of virtual functions from base class
    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
	void DrawXForm(const b2XForm& xf); 
	//----- PUBLIC VARIABLES ------

private:
	//----- INTERNAL VARIABLES -----
	IndieLibManager* mpILib;		//Pointer to IndieLib Manager
	float mScale;					//Scaling factor to draw primitives
	float mOffsetY;					//Offset to draw y axis in correct direction, as IndieLib coords of pixels start at 0 in upper position.
	//----- INTERNAL FUNCTIONS -----
};

#endif