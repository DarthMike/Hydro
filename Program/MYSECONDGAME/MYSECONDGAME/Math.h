/*
	Filename: Math.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate useful math operations in C++
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _MATH
#define _MATH

//Library dependencies
#include <stdlib.h>
#include <time.h>
#include <math.h>
//Class dependencies
#include "Singleton_Template.h"

//Forward declarations
class Vector2;
class Vector3;

class Math : public MeyersSingleton<Math>
{
	//Definitions
public:
	static const double MaxDegrees;
	static const double MinDegrees;
	static const double Pi;
	static const double Two_Pi;
	static const double ClampingRatio;
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	Math()
	{
		//Initialize random number generator
		srand ((unsigned int) time(NULL) );
	}
	~Math()
	{}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	//Generic
	int NewRandom(int low, int high);   //Returns a random number between boundaries
	double ClampedRandom();				//Returns a random number between -1 and 1
	Vector2 ClampVector2(Vector2& vector,double maxvalue);	//Values clamping to max
	Vector3 ClampVector3(Vector3& vector,double maxvalue);	//Values clamping to max
	Vector2 FindPerpendicularVector2(Vector2& vector, float value);	//Find a perpendicular vector given a value > or < 0
	double ClampNumber(double num, double max);				//Values clamping to max
	//Radians to angle (templated)
	template <typename type>
	type RadiansToAngle(type rads, bool invert = false)	
	{
		type angle; //Angle to return

		//Inversion of angle results
		if(invert)
		{
			angle = (-1) * rads * static_cast<type>(MaxDegrees / Two_Pi);
		}
		else
		{
			angle = rads * static_cast<type>(MaxDegrees / Two_Pi);
		}
		return angle;
	}

	//Angle to radians (templated)
	template <typename type>
	type AngleToRadians(type angle, bool invert = false)
	{
		type radians; //Radians to return

		//Inversion of radians result
		if(invert)
		{
			radians = (-1) * angle * static_cast<type>(Two_Pi / MaxDegrees);
		}
		else
		{
			radians = angle * static_cast<type>(Two_Pi / MaxDegrees);
		}

		return radians;
	}
};

//Definitions - SINGLETON MATH OBJECT
typedef Math SingletonMath;

#endif