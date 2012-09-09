/*****************************************************************************************
/* File: IND_Window.cpp
/* Desc: Manages the window (uses SDL for creating the window)
/*****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (info@pixelartgames.com) 

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
#include "sdl.h"
#include "SDL_syswm.h"
#include "IND_Window.h"


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pTitle                  Title of the window
\arg \b pWidth                  Width of the window
\arg \b pHeight                 Height of the window
\arg \b pBpp                   Quality of color (32 o 16 bits). 32 bits offers better quality. 16 bits offers better speed.
\arg \b pVsync                  Waiting for the vertical sync. (1 / 0) = (on / off).
\arg \b pFullscreen             Full screen. (1 / 0) = (on / off).

\b Operation:   

This function returns 1 (true) if a window with the attributes indicated as parameters is created.
*/
bool IND_Window::Init (char *pTitle, 
					   int pWidth, 
					   int pHeight, 
					   int pBpp, 
					   bool pVsync, 
					   bool pFullscreen)
{
	End ();
	InitVars ();

	if (CreateMyWindow (pTitle, pWidth, pHeight, pBpp, pVsync, pFullscreen))
		mOk = true;
	else
		mOk = false;

	return mOk;
}


/*!
\b Operation:

This function frees the manager and all the objects that it contains.
*/
void IND_Window::End()
{
	if (mOk)
	{		
		Debug ->Header ("Finalizing the window", 5);		
		FreeVars ();
		Debug ->Header ("Window finalized", 6);
		
		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//									 Public methods
// --------------------------------------------------------------------------------


/*!
\b Parameters:

\arg \b pTitle                         Title of the window

\b Operation:   

This function changes the title of the actual window.
*/
void IND_Window::SetTitle (char *pTitle)
{
	mAttributes.mTitle = pTitle;
	SDL_WM_SetCaption (pTitle, NULL); 
}


/*!
\b Parameters:

\arg \b pEnable                         Show cursor (1 / 0) = (on / off)

\b Operation:   

This function shows or hides the cursor.
*/
void IND_Window::Cursor (bool pEnable)
{
	if (pEnable)
		SDL_ShowCursor (SDL_ENABLE); 
	else
		SDL_ShowCursor (SDL_DISABLE); 
}


// --------------------------------------------------------------------------------
//									 Private methods
// --------------------------------------------------------------------------------

/*
==================
Creates a window
==================
*/
bool IND_Window::CreateMyWindow (char* pTitle, int pWidth, int pHeight, int pBpp, bool pVsync, bool pFullscreen)
{
	if (!Create (pTitle, pWidth, pHeight, pBpp, pVsync, pFullscreen))
		return 0;

	return 1;
}


/*
==================
Creates a window
==================
*/
bool IND_Window::Create (char *pTitle, int pWidth, int pHeight, int pBpp, bool pVsync, bool pFullscreen)
{
	Debug->Header ("Creating SDL window", 5);

	mAttributes.mWidth		= pWidth;
	mAttributes.mHeight		= pHeight;
	mAttributes.mBits		= pBpp;
	mAttributes.mVsync		= pVsync;
	mAttributes.mFullscreen	= pFullscreen;
	mAttributes.mTitle		= pTitle;

	if (!pFullscreen)
	{
		// Windowed mode
		if (!SDL_SetVideoMode (pWidth, pHeight, pBpp, 0))
		{
			Debug->Header ("Error creating SDL window", 2);
			return 0;
		}
	}
	else
	{
		// Full screen mode
		if (!SDL_SetVideoMode (pWidth, pHeight, pBpp, SDL_FULLSCREEN))
		{
			Debug->Header ("Error creating SDL window", 2);
			return 0;
		}
	}

	// Obtain HWND
	SDL_SysWMinfo WMInfo;
	SDL_VERSION	(&WMInfo.version);
	SDL_GetWMInfo (&WMInfo);
	mAttributes.mWnd = WMInfo.window;
	//mAttributes.mWnd = GetActiveWindow();

	// ---- Center window -----

	int mX = GetSystemMetrics (SM_CXSCREEN);
	int mY = GetSystemMetrics (SM_CYSCREEN);

	int mCenterX = abs (mX - pWidth) / 2;
    int mCenterY = abs (mY - pHeight) / 2;

	SetWindowPos (mAttributes.mWnd, 0, mCenterX, mCenterY, pWidth, pHeight, 0 );

	// Hide cursor
	SDL_ShowCursor (SDL_DISABLE);

	// Set Title
	SDL_WM_SetCaption (pTitle, NULL); 

	if (!mAttributes.mWnd) 
	{	
		Debug->Header ("Error creating the window", 2);
		return 0;
	}
	Debug->Header ("Window created", 1);


	Debug->Header ("Mode:", 3);
	if (!pFullscreen)
		Debug->DataChar ("Window", 1);
	else
		Debug->DataChar ("Full screen", 1);
	
	Debug->Header ("Window OK", 6);

	return 1;
}


/*
==================
Window destruction
==================
*/
void IND_Window::DestroyIndieLibWindow ()
{	
	
}


/*
==================
Init attributes
==================
*/
void IND_Window::InitAttribWindow ()
{
	mAttributes.mWidth		= 0;
	mAttributes.mHeight		= 0;
	mAttributes.mBits		= 0;
	mAttributes.mFullscreen	= false;
	mAttributes.mTitle		= "";
	mAttributes.mWnd		= NULL;
}


/*
==================
Init vars
==================
*/
void IND_Window::InitVars ()
{
	InitAttribWindow ();
}


/*
==================
Free memory
==================
*/
void IND_Window::FreeVars ()
{
	DestroyIndieLibWindow ();
}
