/*****************************************************************************************
/* File: DebugApi.cpp
/* Desc: Class that manages the output of debuggin information about the execution of IndieLib
/* TODO: quit type numbers and use defines
/*****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (javierlopezpro@gmail.com) 

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

#include "DebugApi.h"


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*
==================
Init
==================
*/
bool DebugApi::Init ()
{
	End ();
	InitVars ();

	// File
	mCout = new ofstream ("debug.log", ios::out); 

	// Time
	time_t mT;
	time (&mT);
	struct tm *mPetm = localtime (&mT);

	// :D

	*mCout << endl;

	*mCout << "                         ''~``                         " << endl;
	*mCout << "                        ( o o )                        " << endl;
	*mCout << "+------------------.oooO--(_)--Oooo.------------------+" << endl;
	*mCout << "|                                                     |" << endl;
	*mCout << "|                ®-------------------®                |" << endl;
	*mCout << "|                | I N D I E  L I B  |                |" << endl;
	*mCout << "|                ®-------------------®                |" << endl;
	*mCout << "|                    .oooO                            |" << endl;
	*mCout << "|                    (   )   Oooo.                    |" << endl;
	*mCout << "+---------------------\\ (----(   )--------------------+" << endl;
	*mCout << "                       \\_)    ) /                      " << endl;
	*mCout << "                             (_/                       " << endl;

	*mCout << endl;
	*mCout << endl;
	*mCout << "[Debug.log]:" << " (";

	// Date
	char *mDs [7] = {"Sunday","Monday","Tuesday","Wednesday","Thrusday","Friday","Saturday"};
	char *mMs [12] = {"January","February","March","April","May","June","July","August","September","October","November","December"};
			
	// Date
	*mCout << mDs [mPetm->tm_wday] << ", " << mPetm->tm_mday << " de " << mMs [mPetm->tm_mon] << " del " << mPetm->tm_year+1900 << ")" << endl << endl;

	mOk = true;

	return mOk;
}


