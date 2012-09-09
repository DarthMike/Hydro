/*****************************************************************************************
/* File: IND_Render.cpp
/* Desc: Initialization / Destructi�n usind D3D
/*****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier L�pez L�pez (info@pixelartgames.com) 

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
#include "IND_SurfaceManager.h"
#include "sdl.h"
#include "IND_Render.h"
#include <d3d9.h> 
#include <d3dx9.h>

// ----- Libs -----

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

// ----- Defines -----

#define VENDORID_NVIDIA 0x000010de
#define VENDORID_ATI	0x00001002 


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pWindow                 Pointer to an object initializing IND_Window 

\b Operation:   

This function returns 1 (true) if the render is initialized sucessfully, 
0 (false) otherwise.
*/
bool IND_Render::Init (IND_Window *pWindow)
{
	End ();
	InitVars ();

	if (!CreateRender (pWindow))
		mOk = false;
	else
		mOk = true;

	// Timer
	mLastTime = 0;
	mLast = 0;

	// Fps counter
	mFpsCounter = 0;
	mCurrentTimeFps = 0;
	mLastTimeFps = 0;
	mLastFps = 0;

	// Window
	mWindow = pWindow;

	return mOk;
}


/*!
\b Parameters:

\arg \b pDirect3d             Pointer to a Direct3d render
\arg \b pD3dDevice            Pointer to  the Direct3d Device

\b Operation:   

This function returns 1 (true) if the render is initialized sucessfully, 
0 (false) otherwise.
*/
bool IND_Render::Init	(LPDIRECT3D9 pDirect3d, IDirect3DDevice9 *pD3dDevice)
{
	Debug->Header ("Initializing Direct3D", 5);

	// Fill Info
	mInfo.mDirect3d	= pDirect3d;
	mInfo.mDevice	= pD3dDevice;

	// Witdh and Height of the backbuffer
	D3DSURFACE_DESC mSurfaceBackBuffer;
	LPDIRECT3DSURFACE9 mBackBuffer;
    mInfo.mDevice->GetBackBuffer (0, 0, D3DBACKBUFFER_TYPE_MONO, &mBackBuffer);
    mBackBuffer->GetDesc (&mSurfaceBackBuffer);
    mBackBuffer->Release();
	mInfo.mFBWidth  = mSurfaceBackBuffer.Width;
	mInfo.mFBHeight = mSurfaceBackBuffer.Height;

	// System info
	GetInfo ();

	// Hardware info
	WriteInfo ();

	mOk = true;

	// ViewPort
	SetViewPort2d (0, 0, mInfo.mFBWidth, mInfo.mFBHeight);
	IND_Camera2d mCamera2d (mInfo.mViewPortWidth / 2, mInfo.mViewPortHeight / 2);
	SetCamera2d (&mCamera2d);
	ClearViewPort (0, 0, 0);

	// Timer
	mLastTime = 0;
	mLast = 0;

	// Fps counter
	mFpsCounter = 0;
	mCurrentTimeFps = 0;
	mLastTimeFps = 0;
	mLastFps = 0;

	Debug->Header ("Direct3D OK", 6);

	return mOk;
}


/*!
\b Parameters:

\arg \b pTitle                  Title of the window
\arg \b pWidth                  Width of the window
\arg \b pHeight                 Height of the window
\arg \b pBpp                    Quality of color (32 o 16 bits). 32 bits offers better quality. 16 bits offers better speed.
\arg \b pVsync                  Wait for vertical sync. (1 / 0) = (on / off).
\arg \b pFullscreen             Full screen. (1 / 0) = (on / off).

\b Operation:   

This function returns 1 (true) if the application window resets to the attributes passed as parameters. This method
is useful when you want to change the application window on runtime, 0 (false) if it is not possible to create
the new window.
*/
bool IND_Render::Reset (char *pTitle, int pWidth, int pHeight, int pBpp, bool pVsync, bool pFullscreen)
{
	// Delete Window
	mWindow->End();

	// Create Window
	if (!mWindow->Init (pTitle, pWidth, pHeight, pBpp, pVsync, pFullscreen)) return 0;

	// Reset device
	if (!Direct3dReset (pWidth, pHeight, pBpp, pVsync, pFullscreen)) return 0;

	return 1;
}


