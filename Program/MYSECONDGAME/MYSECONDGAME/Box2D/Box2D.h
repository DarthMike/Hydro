/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef BOX2D_H
#define BOX2D_H

/**
\mainpage Box2D API Documentation

\section intro_sec Getting Started

For tutorials please see http://www.box2d.org/manual.html

For discussion please visit http://www.box2d.org/forum
*/

// These include files constitute the main Box2D API
/*	CHANGES FOR THIS PROJECT (HYDRO): (Marked as MIGUEL MODIFICATION)
	- METHOD DRAWDEBUGDATA IN B2WORLD IS PUBLIC TO BE ABLE TO DRAW WHEN YOU WANT. File: b2World.h
	- ACCESS TO LINEAR AND ANGULAR DAMPING VALUES IN B2BODY. File: b2Body.h
	- TEMPORALY MODIFIED b2Island.cpp TO DISABLE POSITION CORRECTION FOR ALL BODIES! CHANGE BACK!!!!
	- FORCES RESET FOR MANY STEPS IN ONE GAME TICK (WORLD::STEP AND iSLAND::SOLVE)
	  File: b2Island.h b2Island.cpp b2World.h b2World.cpp
	- MODIFY BODIES TO ADD A FLAG TO RESET POSITION CORRECTION (USED FOR SOFT BODIES ONLY) Files: b2Body.h b2Body.cpp 
	  THIS FLAG WILL DISABLE POSITION CORRECTION IN BODIES OF SOME ISLAND Files: b2Island.h b2Island.cpp b2World.cpp
*/

#include "Common/b2Settings.h"

#include "Collision/Shapes/b2CircleShape.h"
#include "Collision/Shapes/b2PolygonShape.h"
#include "Collision/Shapes/b2EdgeShape.h"
#include "Collision/b2BroadPhase.h"
#include "Dynamics/b2WorldCallbacks.h"
#include "Dynamics/b2World.h"
#include "Dynamics/b2Body.h"

#include "Dynamics/Contacts/b2Contact.h"

#include "Dynamics/Joints/b2DistanceJoint.h"
#include "Dynamics/Joints/b2GearJoint.h"
#include "Dynamics/Joints/b2LineJoint.h"
#include "Dynamics/Joints/b2MouseJoint.h"
#include "Dynamics/Joints/b2PrismaticJoint.h"
#include "Dynamics/Joints/b2PulleyJoint.h"
#include "Dynamics/Joints/b2RevoluteJoint.h"

#include "Dynamics/Controllers/b2BuoyancyController.h"
#include "Dynamics/Controllers/b2ConstantForceController.h"
#include "Dynamics/Controllers/b2ConstantAccelController.h"
#include "Dynamics/Controllers/b2GravityController.h"
#include "Dynamics/Controllers/b2TensorDampingController.h"

#endif