/*
==================
End
==================
*/
void DebugApi::End()
{
	if (mOk)
	{
		mCout->close ();
		FreeVars ();
		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//							         Public methods
// --------------------------------------------------------------------------------


/*
==================
Header and message
==================
*/
void DebugApi::Header (char *pDataChar, int pType)
{
	if (!mOk) return;

	switch (pType)
	{
		// Ok
		case (1):
		{
			// Line
			*mCout << "          ";	
			*mCout << " [  OK   ] ";			
			Advance ();
			*mCout << pDataChar << endl;

			break;
		}
		// Error
		case (2):
		{
			// Line
			*mCout << "          ";
			*mCout << " [ ERROR ] ";			
			Advance ();
			*mCout << pDataChar << endl;

			// If we are inside a BEGIN / END, we go out
			if (mDepth > 0) 
			{
				// Going back
				mDepth -= ESP;
				// Close bracket
				*mCout << "                     ";
				Advance ();
				*mCout << "}" << endl;

				// Line
				WriteTime ();
				*mCout << " [  END  ] ";
				Advance ();
				*mCout << "Error occurred";
				
				// Measure the time between BEGIN and END
				unsigned long mElapsedTime = GetTickCount() - mTableTime [(mDepth + ESP) / ESP];
				if (mElapsedTime < 0) mElapsedTime = 0; // Medida de seguridad
				*mCout << " [Elaped time = " << mElapsedTime * 0.001f << " seg]" << endl;

				// Line jump after BEGIN/END
				if (!mDepth)
				{
					*mCout << "---------------------------------------------------------------------" << endl;
				}
			}
		
			break;

		}
		// Info
		// Info dosen't make a line jump in order DataChar and DataInt could write just after that line
		case (3):
		{
			// Line
			*mCout << "          ";
			*mCout << " [ INFO  ] ";		
			Advance ();
			*mCout << pDataChar;

			break;
		}
		// Warning
		case (4):
		{
			// Line
			*mCout << "          ";
			*mCout << " [WARNING] ";			
			Advance ();
			*mCout << pDataChar << endl;

			break;
		}
		// Begin
		case (5):
		{
			// Line
			WriteTime ();		
			*mCout << " [ BEGIN ] ";
			Advance ();
			*mCout << "-- " << pDataChar << " --" << endl;

			// Open brackets
			*mCout << "                     ";
			Advance ();
			*mCout << "{" << endl;

			// Advance
			mDepth += ESP;

			// Store the current time in the time table
			mTableTime [mDepth / ESP] = GetTickCount();
	
			break;
		}
		// End
		case (6):
		{
			// Going back
			mDepth -= ESP;
			// Close bracket
			*mCout << "                     ";
			Advance ();
			*mCout << "}" << endl;

			// Line
			WriteTime ();
			*mCout << " [  END  ] ";
			Advance ();
			*mCout << pDataChar;

			// Measure the time between BEGIN and END
			unsigned long mElapsedTime = GetTickCount() - mTableTime [(mDepth + ESP) / ESP];
			if (mElapsedTime < 0) mElapsedTime = 0; // Security Measure
			*mCout << " [Elapsed time = " << mElapsedTime * 0.001f << " seg]" << endl;
	
			// Line jump after BEGIN/END
			if (!mDepth)
			{		
				*mCout << "---------------------------------------------------------------------" << endl;			
			}
					
			break;
		}
	}
}


/*
==================
Writes a string
==================
*/
void DebugApi::DataChar (char *pDataChar, bool pFlag)
{
	if (!mOk) return;

	*mCout << " " << pDataChar;
	// Line jump
	if (pFlag)
		*mCout << endl;
}


/*
==================
Writes an integer
==================
*/
void DebugApi::DataInt (int pDataInt, bool pFlag)
{
	if (!mOk) return;

	*mCout << " " << pDataInt;
	// Jump line
	if (pFlag)
		*mCout << endl;
}


/*
==================
Writes a float
==================
*/
void DebugApi::DataFloat (float pDataFloat, bool pFlag)
{
	if (!mOk) return;

	*mCout << " " << pDataFloat;
	// Line jump
	if (pFlag)
		*mCout << endl;
}


/*
==================
Writes a DWORD
==================
*/
void DebugApi::DataDWORD (DWORD pDataDWORD, bool pFlag)
{
	if (!mOk) return;

	*mCout << " " << pDataDWORD;
	// Jump line
	if (pFlag)
		*mCout << endl;
}


/*
==================
Writes a signal (for debugging purposes)
==================
*/
void DebugApi::BreakPoint ()
{
	if (!mOk) return;

	*mCout << "Abracadabra";
	*mCout << endl;
}


/*
==================
Duplicates an string
==================
*/
char *DebugApi::DuplicateCharString (const char* charString)
{
    if (!charString)
		return 0;

	size_t len=strlen(charString) + 1;
	char *newString = new char[len];
	memcpy(newString, charString, len*sizeof(char));

	return newString;
}

// --------------------------------------------------------------------------------
//										 Private methods
// --------------------------------------------------------------------------------

/*
==================
Writes the current time
==================
*/
void DebugApi::WriteTime ()
{
	time_t t;
	time(&t);
	struct tm* petm = localtime(&t);

	// Hour
	*mCout << "[";

	// Hours
	if (petm->tm_hour < 10)
		*mCout << "0";
	*mCout << petm->tm_hour << ":";

	// Minutes
	if (petm->tm_min < 10)
		*mCout << "0";
	*mCout << petm->tm_min << ":";

	// Secods
	if (petm->tm_sec < 10)
		*mCout << "0";
	*mCout << petm->tm_sec << "]";
}


/*
==================
Advance as many spaces as Depth
==================
*/
void DebugApi::Advance ()
{
	for (int i = 0; i < mDepth; i++)
		*mCout << " ";
}


/*
==================
Starts to measure the time
==================
*/
void DebugApi::Start ()
{
	mTime = GetTickCount();
}


/*
==================
Stops measuring the time
==================
*/
void DebugApi::Stop ()
{
	unsigned long mElapsedTime = mTime - GetTickCount();
	if (mElapsedTime < 0) mElapsedTime = 0;
	mElapsedTime = mElapsedTime * (unsigned long) 0.001f;
	*mCout << mElapsedTime << endl;	
}


/*
==================
Draws all the characteres, included UNICODE
==================
*/
void DebugApi::AllFont ()
{
	for (int i = 0; i < 256; i++)
		*mCout << (char) i;
	*mCout << endl;
}


/*
==================
Init vars
==================
*/
void DebugApi::InitVars ()
{
	mDepth = 0;
	mTime = 0;

	for (int i = 0; i < 16; i++)
		mTableTime [i] = 0;
}


/*
==================
Free memory
==================
*/
void DebugApi::FreeVars ()
{
	Dispose (mCout);
}
