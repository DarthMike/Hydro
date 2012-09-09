/*
	Filename: General_Resources.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Header file for general resources used by many classes in program 
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _GENERAL_RESOURCES
#define _GENERAL_RESOURCES

//GENERAL C++ LIBRARIES
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>

//STD LIBRARIES
#include <map>
#include <list>
#include <string>
#include <vector>
#include <sstream>

//Templates
#include "Singleton_Template.h"

//Game options
#include "ConfigOptions.h"

//Utility classes
#include "Math.h"
#include "LogManager.h" //(Singleton)
#include "Generic2DMatrix.h"
#include "Vector3.h"
#include "Vector2.h"

//Exception managing
#include "GenericException.h"

//Game-related definitions									  //CONTROL DEFINITION
typedef enum GameState{EDITOR,INTRO,MAINMENU,NORMALGAME,CREDITS,CONTROLS,EXIT,NOSTATE};

#endif