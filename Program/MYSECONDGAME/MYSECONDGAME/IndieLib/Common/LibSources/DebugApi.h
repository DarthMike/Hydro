/*****************************************************************************************
/* File: DebugApi.h
/* Desc: Clase que gestiona la salida de información referente a la aplicación
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

#ifndef _DEBUGAPI_H_
#define _DEBUGAPI_H_

// Usada para GetTickCount (TODO: tengo que buscar uno alternativo pensando en la portabilidad)
#include <windows.h> 
#include <time.h>
#include "../LibHeaders/Defines.h"

#define ESP 3

#include <fstream>
using namespace std;

class DebugApi
{
public:

	// ----- Init/End -----

	DebugApi		(): mOk(false)	{ }
	~DebugApi		()				{ End(); }

	bool	Init	();
	void	End		();
	bool	IsOk	()			 const { return mOk; }

	// ----- Public methods -----

	void Header			(char *pData, int pType);
	void DataChar		(char *pDataChar, bool pFlag);
	void DataInt		(int  pDataInt, bool pFlag);
	void DataFloat		(float pDataFloat, bool pFlag);
	void DataDWORD		(DWORD pDataDWORD, bool pFlag);
	void BreakPoint		();
	char *DuplicateCharString (const char* charString);

private:

	// ----- Private -----

	bool mOk;

	// Output debug file
	ofstream *mCout;

	// Depeth (increases with each  "{" and go down with each "}")
	int mDepth;

	// Time table. After each BEGIN we introduce in this table taking in count the depth variable
	// the current time. When we make the END, we substract in order to measure the time that have passed
	// between the BEGIN and the END
	// It is possible to make a total of 16 BEGIN/END
	unsigned long mTableTime [16];

	// Used for start/stop
	unsigned long mTime;

	void WriteTime	();
	void Advance	();
	void Start		();
	void Stop		();
	void AllFont	();

	void InitVars	();
	void FreeVars	();
};

#endif // _DEBUGAPI_H_
