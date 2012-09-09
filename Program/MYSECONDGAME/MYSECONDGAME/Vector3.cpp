/*
	Filename: Vector3.cpp
	Author: Miguel Angel Quinones
	Description: Class to encapsulate functionality of a 3D Vector
	Comments: Standard Vector 3D functionality (3 values per axis x, y, z)
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/


#include "Vector3.h"

//Length of vector
double Vector3::Length()
{
	return(sqrt((x*x) + (y*y) + (z*z)));
}

double Vector3::DotProduct(const Vector3& vec) const
{
    return x * vec.x + y * vec.y + z * vec.z;
}

//Cross Product of vectors - Right handed coord system!!!
Vector3 Vector3::CrossProduct( const Vector3& other ) const
{
    return Vector3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x);
}

//Normalize of vector
double Vector3::Normalise()
{
	double length = Length();

	// Protection on doubleing point calcs
    if ( length > 1e-08 )
    {
        double invlength = 1.0 / length;
        x *= invlength;
        y *= invlength;
        z *= invlength;
    }

	return length;
}	
//Return distance to another vector
double Vector3::Distance(const Vector3& to) const
{
    return (*this - to).Length();
}

//Return angle between this and another vector
double Vector3::AngleBetweenUnitVecs(const Vector3& dest)
{
	//Angle between vectors:
	//alpha = arcos((vec1*vec2)/|vec1|*|vec2|) in a general way
	//alpha = arcos(vec1*vec2) in unit vectors

	double dotp = DotProduct(dest);

	return (acos(dotp));
}