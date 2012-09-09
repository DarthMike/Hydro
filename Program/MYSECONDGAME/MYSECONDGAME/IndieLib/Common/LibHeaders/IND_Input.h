/*****************************************************************************************
/* File: IND_Input.h
/* Desc: Input class (wrapping SDL input)
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

#ifndef _IND_INPUT_
#define _IND_INPUT_

// ----- Includes -----

#include "IND_Timer.h"
#include "IND_Render.h"

// --------------------------------------------------------------------------------
//									      CKey
// --------------------------------------------------------------------------------

// This class stores information about a keyboard key
class CKey
{
public:

	// ----- Init/End -----

	CKey ()	
	{	
		mTimer.Start ();
		mKeyState = IND_KEY_NOT_PRESSED;
	}

	~CKey	()	{}

	// ----- Public methods -----

	// Sets the key state
	void SetState (IND_KeyState pKeyState) 
	{
		// If the key was pressed and is not pressed anymore we set the flag "released"
		if (mKeyState == IND_KEY_PRESSED && pKeyState == IND_KEY_NOT_PRESSED)
			mReleased = 1;

		// If the key was not pressed and now is pressed we set the flag "pressed"
		if (mKeyState == IND_KEY_NOT_PRESSED && pKeyState == IND_KEY_PRESSED)	
			mPressed = 1;	

		// Set the new state
		mKeyState = pKeyState;

		// If the key is pressed, we reset the timer
		if (pKeyState == IND_KEY_PRESSED) 
		{
			mTimer.Start ();
		}
	}

	bool mReleased;
	bool mPressed;
	IND_Key mKey;
	IND_KeyState mKeyState;
	IND_Timer mTimer;
};


// --------------------------------------------------------------------------------
//									      CMouse
// --------------------------------------------------------------------------------

// This class stores information about a mouse button
class CMouseButton
{
public:

	// ----- Init/End -----

	CMouseButton	()	
	{	
		mTimer.Start ();
		mButtonState = IND_MBUTTON_NOT_PRESSED;	
	}

	~CMouseButton	()	{}

	// ----- Methods -----

	// Sets the button state
	void SetState (IND_MouseButtonState pButtonState) 
	{
		// If the button was pressed and is not being pressed anymore we set the flag "released"
		if (mButtonState == IND_MBUTTON_PRESSED && pButtonState == IND_MBUTTON_NOT_PRESSED)	
			mReleased = 1;				

		// If the button was not pressed and is now being pressed we set the flag "pressed"
		if (mButtonState == IND_MBUTTON_NOT_PRESSED && pButtonState == IND_MBUTTON_PRESSED)	
			mPressed = 1;
			
		// Sets the new state
		mButtonState = pButtonState;

		// If the button is pressed, we reset the timer
		if (pButtonState == IND_MBUTTON_PRESSED) 
		{
			mTimer.Start ();
		}
	}
	
	bool mPressed;
	bool mReleased;
	IND_MouseButton mButton;
	IND_MouseButtonState mButtonState;
	IND_Timer mTimer;
};


// Class that stores information about the mouse
class CMouse
{
public:

	// ----- Init/End -----

	CMouse	()	{}
	~CMouse	()	{}

	int mMouseX, mMouseY;
	bool mMouseMotion;

	CMouseButton mMouseButtons [5];
};


// --------------------------------------------------------------------------------
//									 IND_Input
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Input IND_Input
\ingroup Input
IND_Input class for having input from keyboard and mouse. Click in IND_Input to see all the methods of this class.
*/
/*@{*/

/*!
\b IND_Input is a wrapper class of SDL input functions for giving IndieLib the possibility of
keyboard and mouse Input.
*/
class DLL_EXP IND_Input
{
public:

	// ----- Init/End -----

	IND_Input		(): mOk(false)						{ }
	~IND_Input		()									{ End(); }

	bool	Init    (IND_Render *pRender);
	void	End     ();
	bool	IsOk	() { return mOk; }
		
	// ----- Public methods -----

	/** @name Input common
	* 
	*/
	//@{
	void Update		();
	bool Quit		();
	//@}

	/** @name Keyboard
	* 
	*/
	//@{
	bool OnKeyPress				(IND_Key pKey);
	bool OnKeyRelease			(IND_Key pKey);
	bool IsKeyPressed			(IND_Key pKey);
	bool IsKeyPressed			(IND_Key pKey, unsigned long pTime);
	//@}

	/** @name Mouse
	* 
	*/
	//@{
	bool IsMouseMotion			();
	int GetMouseX				();
	int GetMouseY				();
	bool OnMouseButtonPress		(IND_MouseButton pMouseButton);
	bool OnMouseButtonRelease	(IND_MouseButton pMouseButton);
	bool IsMouseButtonPressed	(IND_MouseButton pMouseButton);
	bool IsMouseButtonPressed	(IND_MouseButton pMouseButton, unsigned long pTime);
	//@}

private:

	// ----- Private -----
	
	IND_Render *mRender;
	bool mOk;
	bool mQuit;

	// ----- Objects -----

	CKey mKeys [132];
	CMouse mMouse;
	
	// ----- Private methods -----

	void				InitFlags		();
	void				InitVars		();
	void				FreeVars		();
};
/*@}*/

#endif // _IND_INPUT_
