/*
	Filename: Generic2DMatrix.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Template to create a generic 2D Matrix class for any type
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
#ifndef _MATRIX2DTEMPLATE
#define _MATRIX2DTEMPLATE

//Class dependencies
#include <exception>

template <class T>
class Generic2DMatrix
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	//Matrix constructor
	Generic2DMatrix(int sizex, int sizey): mSizeX(sizex), mSizeY(sizey), mMaxIndex((sizex*sizey)-1)
	{
		//Create data for matrix
		//Internally it will be a linear array
		mMatrixData = new T[mMaxIndex+1];

		if(mMatrixData)
		{
			//Init values
			int i = 0;
			for(i=0;i<=mMaxIndex;i++)
				mMatrixData[i] = 0;
		}
	}

	//Copy constructor
	Generic2DMatrix(const Generic2DMatrix &original)
	{
		_copyvalues(original);
	}

	~Generic2DMatrix()
	{
		delete [] mMatrixData;
	}

	//----- Operators overloading -----
	//OPERATOR +
	Generic2DMatrix<T>& operator =(const Generic2DMatrix<T> &original)
	{
		//Check if original is not the same as this one..!
		if(this != &original)
		{
			//Make copy of values from original
			_copyvalues(original);
		}

		return(*this); //Return reference to allow multiple assignment
	}
	
	//----- GET/SET FUNCTIONS -----
	const T &GetValue(int x, int y);			//Get x,y value
	void SetValue(int x, int y, T& value);		//Set x,y value
	int GetSizeX() { return mSizeX;}			//Get X size
	int GetSizeY() { return mSizeY;}			//Get X size

	//----- OTHER FUNCTIONS -----
	void Reset(T &value);						//Reset matrix to some value
	void Rotate(bool cw);						//Rotate Matrix
	bool IsZero();								//Checks if matrix is zero valued	

protected:
	int mSizeX;					//Size
	int mSizeY;
	int mMaxIndex;				//Maximum index once matrix is created
	T* mMatrixData;				//Internal data pointer

	//----- INTERNAL FUNCTIONS -----
	void _copyvalues(const Generic2DMatrix<T> &original);
};

//-------------IMPLEMENTATION OF METHODS---------------------------------------

//Get x,y value
template <class T>
const T&Generic2DMatrix<T>::GetValue(int x, int y)	
{
	//In range error
	if((x >= mSizeX)
		||
		(y >= mSizeY)
		)
	{
		throw(std::exception("Error in index of GenericMatrix - Out of bounds"));
	}


	//Conversion of coordinates:
	//Real coordinate is linear, so inside linear array:
	//index =  x * mSizeY + y
	return(mMatrixData[x*mSizeY + y]);
}
	
//Set x,y value
template <class T>
void Generic2DMatrix<T>::SetValue(int x, int y, T& value)
{

	//In range error
	if((x >= mSizeX)
		||
		(y >= mSizeY)
		)
	{
		throw(std::exception("Error in index of GenericMatrix - Out of bounds"));
	}

	//Conversion of coordinates:
	//Real coordinate is linear, so inside linear array:
	//index =  x * mSizeY + 
	mMatrixData[x*mSizeY + y] = value;
}

//Rotates a sizexsize SQUARE MATRIX clockwise or counterclockwise
template <class T>
void Generic2DMatrix<T>::Rotate(bool cw)
{
	//Rotation of the matrix stored
	//Note that matrix (2d) is stored linearly 
	//in an array, so indexing is quite different

	//Local variables
	int i;
	int j;
	int *tempmatrix = new int[mMaxIndex+1];
		
	//IF - ROTATION CLOCKWISE / COUNTERCLOCKWISE
	if(cw)
	{
		//Clockwise rotation
		for(i=0;i<mSizeX;i++)
		{
			for(j=0;j<mSizeY;j++)
			{
				//Linear access to memory; in 2d arrays would be:
				//tempmatrix[j][mSizeY-i-1] = Matrix[i][j];
				//Linear access is coord i = mOrder * coord i / coord j = coord j
				//Sum both coords and we have real position!!
				tempmatrix[(mSizeY*j)+(mSizeY-i-1)] = mMatrixData[(mSizeY*i)+j];
			}
		}
		
	}
	else //ELSE - ROTATION CLOCKWISE / COUNTERCLOCKWISE
	{
		//Counterclockwise rotation
		for(i=0;i<mSizeX;i++)
		{
			for(j=0;j<mSizeY;j++)
			{
				//Linear access to memory; in 2d arrays would be:
				//tempmatrix[mSizeY-j-1][i] = Matrix[i][j];
				//Linear access is coord i = mOrder * coord i / coord j = coord j
				//Sum both coords and we have real position!!
				tempmatrix[(mSizeY*(mSizeY-j-1)) + i] = mMatrixData[(mSizeY*i) + j];
			}
		}
	}//END IF - ROTATION CLOCKWISE / COUNTERCLOCKWISE

	//Copy results to original matrix

	for(i=0;i<mSizeX;i++)
	{
		for(j=0;j<mSizeY;j++)
		{
			//Linear access to memory; in 2d arrays would be:
			//Matrix[i][j] = tempmatrix[i][j];
			//Linear access is coord i = mOrder * coordi / coord j = coordj
			//Sum both coords and we have real position!!
			mMatrixData[(mSizeY*i) + j] = tempmatrix[(mSizeY*i) + j]; 
		}
	}

	//Free dynamically allocated memory
	delete [] tempmatrix;
}

//Reset to some value
template <class T>
void Generic2DMatrix<T>::Reset(T &value)
{
	//Local variables
	int i,j;

	for(i=0;i<mOrder;i++)
	{
		for(j=0;j<mOrder;j++)
		{
			mMatrixData[i*mSizeY + j] = value;
		}
	}
}

//Checks if matrix is zero valued	
template <class T>
bool Generic2DMatrix<T>::IsZero()
{
	//Check zero-valued everywhere
	for(int i = 0; i<=mMaxIndex ; i++)
	{
		if(mMatrixData[i] != 0)
			return(false);
	}

	return (true);
}

//Copy values from another MATRIX2D STRUCTURE (for copy constructor and assignment operator)
template <class T>
void Generic2DMatrix<T>::_copyvalues(const Generic2DMatrix<T> &original)
{
	mSizeX = original.mSizeX;
	mSizeY = original.mSizeY;
	mMaxIndex = original.mMaxIndex;
		
	//Deallocate dynamic memory used before
	if(mMatrixData)
		delete[] mMatrixData;

	mMatrixData = new T[original.mMaxIndex+1];
	if(mMatrixData)
	{
		//Init values
		int i = 0;
		for(i=0;i<=mMaxIndex;i++)
		{
			mMatrixData[i] = original.mMatrixData[i];
		}
	}
}

#endif
