/*
	Filename: Math.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate useful math operations in C++ 
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "Math.h"
#include "Vector2.h"
#include "Vector3.h"

const double Math::MaxDegrees = 360.0f;			//Max Angle
const double Math::MinDegrees = 0.0f;			//Min Angle
const double Math::Pi = 3.14159265358979323846f;//Pi definition
const double Math::Two_Pi = 2 * Math::Pi;		//2*Pi definition
const double Math::ClampingRatio = 0.80f;		//How much reduce from max value when clamping

//Returns a random number between boundaries
int Math::NewRandom(int low, int high)   
{
	//Modulo operation to transform 0-RAND_MAX to
	//0 - high, then sum of minimum range
	if(high>low)
	{
		return((rand() % (high - low)) + low);
	}
	else
	{	//If not correct parameters a 0 is returned
		return(0);
	}	
}

//Returns a random number between -1 and 1
double Math::ClampedRandom()
{
	return (NewRandom(-1,1));
}

//Values clamping to max
Vector2 Math::ClampVector2(Vector2& vector,double maxvalue)
{
	//Clamp coords
	vector.x = ClampNumber(vector.x,maxvalue);
	vector.y = ClampNumber(vector.y,maxvalue);

	return Vector2(vector.x,vector.y);   //Returns a new vector
}

//Values clamping to max
Vector3 Math::ClampVector3(Vector3& vector,double maxvalue)
{
	//Clamp coords
	vector.x = ClampNumber(vector.x,maxvalue);
	vector.y = ClampNumber(vector.y,maxvalue);
	vector.z = ClampNumber(vector.z,maxvalue);

	return Vector3(vector.x,vector.y,vector.z);   //Returns a new vector
}

//Find a perpendicular vector given a value > or < 0
Vector2 Math::FindPerpendicularVector2(Vector2& vector, float value)
{
	//If value is > 0 , it returns one direction perpendicular vector, 
	//otherwise, returns the other one

	Vector2 returnvector;

	if(value <= 0)
	{
		double temp = vector.x;
		returnvector.x = vector.y;
		returnvector.y = -temp;
	}
	else
	{
		double temp = vector.x;
		returnvector.x = -vector.y;
		returnvector.y = temp;
	}

	return (returnvector);
}

//Values clamping to max
double Math::ClampNumber(double num, double max)
{
	if(num>max)
	{
		return(num * ClampingRatio);
	}
	else
	{
		return num;
	}
}