/*
	Filename: GameKeyBoard.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to keyboard input
	Comments: It depends of IndieLib, as it will be library used for input
	          The keyboard is used to control things by the user. In this game, it controls the main character,
			  a squishy blob of water! ;)
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*TODO:
	- Merge both input controllers classes by creating an interface class, deriving both gamemouse and gamekeyboard
	  from it... but to make it work hastily is good as-is. (no 2 players expected, neither change of controller so..)
	- Generally speaking, input should be decoupled from which library is used. Throughout the game code it is not 
	  done as I dont expect to change things and libraries in short time. but... it is TODO.
*/


#ifndef _GAMEKEYBOARD
#define _GAMEKEYBOARD

//Library dependencies

//Class dependencies
#include "Vector2.h"

//Forward declarations
class GameOverlay;

class GameKeyBoard
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameKeyBoard(GameOverlay* overlayptr):
	mMoveDirection(0,0),
	mOverlay(overlayptr)
	{	
	}
	virtual ~GameKeyBoard()
	{
	}

	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	void Update(float dt);		//Update method
	void Render();				//Render necessary elements
	//----- PUBLIC VARIABLES ------

protected:
	//----- INTERNAL VARIABLES -----
	GameOverlay* mOverlay;
	Vector2 mMoveDirection;
	bool mFreeCameraMode;
	//----- INTERNAL FUNCTIONS -----
};

#endif