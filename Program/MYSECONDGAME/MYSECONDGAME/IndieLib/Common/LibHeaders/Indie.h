/*****************************************************************************************
/* File: IndieLib.h
/* Desc: Includes
/*****************************************************************************************/

#ifndef _INDIELIB_
#define _INDIELIB_

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

/*
	MODIFICATIONS FROM ORIGINAL: ALL MODIFS ARE MARKED WITH: //MIGUEL MODIFICATION
		- IND_RENDER METHOD: BlitTriangleList Added
		- IND_RENDER: Method SetForPrimitive was not adding IND_ALPHA to make transparencies when blitting!
		- IND_Entity2d: Method GetLayer() added, added int variable mLayer (to get layer of entity easily)
						Method SetLayer(int newlayer) added
		- IND_Entity2d: Method Add(int layer, IND_Entity2d* entity) changed to make a setlayer of the entity
		- IND_Entity2d: Method SetSequence (int pSequence) didnt restart previous animation when setting a new one
										                   this caused me a lot of problems having 1 replay more when 
														   changing animations and replaying no times (pReplays 1)
														   Also needed to put "Active" flag right on to 1, as from there, it 
														   must be considered that animation is active (coherence of flags)
		- IND_Entity2d: Method SetNumReplays(int pReplays  Had to change it as now, to replay, you put how many times 
															you want to replay. If parameter is 0, no replays are made
		- IND_RenderText2d: Method BlitText() Changed it to be able to scale the related entity and make the text bigger as it 
											  should be. Before scale didnt affect to text
*/

// Namespace
#include <windows.h>
#include <fstream>
using namespace std;

// Defines and types
#include "Defines.h"

// Init/End
#include "IndieLib.h"

// Display
#include "IND_Window.h"

// Render
#include "IND_Render.h"

// Timer
#include "IND_Timer.h"

// Math
#include "IND_Math.h"

// Entities
#include "IND_Entity2dManager.h"
#include "IND_Entity3dManager.h"

// Input
#include "IND_Input.h"

// 2d Objects
#include "IND_ImageManager.h"
#include "IND_SurfaceManager.h"
#include "IND_AnimationManager.h"
#include "IND_FontManager.h"

// 3d Objects
#include "IND_3dMeshManager.h"

// Cameras
#include "IND_Camera2d.h"
#include "IND_Camera3d.h"

// Common
#include "IND_LightManager.h"

#endif // _INDIELIB_