/*!
\b Operation:   

This function returns 1 (true) if the application window toggles to fullscreen or windowed, 0 (false) if it is not possible
to create the new window.
*/
bool IND_Render::ToggleFullScreen ()
{
	char *mTitle		= mWindow->GetTitle();
	int mWidth			= mWindow->GetWidth();
	int mHeight			= mWindow->GetHeight();
	int mBpp			= mWindow->GetBpp();
	bool mFullScreen	= mWindow->IsFullScreen();
	bool mVsync			= mWindow->IsVsync();

	// Delete Window
	mWindow->End();

	// Fullscreen to windowed
	if (mFullScreen)
	{
		if (!Direct3dReset (mWidth, mHeight, mBpp, mVsync, 0)) return 0;
		if (!mWindow->Init (mTitle, mWidth, mHeight, mBpp, mVsync, 0)) return 0;
		
	}
	// Windowed to fullscreen
	else
	{
		if (!Direct3dReset (mWidth, mHeight, mBpp, mVsync, 1)) return 0;
		if (!mWindow->Init (mTitle, mWidth, mHeight, mBpp, mVsync, 1)) return 0;
	}

	return 1;
}


/*!
\b Operation:

This function frees the manager and all the objects that it contains.
*/
void IND_Render::End()
{
	if (mOk)
	{		
		Debug->Header ("Finalizing Direct3D", 5);
		FreeVars ();
		Debug->Header ("Direct3D finalized ", 6);	
		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//							        Public methods
// --------------------------------------------------------------------------------

/*!
\b Operation:

Preparing for render. This function must be called before drawing any graphical object.
*/
inline void IND_Render::BeginScene ()
{
	if (!mOk)	
		return;

	static int mFlag = 0;

	// ----- Time counter -----

	mLast = ((float) SDL_GetTicks() - mLastTime);
	mLastTime = (float) SDL_GetTicks();

	// ----- Fps counter ------

    mFpsCounter++;
	mCurrentTimeFps = (float) SDL_GetTicks();	

	// After each second passed
    if (mCurrentTimeFps - mLastTimeFps > 1000)
    {
		mLastTimeFps = mCurrentTimeFps;
		mLastFps = mFpsCounter;
        mFpsCounter = 0;
    }

	// ----- Recovering device (after ALT+TAB) -----

	if (GetDevice()->TestCooperativeLevel () == D3DERR_DEVICENOTRESET)
	{
		// Reset device
		if (GetDevice()->Reset (&mPresentParameters) != D3D_OK) exit (0);

		// Reset view
		SetViewPort2d (0, 0, mInfo.mViewPortWidth, mInfo.mViewPortHeight );
		IND_Camera2d mCamera2d (mInfo.mViewPortWidth / 2, mInfo.mViewPortHeight / 2);
		SetCamera2d (&mCamera2d);

		ResetTimer	();
	}

	mInfo.mDevice->BeginScene();
}
 

/*!
\b Operation:

Finish the scene. This function must be called after drawing all the graphical objects. 
*/
inline void IND_Render::EndScene ()
{
	if (!mOk)	
		return;
	
	mInfo.mDevice->EndScene();
	mInfo.mDevice->Present (NULL, NULL, NULL, NULL);
}


/*!
\b Operation:

This function shows the fps (frames per second) as the title of the window.

NOTE: The updating of the window title is quite time-consuming, so this is not the correct method for
checking the FPS. It's better to use the methods IND_Render::GetFpsInt() or IND_Render::GetFpsString() and drawing
the result using an ::IND_Font object.
*/
inline void IND_Render::ShowFpsInWindowTitle ()
{
	if (!mOk)	return;

	char fps_string[MAX_CHARS_IN_INT32_STR];
	GetFpsString(fps_string);
	mWindow->SetTitle (fps_string);
}


// --------------------------------------------------------------------------------
//							        Private methods
// --------------------------------------------------------------------------------

/*
==================
Init Direct3D
==================
*/	
bool IND_Render::Direct3DInit (int pWidth, 
							   int pHeight, 
							   int pBpp, 
							   bool pVsync, 
							   bool pFullscreen)
{	
	// Direct3D creation
	if (!(mInfo.mDirect3d = Direct3DCreate9 (D3D_SDK_VERSION)))
	{	
		Debug->Header ("Error creating D3D object", 2);
		return 0;
	}
	else
		Debug->Header ("Creating D3D object", 1);

	// Windowed
	if (!pFullscreen) 
	{
		if ((mInfo.mDirect3d->GetAdapterDisplayMode (D3DADAPTER_DEFAULT, &mDisplayMode)) != D3D_OK)
		{
			Debug->Header ("Error obtaining the adapter", 2);
			return 0;
		}
		else
			Debug->Header ("Obtaining the adapter", 1);
	}
	else
	// Full screen
	{
		mDisplayMode.Width = pWidth; 
		mDisplayMode.Height = pHeight; 
		mDisplayMode.RefreshRate = 0; 

		if (pBpp == 32)
			mDisplayMode.Format = D3DFMT_A8R8G8B8;
		else
			mDisplayMode.Format = D3DFMT_R5G6B5;
		
		Debug->Header ("Obtaining the adapter", 1);
	}
	
	ZeroMemory (&mPresentParameters, sizeof(mPresentParameters));

	// Windowed
	if (!pFullscreen) 
	{ 
		mPresentParameters.Windowed = 1; 
	}
	// Full screen
	else 
	{
		mPresentParameters.Windowed   = 0; 
		mPresentParameters.BackBufferWidth  = mDisplayMode.Width;		
		mPresentParameters.BackBufferHeight = mDisplayMode.Height;
	}

	//MIGUEL MODIFICATION
	mPresentParameters.BackBufferCount  = 1;
	mPresentParameters.SwapEffect       = D3DSWAPEFFECT_DISCARD;
	mPresentParameters.BackBufferFormat = mDisplayMode.Format;
	

	// Depth buffer
	mPresentParameters.EnableAutoDepthStencil = true;					//own depth and stencil format
	mPresentParameters.AutoDepthStencilFormat = D3DFMT_D16;				//depth stencil format

	/*
	// Antialiasing
	DWORD mQualityLevels;

	D3DCAPS9 pCaps;
	mInfo.mDirect3d->GetDeviceCaps (D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &pCaps);

	if	(SUCCEEDED (mInfo.mDirect3d->CheckDeviceMultiSampleType	(pCaps.AdapterOrdinal, 
																pCaps.DeviceType, 
																mDisplayMode.Format, 
																mPresentParameters.Windowed, 
																D3DMULTISAMPLE_2_SAMPLES, 
																&mQualityLevels)))
	{
		Debug->Header ("Iniciando Antialiasing", 1);
		mPresentParameters.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
		mPresentParameters.MultiSampleQuality = mQualityLevels;
	}
	*/

	// Vsync
	if (!pVsync)
		mPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// We try to create the device using hardware vertex processing, if it is not possible
	// we use software vertex processing
	Debug->Header ("Creating the device (D3DCREATE_HARDWARE_VERTEXPROCESSING)", 1);

	if ((mInfo.mDirect3d->CreateDevice (D3DADAPTER_DEFAULT, 
										D3DDEVTYPE_HAL, 
										mWnd,
										D3DCREATE_HARDWARE_VERTEXPROCESSING,
										&mPresentParameters, 
										&mInfo.mDevice)) != D3D_OK) 
	{
		Debug->Header ("Not possible to create the device (D3DCREATE_HARDWARE_VERTEXPROCESSING)", 1);		
		Debug->Header ("Creating the device (D3DCREATE_SOFTWARE_VERTEXPROCESSING) instead", 1);

		if ((mInfo.mDirect3d->CreateDevice (D3DADAPTER_DEFAULT, 
											D3DDEVTYPE_HAL, 
											mWnd,
											D3DCREATE_SOFTWARE_VERTEXPROCESSING,
											&mPresentParameters, 
											&mInfo.mDevice)) != D3D_OK) 
		{
			Debug->Header ("Error creating the Device (D3DCREATE_SOFTWARE_VERTEXPROCESSING)", 2);
			return 0;
		}
		else
		{
			mInfo.mSoftwareVertexProcessing = 1;
		}
	}
	else
	{
		mInfo.mSoftwareVertexProcessing = 0;
	}

	// System info
	GetInfo ();

	mInfo.mFBWidth  = pWidth;
	mInfo.mFBHeight = pHeight;

	// Video mode
	Debug->Header	("Video mode:", 3);
	Debug->DataInt	(pWidth, 0);
	Debug->DataChar ("x", 0);
	Debug->DataInt	(pHeight, 0);
	Debug->DataChar ("x", 0);
	Debug->DataInt	(pBpp, 1);

	SetViewPort2d (0, 0, pWidth, pHeight);
	IND_Camera2d mCamera2d (mInfo.mViewPortWidth / 2, mInfo.mViewPortHeight / 2);
	SetCamera2d (&mCamera2d);
	ClearViewPort (0, 0, 0);

	return 1;
}


/*
==================
Reset Direct3D
==================
*/	
bool IND_Render::Direct3dReset	(int pWidth, 
								int pHeight, 
								int pBpp, 
								bool pVsync, 
								bool pFullscreen)
{	
	Debug->Header ("Reseting Direct3D", 5);

	// Windowed
	if (!pFullscreen) 
	{
		if ((mInfo.mDirect3d->GetAdapterDisplayMode (D3DADAPTER_DEFAULT, &mDisplayMode)) != D3D_OK)
		{
			Debug->Header ("Error obtaining the adapter", 2);
			return 0;
		}
		else
			Debug->Header ("Obtaining the adapter", 1);
	}
	else
	// Full screen
	{
		mDisplayMode.Width = pWidth; 
		mDisplayMode.Height = pHeight; 
		mDisplayMode.RefreshRate = 0; 

		if (pBpp == 32)
			mDisplayMode.Format = D3DFMT_A8R8G8B8;
		else
			mDisplayMode.Format = D3DFMT_R5G6B5;
		
		Debug->Header ("Obtaining the adapter", 1);
	}
	
	ZeroMemory (&mPresentParameters, sizeof(mPresentParameters));

	// Windowed
	if (!pFullscreen) 
	{ 
		mPresentParameters.Windowed = 1; 
	}
	// Full screen
	else 
	{
		mPresentParameters.Windowed   = 0;
		mPresentParameters.BackBufferWidth  = mDisplayMode.Width;		
		mPresentParameters.BackBufferHeight = mDisplayMode.Height;
	}

	mPresentParameters.SwapEffect       = D3DSWAPEFFECT_DISCARD;
	mPresentParameters.BackBufferFormat = mDisplayMode.Format;


	// Depth buffer
	mPresentParameters.EnableAutoDepthStencil = true;					//own depth and stencil format
	mPresentParameters.AutoDepthStencilFormat = D3DFMT_D16;				//depth stencil format

	// Vsync
	if (!pVsync)
		mPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;


	// Reset Device
	bool mExit = 0;
	while (!mExit)
	{
		HRESULT mResult = GetDevice()->Reset (&mPresentParameters);
		if (mResult != D3D_OK)
		{
			// HERE IS THE PROBLEM !!!!

			/*
			switch (mResult)
			{
				case D3DERR_DEVICELOST: Debug->DataChar ("D3DERR_DEVICELOST", 1); break;
				case D3DERR_DRIVERINTERNALERROR: Debug->DataChar ("D3DERR_DRIVERINTERNALERROR", 1); break;
				case D3DERR_INVALIDCALL: Debug->DataChar ("D3DERR_INVALIDCALL", 1); break;
				case D3DERR_OUTOFVIDEOMEMORY: Debug->DataChar ("D3DERR_OUTOFVIDEOMEMORY", 1); break;
				case E_OUTOFMEMORY: Debug->DataChar ("E_OUTOFMEMORY", 1); break;
				default: Debug->DataChar ("Unknown", 1); break;
			}

			exit (0);
			*/

			/*
			while (GetDevice()->TestCooperativeLevel () != D3DERR_DEVICENOTRESET)
			{
				Sleep (10);
			}
			*/
		}
		else
		{
			mExit = 1;
		}
	}

	// System info
	GetInfo ();

	mInfo.mFBWidth  = pWidth;
	mInfo.mFBHeight = pHeight;

	// ViewPort
	SetViewPort2d (0, 0, mInfo.mFBWidth, mInfo.mFBHeight);
	IND_Camera2d mCamera2d (mInfo.mViewPortWidth / 2, mInfo.mViewPortHeight / 2);
	SetCamera2d (&mCamera2d);
	ClearViewPort (0, 0, 0);

	// Video mode
	Debug->Header	("Video mode:", 3);
	Debug->DataInt	(pWidth, 0);
	Debug->DataChar ("x", 0);
	Debug->DataInt	(pHeight, 0);
	Debug->DataChar ("x", 0);
	Debug->DataInt	(pBpp, 1);

	Debug->Header ("Direct3D reseted", 6);

	// Reset timer
	ResetTimer();

	return 1;
}


/*
==================
Creates the render 
==================
*/
bool IND_Render::CreateRender (IND_Window *pWindow)
{
	Debug->Header ("Initializing Direct3D", 5);

	// If the window is correctly initialized
	if (pWindow->IsOk ())
	{
		mWnd				= pWindow->GetWnd	();
		int mWidthWin		= pWindow->GetWidth	();
		int mHeightWin		= pWindow->GetHeight();
		int mBitsWin		= pWindow->GetBpp	();
		bool mVsync			= pWindow->IsVsync	();
		bool mFullscreen	= pWindow->IsFullScreen();

		// Direct3d initialization
		if (!Direct3DInit (mWidthWin, mHeightWin, mBitsWin, mVsync, mFullscreen))
		{
			Debug->Header ("Finalizing D3D", 5);
			FreeVars ();
			Debug->Header ("D3D finalized", 6);
			return 0;
		}

		Debug->Header ("Direct3D OK", 6);
		
		// Hardware information
		WriteInfo ();
		
		return 1;
	}
	
	// Window error
	Debug->Header ("This operation can not be done:", 3);
	Debug->DataChar ("", 1);
	Debug->Header ("Invalid Id or IND_Window not correctly initialized.", 2);

	return 0;
}


/*
==================
Graphics card information
==================
*/	
void IND_Render::GetInfo ()
{
	// Adapter
	D3DADAPTER_IDENTIFIER9 *mAdapter = new D3DADAPTER_IDENTIFIER9;
	mInfo.mDirect3d->GetAdapterIdentifier (0, 0, mAdapter);

	// ----- d3d Version -----

	strcpy (mInfo.mVersion, "DXSDK November 2007");

	// ----- Vendor -----	
	
	unsigned long mVId = mAdapter->VendorId;

	switch (mVId)
	{
		case VENDORID_NVIDIA:
		{
			strcpy (mInfo.mVendor, "Nvidia");
			break;
		}
		case VENDORID_ATI:
		{
			strcpy (mInfo.mVendor, "ATI Technologies Inc");
			break;
		}
		default:
		{
			strcpy (mInfo.mVendor, "?");
			break;
		}
	}	

	// ----- Renderer -----

	// Chip
	strcpy (mInfo.mRenderer, mAdapter->Description);

	// Free D3DADAPTER_IDENTIFIER9 adapter
	Dispose (mAdapter);

	// Caps
	D3DCAPS9 mD3dcap;
	mInfo.mDevice->GetDeviceCaps (&mD3dcap);

	// Antialiasing D3DRS_ANTIALIASEDLINEENABLE
	if (mD3dcap.LineCaps & D3DLINECAPS_ANTIALIAS) mInfo.mAntialiasing = 1;

	// Max texture size
	mInfo.mMaxTextureSize = mD3dcap.MaxTextureWidth;

	// Vertex Shader version
	mInfo.mVertexShaderVersion = mD3dcap.VertexShaderVersion;

	// Pixel Shader version
	mInfo.mPixelShaderVersion = mD3dcap.PixelShaderVersion;
}


/*
==================
Writes the information into the debug field
==================
*/	
void IND_Render::WriteInfo ()
{
	Debug->Header ("Hardware information" , 5);

	// ----- D3D version -----

	Debug->Header ("Direct3D version:" , 3);
	Debug->DataChar (mInfo.mVersion, 1);

	// ----- Vendor -----	
	
	Debug->Header ("Mark:" , 3);
	Debug->DataChar (mInfo.mVendor, 1);

	// ----- Renderer -----

	Debug->Header ("Chip:" , 3);
	Debug->DataChar (mInfo.mRenderer, 1);

	// ----- Antialiasing -----

	Debug->Header ("Primitive antialising (D3DRS_ANTIALIASEDLINEENABLE):", 3);
	if (mInfo.mAntialiasing) 
		Debug->DataChar ("Yes", 1);
	else
		Debug->DataChar ("No", 1);

	// ----- Max texture size -----

	Debug->Header ("Maximum texture size:" , 3);
	Debug->DataInt (mInfo.mMaxTextureSize, 0);
	Debug->DataChar ("x", 0);
	Debug->DataInt (mInfo.mMaxTextureSize, 1);

	// ----- Vertex Shader version  -----

	Debug->Header ("Vertex Shader:" , 3);
	Debug->DataInt (D3DSHADER_VERSION_MAJOR (mInfo.mVertexShaderVersion), 0);
	Debug->DataChar (".", 0);
	Debug->DataInt (D3DSHADER_VERSION_MINOR (mInfo.mVertexShaderVersion), 0);

	if (mInfo.mSoftwareVertexProcessing)
		Debug->DataChar ("(Software)", 1);
	else
		Debug->DataChar ("", 1);

	// ----- Pixel Shader version -----

	Debug->Header ("Pixel Shader:" , 3);
	Debug->DataInt (D3DSHADER_VERSION_MAJOR (mInfo.mPixelShaderVersion), 0);
	Debug->DataChar (".", 0);
	Debug->DataInt (D3DSHADER_VERSION_MINOR (mInfo.mPixelShaderVersion), 1);

	Debug->Header ("Hardware Ok" , 6);	
}


/*
==================
Destroys the renderer 
==================
*/
void IND_Render::DestroyD3DWindow ()
{
	// Free device
	if (mInfo.mDevice)
		mInfo.mDevice->Release();
	mInfo.mDevice = NULL;
	Debug ->Header ("Finalizing the Device", 1);

	// Free D3D object
	if (mInfo.mDirect3d)
		mInfo.mDirect3d->Release();
	mInfo.mDirect3d = NULL;
	Debug ->Header ("Finalizing D3D object", 1);
}


/*
==================
Reset the timing. This method is used after recovering the focus
==================
*/
void IND_Render::ResetTimer	()
{
	// Reset timing
	mLast = 0;
	mLastTime = (float) SDL_GetTicks();
}


/*
==================
Init render attributes
==================
*/
void IND_Render::InitAttribRender ()
{
	mInfo.mVersion [0]		= 0;					
	mInfo.mVendor [0]		= 0;					
	mInfo.mRenderer [0]		= 0;	
	mInfo.mMaxTextureSize	= 0;
}


/*
==================
Init vars
==================
*/
void IND_Render::InitVars ()
{
	InitAttribRender ();
}


/*
==================
Free memory
==================
*/
void IND_Render::FreeVars ()
{	
	// Destroy the render
	DestroyD3DWindow ();
}