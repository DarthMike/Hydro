/*****************************************************************************************
/* File: IND_Math.cpp
/* Desc: �Math methods?
/*****************************************************************************************/
 
/*
IndieLib 2d library Copyright (C) 2005 Javier L�pez L�pez (javierlopezpro@gmail.com) 

This library is free software; you can redistribute it and/or modify it under the 
terms of the GNU Lesser General Public License as published by the Free Software 
Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with 
this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
Suite 330, Boston, MA 02111-1307 USA 
*/

// ----- Includes -----

#include "Global.h"
#include "Defines.h"
#include "IND_Math.h"


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*
==================
Init
==================
*/
bool IND_Math::Init ()
{
	End ();
	FreeVars ();

	// Random Seed
	srand (GetTickCount());

	mOk = true;
	
	return mOk;
}


/*
==================
End
==================
*/
void IND_Math::End()
{
	if (mOk)
	{
	
		FreeVars ();	
	
		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//										 Public methods
// --------------------------------------------------------------------------------

/*
==================
Sets the seed for random numbers
==================
*/
void IND_Math::RandSeed	(long pNum)
{
	srand (pNum);
}


/*
==================
Returns a random number between two boundaries
==================
*/
inline long IND_Math::Rand	(long pIni, long pEnd)
{
	if (pIni > pEnd)
	{
		long pTemp = pEnd;
		pEnd = pIni;
		pIni = pTemp;
	}

	return rand () % (pEnd - pIni + 1) + pIni;   
	
	//long mResu = pIni + (rand () % ((pEnd - pIni) + 1));
	//return mResu;
}


/*
==================
Returns 1 if the number is power of two, 0 otherwise
==================
*/
bool IND_Math::IsPowerOfTwo (long pN)      
{
	while (pN > 1L) 
	{
		if (pN % 2L) break;
		pN >>= 1L;
	}

	return (pN == 1L);
}


// --------------------------------------------------------------------------------
//									 Private methods
// --------------------------------------------------------------------------------

/*
==================
Init vars
==================
*/
void IND_Math::InitVars ()
{

}


/*
==================
Free memory
==================
*/
void IND_Math::FreeVars ()
{

}
