/*
	Filename: GFXDefs.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Definitions used in Graphics system
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _GFXDEFS
#define _GFXDEFS

#include "Shared_Resources.h"
#include "Vector2.h"

//Color class declaration
typedef struct ColorRGBA
{
	//CONTRUCTOR by default
	ColorRGBA():
	red(0),
	green(0),
	blue(0),
	alpha(255)
	{}
	//CONSTRUCTOR With values
	ColorRGBA(int r, int g, int b, int a):
	red(r),
	green(g),
	blue(b),
	alpha(a)
	{}

	//VARIABLES
	int red,green,blue,alpha;
	
	//Assignment operator for fast copying
	ColorRGBA& operator=(const ColorRGBA &newcolor)
	{
		//Deep copy of elements in struct
		red = newcolor.red;
		green = newcolor.green;
		blue = newcolor.blue;
		alpha = newcolor.alpha;
		return (*this);
	}

	//Compare operator for fast compare
	bool operator==(const ColorRGBA& tocompare)
	{
		//Deep copy of elements in struct
		return (red == tocompare.red && green == tocompare.green && blue == tocompare.blue && alpha == tocompare.alpha);
	}

	//Compare operator for fast compare
	bool operator!=(const ColorRGBA& tocompare)
	{
		//Deep copy of elements in struct
		return (red != tocompare.red || green != tocompare.green || blue != tocompare.blue || alpha != tocompare.alpha);
	}
}ColorRGBA;

typedef struct ColorHSLA
{
	//CONSTRUCTOF by default
	ColorHSLA():
	hue(0),
	saturation(0),
	lightness(0),
	alpha(255)
	{}
	//CONSTRUCTOR With values
	ColorHSLA(int h, float s, float l, int a):
	hue(h),
	saturation(s),
	lightness(l),
	alpha(a)
	{}

	//VARIABLES
	int hue;
	float saturation,lightness;
	int alpha;

	//Assignment operator for fast copying
	ColorHSLA& operator=(const ColorHSLA &newcolor)
	{
		//Deep copy of elements in struct
		hue = newcolor.hue;
		saturation = newcolor.saturation;
		lightness = newcolor.lightness;
		alpha = newcolor.alpha;
		return (*this);
	}

	//Compare operator for fast compare
	bool operator==(const ColorHSLA& tocompare)
	{
		//Compare floating points!
		return (hue == tocompare.hue && saturation == tocompare.saturation && lightness == tocompare.lightness && alpha == tocompare.alpha);
	}
			
	//Compare operator for fast compare
	bool operator!=(const ColorHSLA& tocompare)
	{
		//Compare floating points!
		return (hue != tocompare.hue || saturation != tocompare.saturation || lightness != tocompare.lightness || alpha != tocompare.alpha);
	}
}ColorHSLA;

//A custom structure to hold contained sprite data
typedef struct ContainedSprite
{
	//CONTRUCTOR by default
	ContainedSprite():
	rotoffset(0),
	posoffset(0,0)
	{}
	//CONTRUCTOR with values
	ContainedSprite(SpritePointer gfx, const Vector2& positionoffset, float rotationoffset):
	gfxentity(gfx),
	rotoffset(rotationoffset),
	posoffset(positionoffset)
	{}

	SpritePointer gfxentity;
	float rotoffset;
	Vector2 posoffset;

	//Assignment operator for fast copying
	ContainedSprite& operator=(const ContainedSprite &newvalues)
	{
		//Deep copy of elements in struct
		gfxentity = newvalues.gfxentity;
		rotoffset = newvalues.rotoffset;
		posoffset = newvalues.posoffset;
		return (*this);
	}
}ContainedSprite;
#endif