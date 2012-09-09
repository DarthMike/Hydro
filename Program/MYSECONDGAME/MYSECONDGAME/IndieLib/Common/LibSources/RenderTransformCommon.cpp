/*****************************************************************************************
/* File: RenderTransformCommon.cpp
/* Desc: Transformations applied before blitting objects usind D3D
/*****************************************************************************************/

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

// ----- Includes -----

#include "Global.h"
#include "Defines.h"
#include "IND_Render.h"

// --------------------------------------------------------------------------------
//							         Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pR                  Byte R (Red)
\arg \b pG                  Byte G (Green)
\arg \b pB                  Byte B (Blue)

\b Operation:  
 
Clean the viewport with a color expressed in RGB
*/
inline void	IND_Render::ClearViewPort	(byte pR, 
										 byte pG, 
										 byte pB)
{
	mInfo.mDevice->Clear (0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB (pR, pG, pB), 1.0f, 0);
}


/*!
\b Parameters:

\arg \b pEyeX, pEyeY, pEyeZ							Position of the camera
\arg \b pLookAtX, pLookAtY, pLookAtZ				Vector defining the direction of the camera
\arg \b pUpX, pUpY, pUpZ							Vector "up" of the camera.

Operation:  
 
This function sets a perspective matrix through the definition of a position and two vectors (lookat and up).

Using this method is equivalent to using a combination of methods of the class ::SetCamera3d().
*/
inline void IND_Render::LookAt			(float pEyeX, float pEyeY, float pEyeZ,
										float pLookAtX, float pLookAtY, float pLookAtZ,
										float pUpX, float pUpY, float pUpZ)
{
	// ----- View matrix -----

	D3DXMATRIX mMatView;
	D3DXVECTOR3 mEyePt		(pEyeX, pEyeY, pEyeZ);
	D3DXVECTOR3 mLookatPt	(pLookAtX, pLookAtY, pLookAtZ);
	D3DXVECTOR3 mUpVec		(pUpX, pUpY, pUpZ);
	D3DXMatrixLookAtLH		(&mMatView, &mEyePt, &mLookatPt, &mUpVec);

	mInfo.mDevice->SetTransform (D3DTS_VIEW, &mMatView);
}


/*!
\b Parameters:

\arg \b pFov										Vertical fov
\arg \b pAspect										Aspect ratio (usually the width of the viewport divided by the height)
\arg \b pNearClippingPlane							Near clipping plane
\arg \b pFarClippingPlane							Far clipping plane

Operation:  
 
This function sets a fov projection matrix.

This method is equivalent to use a combination of methods of the class ::SetCamera3d().
*/
inline void IND_Render::PerspectiveFov	(float pFov, float pAspect, float pNearClippingPlane, float pFarClippingPlane)
{
	// ----- Projection matrix -----

	D3DXMATRIX mMatProj;

	D3DXMatrixPerspectiveFovLH(&mMatProj,				// output
							   pAspect,					// Fov vertical
							   pAspect,					// Viewport aspect ratio
							   pNearClippingPlane,		// Near clipping plane z
							   pFarClippingPlane);		// Far clipping  plane z


	mInfo.mDevice->SetTransform(D3DTS_PROJECTION, &mMatProj);
}


/*!
\b Parameters:

\arg \b pWidth										Width 
\arg \b pHeight										Height
\arg \b pNearClippingPlane							Near clipping plane
\arg \b pFarClippingPlane							Far clipping plane

Operation:  
 
This function sets a orthographic projection matrix.

Using this method is equivalent to using a combination of the methods of the class ::SetCamera3d().
*/
inline void IND_Render::PerspectiveOrtho	(float pWidth, float pHeight, float pNearClippingPlane, float pFarClippingPlane)
{
	D3DXMATRIX mMatProj;
	D3DXMatrixOrthoRH (&mMatProj, pWidth, pHeight, pNearClippingPlane, pFarClippingPlane);
	mInfo.mDevice->SetTransform(D3DTS_PROJECTION, &mMatProj);
}


// --------------------------------------------------------------------------------
//							         Private methods
// --------------------------------------------------------------------------------

/*
==================
IND_BlendingType to D3DBLEND conversion
==================
*/
inline D3DBLEND IND_Render::GetD3DBlendingType (IND_BlendingType pBlendingType)
{
	switch (pBlendingType)
	{
		case IND_ZERO:				return D3DBLEND_ZERO;
		case IND_ONE:				return D3DBLEND_ONE;
		case IND_SRCCOLOR:			return D3DBLEND_SRCCOLOR;
		case IND_INVSRCCOLOR:		return D3DBLEND_INVSRCCOLOR;
		case IND_SRCALPHA:			return D3DBLEND_SRCALPHA;
		case IND_INVSRCALPHA:		return D3DBLEND_INVSRCALPHA;
		case IND_DESTALPHA:			return D3DBLEND_DESTALPHA;
		case IND_INVDESTALPHA:		return D3DBLEND_INVDESTALPHA;
		case IND_DESTCOLOR:			return D3DBLEND_DESTCOLOR;
		case IND_INVDESTCOLOR:		return D3DBLEND_INVDESTCOLOR;
		case IND_SRCALPHASAT:		return D3DBLEND_SRCALPHASAT;
		case IND_BOTHSRCALPHA:		return D3DBLEND_BOTHSRCALPHA;
		case IND_BOTHINVSRCALPHA:	return D3DBLEND_BOTHINVSRCALPHA;
		case IND_BLENDFACTOR:		return D3DBLEND_BLENDFACTOR;
		case IND_INVBLENDFACTOR:	return D3DBLEND_INVBLENDFACTOR;
			
		default:					return D3DBLEND_SRCALPHA;
	}
}


/*
==================
IND_BlendingType to D3DBLEND conversion
==================
*/
inline D3DTEXTUREFILTERTYPE IND_Render::GetD3DFilter (IND_Filter pFilter)
{
	switch (pFilter)
	{
		case IND_FILTER_NONE:				return D3DTEXF_NONE;
		case IND_FILTER_POINT:				return D3DTEXF_POINT;
		case IND_FILTER_LINEAR:				return D3DTEXF_LINEAR;
			
		default:							return D3DTEXF_LINEAR;
	}
}


/*
==================
Transforms an IndieLib matrix into a D3DXMATRIX
==================
*/
inline void IND_Render::GetD3DMatrix (IND_Matrix pMatrix, D3DXMATRIX *pD3DMatrix)
{
	pD3DMatrix->_11 = pMatrix._11;
	pD3DMatrix->_12 = pMatrix._12;
	pD3DMatrix->_13 = pMatrix._13;
	pD3DMatrix->_14 = pMatrix._14;
	
	pD3DMatrix->_21 = pMatrix._21;
	pD3DMatrix->_22 = pMatrix._22;
	pD3DMatrix->_23 = pMatrix._23;
	pD3DMatrix->_24 = pMatrix._24;

	pD3DMatrix->_31 = pMatrix._31;
	pD3DMatrix->_32 = pMatrix._32;
	pD3DMatrix->_33 = pMatrix._33;
	pD3DMatrix->_34 = pMatrix._34;

	pD3DMatrix->_41 = pMatrix._41;
	pD3DMatrix->_42 = pMatrix._42;
	pD3DMatrix->_43 = pMatrix._43;
	pD3DMatrix->_44 = pMatrix._44;
}
