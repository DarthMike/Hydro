/*
	Filename: Vector2.h
	Author: Miguel Angel Quinones
	Description: Class to encapsulate functionality of a 2D Vector
	Comments: Standard Vector 2D functionality (3 values per axis x, y, z)
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _Vector2D
#define _Vector2D 

//Libraries dependencies
#include <math.h>
#include <assert.h>

//Classes dependencies

class Vector2
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	//Default constructor
	Vector2(): x(0), y(0)
	{
	}
	//Must use this constructor to create a vector with data
	explicit Vector2(double newx, double newy): x(newx), y(newy)
	{}
	~Vector2()
	{}
	//****Operators overloading****
	//Equaling to other vector
    Vector2& operator = ( const Vector2& newvector )
    {
         x = newvector.x;
         y = newvector.y;

		 return *this;
    }
	//Comparison between vectors
    inline bool operator == ( const Vector2& newvector ) const
    {
            return ( x == newvector.x && y == newvector.y);
    }
    inline bool operator != ( const Vector2& newvector ) const
	{
            return ( x != newvector.x || y != newvector.y);
	}
	//Arithmetic
	//Sum of coordinates
    inline Vector2 operator + ( const Vector2& newvector ) const
    {
          return Vector2(
                x + newvector.x,
                y + newvector.y);
    }
	//Self sum....
    inline const Vector2& operator + () const
    {
        return *this;
    }
	//Sum updating - Point to point
    inline Vector2& operator += ( const Vector2& tosum )
    {
        x += tosum.x;
        y += tosum.y;

		return *this;
    }
	//Sum updating - Offset to every coordinate
	inline Vector2& operator += (const double offset)
	{
        x += offset;
        y += offset;
	}
	//Rest of coordinates
    inline Vector2 operator - ( const Vector2& newvector ) const
    {
         return Vector2(
             x - newvector.x,
             y - newvector.y);
    }
	//Rest Update
    inline Vector2& operator -= ( const Vector2& torest )
    {
        x -= torest.x;
        y -= torest.y;

		return *this;
    }
	//Rest updating - Offset to every coordinate
	inline Vector2& operator -= (const double offset)
	{
        x -= offset;
        y -= offset;

		return *this;
	}
	//Rest or negative vector
    inline Vector2 operator - () const
    {
         return Vector2(-x, -y);
    }
	//Scalar multiply
    inline Vector2 operator * ( const double scalar ) const
    {
        return Vector2(
            x * scalar,
            y * scalar);
    }
	//Point to point multiply
    inline Vector2 operator * ( const Vector2& rhs) const
    {
        return Vector2(
            x * rhs.x,
            y * rhs.y);
    }
	//Multiply update - Multiply to scalar
    inline Vector2& operator *= ( const double scalar )
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }
	//Multiply update - Point to point multiply
    inline  Vector2& operator *= ( const Vector2& tomultiply )
    {
        x *= tomultiply.x;
        y *= tomultiply.y;

		return *this;
    }
	//Scalar divide
    inline Vector2 operator / ( const double scalar ) const
    {
        assert( scalar != 0.0 );
        double inv = 1.0 / scalar;
        return Vector2(
	        x * inv,
            y * inv);
    }
	//Point to point divide
	inline Vector2 operator / ( const Vector2& divideto) const
	{
		assert(divideto.x != 0 && divideto.y != 0);
        return Vector2(
            x / divideto.x,
            y / divideto.y);
    }
	//Divide update - Divide to scalar
	inline Vector2& operator /= ( const double scalar )
    {
        assert( scalar != 0.0 );

		double inv = 1.0 / scalar;
        x *= inv;
        y *= inv;
        return *this;
    }
	//Divide update - Point to point divide
    inline Vector2& operator /= ( const Vector2& todivide )
    {
        x /= todivide.x;
        y /= todivide.y;
        return *this;
    }

	//******************
	
	//----- GET/SET FUNCTIONS -----
	//No Get/Set - PUBLIC VARIABLES
	//----- OTHER FUNCTIONS -----
	//Clamp values
	inline void Clamp(double maxvalue)
	{
		if(x > maxvalue)
		{
			x =  maxvalue;
		}
		else if(x < -maxvalue)
		{
			x = -maxvalue;
		}

		if(y > maxvalue)
		{
			y =  maxvalue;
		}
		else if(y < -maxvalue)
		{
			y = -maxvalue;
		}
	}
	//Length of vector
	inline double Length() const
	{
		return(sqrt((x*x) + (y*y)));
	}
	//Dot product of vectors
	inline double DotProduct(const Vector2& vec) const
	{
		return x * vec.x + y * vec.y;
	}
	//Normalize of vector
	inline double Normalise()
	{
		double length = Length();

		// Protection on doubleing point calcs
		if ( length > 1e-08 )
		{
			double invlength = 1.0 / length;
			x *= invlength;
			y *= invlength;
		}

		return length;
	}
	//Return distance to another vector
	inline double Distance(const Vector2& to) const
	{
		return (*this - to).Length();
	}	
	
	//Return angle to another vector
	inline double AngleBetweenUnitVecs(const Vector2& dest)
	{
		//Angle between vectors:
		//alpha = arcos((vec1*vec2)/|vec1|*|vec2|) in a general way
		//alpha = arcos(vec1*vec2) in unit vectors

		double dotp = DotProduct(dest);

		return (acos(dotp));
	}
	//Check if it is equal to another vector with tolerance
	inline bool IsEqualWithToleranceTo(const Vector2& tocompare, float tolerance)
	{
		//Compare to another vector, with tolerance
		bool equalx = false;
		bool equaly = false;

		//X coords comparing
		if( (x < (tocompare.x + tolerance)) && (x > (tocompare.x - tolerance)) )
		{
			equalx = true;
		}
		//Y coords comparing
		if( (y < (tocompare.y + tolerance)) && (y > (tocompare.y - tolerance)) )
		{
			equaly = true;
		}

		return (equalx && equaly);
	}
	
	//----- PUBLIC VARIABLES -----
	double x,y;			//Values of vector
};

#endif
