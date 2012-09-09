/*
	Filename: Vector3.h
	Author: Miguel Angel Quinones
	Description: Class to encapsulate functionality of a 3D Vector
	Comments: Standard Vector 3D functionality (3 values per axis x, y, z)
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _VECTOR3D
#define _VECTOR3D 

//Libraries dependencies
#include <math.h>
#include <assert.h>

//Classes dependencies

class Vector3
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	//Default constructor
	Vector3(): x(0), y(0), z(0)
	{
	}
	//Must use this constructor to create a vector with data
	explicit Vector3(double newx, double newy,double newz): x(newx), y(newy), z(newz)
	{}
	~Vector3()
	{}
	//****Operators overloading****
	//Equaling to other vector
    Vector3& operator = ( const Vector3& newvector )
    {
         x = newvector.x;
         y = newvector.y;
         z = newvector.z;

         return *this;
    }
	//Comparison between vectors
    inline bool operator == ( const Vector3& newvector ) const
    {
            return ( x == newvector.x && y == newvector.y && z == newvector.z );
    }
    inline bool operator != ( const Vector3& newvector ) const
	{
            return ( x != newvector.x || y != newvector.y || z != newvector.z );
	}
	//Arithmetic
	//Sum of coordinates
    inline Vector3 operator + ( const Vector3& newvector ) const
    {
          return Vector3(
                x + newvector.x,
                y + newvector.y,
                z + newvector.z);
    }
	//Self sum....
    inline const Vector3& operator + () const
    {
        return *this;
    }
	//Sum updating - Point to point
    inline Vector3& operator += ( const Vector3& tosum )
    {
        x += tosum.x;
        y += tosum.y;
        z += tosum.z;

		return *this;
    }
	//Sum updating - Offset to every coordinate
	inline Vector3& operator += (const double offset)
	{
        x += offset;
        y += offset;
        z += offset;
	}
	//Rest of coordinates
    inline Vector3 operator - ( const Vector3& newvector ) const
    {
         return Vector3(
             x - newvector.x,
             y - newvector.y,
             z - newvector.z);
    }
	//Rest Update
    inline Vector3& operator -= ( const Vector3& torest )
    {
        x -= torest.x;
        y -= torest.y;
        z -= torest.z;

		return *this;
    }
	//Rest updating - Offset to every coordinate
	inline Vector3& operator -= (const double offset)
	{
        x -= offset;
        y -= offset;
        z -= offset;
	}
	//Rest or negative vector
    inline Vector3 operator - () const
    {
         return Vector3(-x, -y, -z);
    }
	//Scalar multiply
    inline Vector3 operator * ( const double scalar ) const
    {
        return Vector3(
            x * scalar,
            y * scalar,
            z * scalar);
    }
	//Point to point multiply
    inline Vector3 operator * ( const Vector3& rhs) const
    {
        return Vector3(
            x * rhs.x,
            y * rhs.y,
            z * rhs.z);
    }
	//Multiply update - Multiply to scalar
    inline Vector3& operator *= ( const double scalar )
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
	//Multiply update - Point to point multiply
    inline  Vector3& operator *= ( const Vector3& tomultiply )
    {
        x *= tomultiply.x;
        y *= tomultiply.y;
        z *= tomultiply.z;

		return *this;
    }
	//Scalar divide
    inline Vector3 operator / ( const double scalar ) const
    {
        assert( scalar != 0.0 );
        double inv = 1.0 / scalar;
        return Vector3(
	        x * inv,
            y * inv,
            z * inv);
    }
	//Point to point divide
	inline Vector3 operator / ( const Vector3& divideto) const
	{
		assert(divideto.x != 0 && divideto.y != 0 && divideto.z != 0);
        return Vector3(
            x / divideto.x,
            y / divideto.y,
            z / divideto.z);
    }
	//Divide update - Divide to scalar
	inline Vector3& operator /= ( const double scalar )
    {
        assert( scalar != 0.0 );

		double inv = 1.0 / scalar;
        x *= inv;
        y *= inv;
        z *= inv;
        return *this;
    }
	//Divide update - Point to point divide
    inline Vector3& operator /= ( const Vector3& todivide )
    {
        x /= todivide.x;
        y /= todivide.y;
        z /= todivide.z;
        return *this;
    }

	//******************
	
	//----- GET/SET FUNCTIONS -----
	//No Get/Set - PUBLIC VARIABLES
	//----- OTHER FUNCTIONS -----
	//Length of vector
	double Length();
	//Dot product of vectors
	double DotProduct(const Vector3& vec) const;
	//Cross Product of vectors - Right handed coord system!!!
    Vector3 CrossProduct( const Vector3& other ) const;
	//Normalize of vector
	double Normalise();	
	//Return distance to another vector
	double Distance(const Vector3& to) const;
	//Return angle to another vector
	double AngleBetweenUnitVecs(const Vector3& dest); 
	
	//----- PUBLIC VARIABLES -----
	double x,y,z;			//Values of vector
};

#endif
