/*****************************************************************************************
/* File: IndieLib.cpp
/* Desc: IndieLib initialization / destruction
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

// ----- Includes -----

#include "Global.h"

// ----- Librerias -----

// Devil lib (for loading images and applying filters)
#pragma comment(lib, "devil.lib")
#pragma comment(lib, "ilu.lib")
#include "il/il.h"
#include "il/ilu.h"

// SDL lib (for input, timers and window creation)
#pragma comment(lib, "sdl.lib")
#include "sdl.h"

// ----- Header -----

#include "IndieLib.h"


// --------------------------------------------------------------------------------
//							 Initialization /Destruction
// --------------------------------------------------------------------------------

/*! 
\arg \b pMode				::IND_InitializationMode. See ::IND_InitializationMode.

\b Operation:  
 
This function initializes IndieLib.
*/
void IndieLib::Init (IND_InitializationMode pMode)
{
	// Debug initilization
	if (pMode)	Debug->Init ();

	// Devil initialization
	ilInit ();
	iluInit();

	// Flip the image
	ilEnable (IL_ORIGIN_SET);

	// SDL initialization
	//_putenv("SDL_VIDEODRIVER=windib");
	SDL_Init (SDL_INIT_TIMER);
}


/*! 
\b Operation:  
 
This function finalizes IndieLib and frees all the memory of the managers. So, all the resources like textures,
3d meshes, etc. will be freed.
*/
void IndieLib::End ()
{
	Debug->End();
	Dispose (Debug);
	SDL_Quit();
}
