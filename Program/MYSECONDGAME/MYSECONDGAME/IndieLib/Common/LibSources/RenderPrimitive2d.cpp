/*****************************************************************************************
/* File: RenderPrimitive2d.cpp
/* Desc: Primitive drawing using D3D
/* TODO: Polygons blitted from a loaded xml, suffering transformations, etc...
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
#include "IND_SurfaceManager.h"
#include "IND_Render.h"



// --------------------------------------------------------------------------------
//							         Public methods
// --------------------------------------------------------------------------------

/*
==================
TODO: CHECK THE USE OF THIS METHOD IN THE FUTURE, CURRENTLY IS DEPRECATED. ¿LET IT BE UNTIL HAVING PIXEL SHADER SUPPORT?

\b Parameters:

\b pSwitch       Activates or deactivates the antialiasing. (true = antialiasing on, false = antialiasing of)

Operation:  
 
This method activates or deativates the antialiasing when drawing primitives. It doesn't affect
to other graphical objects, only to primitives.

This function will return 1 if the antialiasing is activated or deactivated correctly and 0 if the
graphic card of the user doesn't support this feature.
==================
*/
inline bool IND_Render::SetAntialiasing	(bool pSwitch)
{
	if (!mInfo.mAntialiasing) return 0;

	if (pSwitch) 
	{
		mInfo.mDevice->SetRenderState (D3DRS_ANTIALIASEDLINEENABLE, TRUE);
	}
	else
	{
		mInfo.mDevice->SetRenderState (D3DRS_ANTIALIASEDLINEENABLE, FALSE);
	}
		
	return 1;
}

/*!
\defgroup Primitives Bliting Primitives
\ingroup Advances
With these methods you can directly blit to the screen primitives using IND_Render class. Remember that you can also use IND_Entity2d with primitives joined to this object, in order to
draw primitives.
*/
/*@{*/

/*!
\b Parameters:

\arg \b pX, \b pY				Position in the screen
\arg \b pR, \b pG, \b pB		R, G, B components of the color
\arg \b pA						Level of transparency. (255 = completly opaque)

\b Operation:

This function draws a pixel into the screen. This is a really slow method when the number of pixels is
big.

This method is equivalent to use a combination of these methods:
- IND_Entity2d::SetPrimitive2d()
- IND_Entity2d::SetLine()
- IND_Entity2d::SetTint()
- IND_Entity2d::SetTransparency()
*/
inline void IND_Render::BlitPixel	(int pX, 
									int pY, 
									byte pR, 
									byte pG, 
									byte pB,
									byte pA)
{
	
	// Fill the PIXEL structure
	FillPixel (&mPixels [0], pX, pY, pR, pG, pB);

	// Transformation
	SetForPrimitive (pA);

	// Pixel drawing
	mInfo.mDevice->DrawPrimitiveUP (D3DPT_POINTLIST, 1, &mPixels, sizeof (PIXEL));
}


/*!
\b Parameters:

\arg \b pX1, \b pY1				Origin point
\arg \b pX2, pY2				Destiny point
\arg \b pR, \b pG, \b pB		R, G, B components of the color
\arg \b pA						Level of transparency. (255 = completly opaque)

\b Operation:

This function draws a line into the screen

Using this method is equivalent to using all of these methods:
- IND_Entity2d::SetPrimitive2d()
- IND_Entity2d::SetLine()
- IND_Entity2d::SetTint()
- IND_Entity2d::SetTransparency()
*/
inline void IND_Render::BlitLine	(int pX1, 
									int pY1,
									int pX2,
									int pY2,
									byte pR, 
									byte pG, 
									byte pB,
									byte pA)
{
	// Fill the PIXEL structure
	FillPixel (&mPixels [0], pX1, pY1, pR, pG, pB);
	FillPixel (&mPixels [1], pX2, pY2, pR, pG, pB);

	// Transformation
	SetForPrimitive (pA);
	
	// Line blitting
	mInfo.mDevice->DrawPrimitiveUP (D3DPT_LINESTRIP, 1, &mPixels, sizeof (PIXEL));
}


/*!
\b Parameters:

\arg \b pX1, \b pY1				Upper left corner of the rectangle
\arg \b pX2, \b pY2				Lower right corner of the rectangle                       
\arg \b pR, \b pG, \b pB		R, G, B components of the color
\arg \b pA						Level of transparency. (255 = completly opaque)

\b Operation:

This function draws a rectangle into the screen

Using this method is equivalent to using all of these methods:
- IND_Entity2d::SetPrimitive2d()
- IND_Entity2d::SetRectangle()
- IND_Entity2d::SetTint()
- IND_Entity2d::SetTransparency()
*/
inline void IND_Render::BlitRectangle	(int pX1, 
										int pY1,
										int pX2,
										int pY2,
										byte pR, 
										byte pG, 
										byte pB,
										byte pA)
{
	// Fill PIXEL structures
	FillPixel (&mPixels [0], pX1, pY1,		pR, pG, pB);
	FillPixel (&mPixels [1], pX2, pY1,		pR, pG, pB);
	FillPixel (&mPixels [2], pX2, pY2,		pR, pG, pB);
	FillPixel (&mPixels [3], pX1, pY2,		pR, pG, pB);
	FillPixel (&mPixels [4], pX1, pY1,		pR, pG, pB);

	// Transformation
	SetForPrimitive (pA);

	// Rectangle blitting
	mInfo.mDevice->DrawPrimitiveUP (D3DPT_LINESTRIP, 4, &mPixels, sizeof (PIXEL));
}


/*!
\b Parameters:

\arg \b pX1, \b pY1					Upper left corner of the rectangle
\arg \b pX2, \b pY2					Lower right corner of the rectangle                       
\arg \b pR, \b pG, \b pB			R, G, B components of the color
\arg \b pA							Level of transparency. (255 = completly opaque)

\b Operation:

This function draws a rectangle filled with a color into the screen. The A component is the
transparency level (255 = complety opaque).

Using this method is equivalent to using all of these methods:
- IND_Entity2d::SetPrimitive2d()
- IND_Entity2d::SetRectangle()
- IND_Entity2d::SetTint()
- IND_Entity2d::SetTransparency()
*/
inline void IND_Render::BlitFillRectangle	(int pX1, 
											int pY1,
											int pX2,
											int pY2,
											byte pR, 
											byte pG, 
											byte pB,
											byte pA)
{
	// Fill PIXEL structures
	FillPixel (&mPixels [0], pX2, pY1,		pR, pG, pB);
	FillPixel (&mPixels [1], pX2, pY2,		pR, pG, pB);
	FillPixel (&mPixels [2], pX1, pY1,		pR, pG, pB);
	FillPixel (&mPixels [3], pX1, pY2,		pR, pG, pB);

	// Transformation
	SetTransform2d (0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0);

	// Color transformation
	SetForPrimitive (pA);

	// Rectangle blitting
	mInfo.mDevice->DrawPrimitiveUP (D3DPT_TRIANGLESTRIP, 2, &mPixels, sizeof (PIXEL));
}

/*!
\b Parameters:

\arg \b pTrianglePoints				Triangle Points allocated array
\arg \b pNumPoints				    Number of points passed (numtriangles =  pNumPoints - 2)
\arg \b pR, \b pG, \b pB			R, G, B components of the color in outer vertexs
\arg \b pA							Level of transparency. (255 = completly opaque)

\b Operation:

This function draws a complete triangle fan. The triangle fan is a set of triangles which
share a central vertex (http://msdn.microsoft.com/en-us/library/ee422512(VS.85).aspx)
The A parameter is transparency (255 = complety opaque).
*/
inline void IND_Render::BlitTriangleList   (IND_Point *pTrianglePoints, 
											int pNumPoints,
											byte pR, 
											byte pG, 
											byte pB,
											byte pA)
{
	//TODO: CHECK MAX POLYGONS PER CALL...  mD3dcap.MaxPrimitiveCount
	if(pNumPoints < 3)
		return;

	//LOOP - Fill pixels structure
	for(int i = 0; i < pNumPoints;i++)
	{
		FillPixel(&mPixels[i], pTrianglePoints[i].x,pTrianglePoints[i].y,pR,pG,pB);
	}//LOOP END

	//Transformation
	SetForPrimitive(pA);

	//Blitting
	mInfo.mDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,pNumPoints-2,&mPixels, sizeof(PIXEL));
/*

	// Fill PIXEL structures
	for (int i = 0; i < pNumLines + 1; i++)
		FillPixel (&mPixels [i], pPolyPoints [i].x, pPolyPoints [i].y, pR, pG, pB);

	// Transformation
	SetForPrimitive (pA);

	// Polygon blitting
	mInfo.mDevice->DrawPrimitiveUP (D3DPT_LINESTRIP, pNumLines, &mPixels, sizeof (PIXEL));
*/
	/*// Fill PIXEL structures
	FillPixel (&mPixels [0], pX2, pY1,		pR, pG, pB);
	FillPixel (&mPixels [1], pX2, pY2,		pR, pG, pB);
	FillPixel (&mPixels [2], pX1, pY1,		pR, pG, pB);
	FillPixel (&mPixels [3], pX1, pY2,		pR, pG, pB);

	// Transformation
	SetTransform2d (0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0);

	// Color transformation
	SetForPrimitive (pA);

	// Rectangle blitting
	mInfo.mDevice->DrawPrimitiveUP (D3DPT_TRIANGLESTRIP, 2, &mPixels, sizeof (PIXEL));*/
}

/********************************************************************************/

/*!
\b Parameters:

\arg \b pX1, \b pY1						Triangle corner #1
\arg \b pX2, \b pY2						Triangle corner #2        
\arg \b pX3, \b pY3						Triangle corner #3 
\arg \b pR1, \b pG1, \b pB1				R, G, B components of the color for corner #1
\arg \b pR2, \b pG2, \b pB2				R, G, B components of the color for corner #2
\arg \b pR3, \b pG3, \b pB3				R, G, B components of the color for corner #3	
\arg \b pZ								Z depth of coords in all triangle
\arg \b pA								Level of transparency. (255 = completly opaque)

\b Operation:

This function draws a triangle filled with a color given in three corners. The A component is the
transparency level (255 = complety opaque).
*/

inline void IND_Render::BlitColoredTriangle	(int pX1, 
											int pY1,
											int pX2,
											int pY2,
											int pX3,
											int pY3,
											byte pR1, byte pG1, byte pB1,
											byte pR2, byte pG2, byte pB2,
											byte pR3, byte pG3, byte pB3, 
											byte pA)
{
	// Fill PIXEL structures
	FillPixel (&mPixels [0], pX1, pY1, pR1, pG1, pB1);
	FillPixel (&mPixels [1], pX2, pY2, pR2, pG2, pB2);
	FillPixel (&mPixels [2], pX3, pY3, pR3, pG3, pB3);

	// Transformation
	SetTransform2d (0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0);

	// Color transformation
	SetForPrimitive (pA);

	// Rectangle blitting
	mInfo.mDevice->DrawPrimitiveUP (D3DPT_TRIANGLELIST, 1, &mPixels, sizeof (PIXEL));
}


/*!
\b Parameters:

\arg \b pPixel						Pointer to a points array ::IND_Point. Example: ::IND_Point mPoly3 [ ] = { {60, 10},  {20, 15},  {50, 90},  {170, 190} } => Sets 3 points (each one with x and y coordinates).
\arg \b pNumLines					Number of edges to draw
\arg \b pR, \b pG, \b pB			R, G, B components of the color
\arg \b pA							Level of transparency. (255 = completly opaque)

\b Operation:

This function draws a 2d poly

Using this method is equivalent to using all of these methods:
- IND_Entity2d::SetPrimitive2d()
- IND_Entity2d::SetPolyPoints()
- IND_Entity2d::SetNumSides()
- IND_Entity2d::SetTint()
- IND_Entity2d::SetTransparency()
*/
inline bool IND_Render::BlitPoly2d	(IND_Point *pPolyPoints, 
									int pNumLines,
									byte pR, 
									byte pG, 
									byte pB,
									byte pA)
{
	if (!pPolyPoints)	return 0;
	if (pNumLines < 1)	return 0;

	// Fill PIXEL structures
	for (int i = 0; i < pNumLines + 1; i++)
		FillPixel (&mPixels [i], pPolyPoints [i].x, pPolyPoints [i].y, pR, pG, pB);

	// Transformation
	SetForPrimitive (pA);

	// Polygon blitting
	mInfo.mDevice->DrawPrimitiveUP (D3DPT_LINESTRIP, pNumLines, &mPixels, sizeof (PIXEL));

	return 1;
}


/*!
\b Parameters:

\arg \b pX, \b pY                   Position in the screen
\arg \b pRadius						Radius
\arg \b pN							Number of sides
\arg \b pAngle						Angle in degrees (if you change this parameter the polygon 
									will rotate)
\arg \b pR, \b pG, \b pB			R, G, B components of the color
\arg \b pA							Level of transparency. (255 = completly opaque)

\b Operation:

This function draws 2d regunr poly of n sides. If you need to draw circles you can use this method 
using 30 or more sides.

This method is equivalent to use a combination of these methods:
- IND_Entity2d::SetPrimitive2d()
- IND_Entity2d::SetRadius()
- IND_Entity2d::SetPosition()
- IND_Entity2d::SetTint()
- IND_Entity2d::SetTransparency()
*/
inline bool IND_Render::BlitRegularPoly	 (int pX, 
										  int pY,
										  int pRadius,
										  int pN,
										  float pAngle,
										  byte pR, 
										  byte pG, 
										  byte pB,
										  byte pA)
{	
	int x, y, i;

	for (i = 0; i < pN; i++)
	{
		float c = i * 2 * (float) (PI / pN);
		x = (int) ( pX + (pRadius * cos (c + D3DXToRadian (pAngle))) );
		y = (int) ( pY + (pRadius * sin (c + D3DXToRadian (pAngle))) );

		FillPixel (&mPixels [i], x, y, pR, pG, pB);
	}

	FillPixel (&mPixels [i], pX + (int) (pRadius * cos (D3DXToRadian (pAngle))), pY + (int) (pRadius * sin (D3DXToRadian (pAngle))), pR, pG, pB);

	// Transformation
	SetForPrimitive (pA);

	// Polygon blitting
	mInfo.mDevice->DrawPrimitiveUP (D3DPT_LINESTRIP, i, &mPixels, sizeof (PIXEL));	
	
	return 1;
}
/*@}*/


/*
==================
Blits a bounding circle area
==================
*/
inline void IND_Render::BlitCollisionCircle (int pPosX, int pPosY, int pRadius, float pScale,  byte pR, byte pG, byte pB, byte pA, IND_Matrix pIndWorldMatrix)
{
	if (pScale != 1.0f) pRadius = (int) (pRadius * pScale);

	SetTransform2d (0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0);

	D3DXMATRIX mWorldMatrix;
	GetD3DMatrix (pIndWorldMatrix, &mWorldMatrix);

	// Untransformed points
	D3DXVECTOR2 mP1Untransformed ((float) pPosX, (float) pPosY);			

	D3DXVECTOR4 mP1;
	D3DXVec2Transform (&mP1, &mP1Untransformed, &mWorldMatrix);

	// Color
	SetRainbow2d (IND_OPAQUE, 1, 0, 0, IND_FILTER_POINT, pR, pG, pB, pA, 0, 0, 0, 255, 0, 0);
	
	// Color
	mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

	// Pixel format
	mInfo.mDevice->SetFVF (D3DFVF_PIXEL);

	// Blitting
	int x, y, i;

	for (i = 0; i < 30; i++)
	{
		float c = i * 2 * (float) (PI / 30);
		x = (int) ( mP1.x + (pRadius * cos (c + D3DXToRadian (0))) );
		y = (int) ( mP1.y + (pRadius * sin (c + D3DXToRadian (0))) );

		FillPixel (&mPixels [i], x, y, pR, pG, pB);
	}

	FillPixel (&mPixels [i], (int) mP1.x + (int) (pRadius * cos (D3DXToRadian (0))), (int) mP1.y + (int) (pRadius * sin (D3DXToRadian (0))), pR, pG, pB);

	// Blitting circle
	mInfo.mDevice->DrawPrimitiveUP (D3DPT_LINESTRIP, i, &mPixels, sizeof (PIXEL));	
}


/*
==================
Blits a bounding line
==================
*/
inline void IND_Render::BlitCollisionLine (int pPosX1, int pPosY1, int pPosX2, int pPosY2,  byte pR, byte pG, byte pB, byte pA, IND_Matrix pIndWorldMatrix)
{
	SetTransform2d (0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0);

	D3DXMATRIX mWorldMatrix;
	GetD3DMatrix (pIndWorldMatrix, &mWorldMatrix);

	// Untransformed points
	D3DXVECTOR2 mP1Untransformed ((float) pPosX1, (float) pPosY1);			
	D3DXVECTOR2 mP2Untransformed ((float) pPosX2, (float) pPosY2);	

	D3DXVECTOR4 mP1, mP2;
	D3DXVec2Transform (&mP1, &mP1Untransformed, &mWorldMatrix);
	D3DXVec2Transform (&mP2, &mP2Untransformed, &mWorldMatrix);
	
	// Color
	SetRainbow2d (IND_OPAQUE, 1, 0, 0, IND_FILTER_POINT, pR, pG, pB, pA, 0, 0, 0, 255, 0, 0);

	// Color
	mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

	// Pixel format
	mInfo.mDevice->SetFVF (D3DFVF_PIXEL);

	// Filling pixels
	FillPixel (&mPixels [0], (int) mP1.x, (int) mP1.y, pR, pG, pB);
	FillPixel (&mPixels [1], (int) mP2.x, (int) mP2.y, pR, pG, pB);

	// Blitting line
	mInfo.mDevice->DrawPrimitiveUP (D3DPT_LINESTRIP, 1, &mPixels, sizeof (PIXEL));
}


/*
==================
Blits a bounding line
==================
*/
inline void IND_Render::BlitGridLine (int pPosX1, int pPosY1, int pPosX2, int pPosY2,  byte pR, byte pG, byte pB, byte pA, D3DXMATRIX pWorldMatrix)
{
	SetTransform2d (0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0);

	// Untransformed Points
	D3DXVECTOR2 mP1Untransformed ((float) pPosX1, (float) pPosY1);			
	D3DXVECTOR2 mP2Untransformed ((float) pPosX2, (float) pPosY2);	

	D3DXVECTOR4 mP1, mP2;
	D3DXVec2Transform (&mP1, &mP1Untransformed, &pWorldMatrix);
	D3DXVec2Transform (&mP2, &mP2Untransformed, &pWorldMatrix);
	
	// Color
	SetRainbow2d (IND_OPAQUE, 1, 0, 0, IND_FILTER_POINT, pR, pG, pB, pA, 0, 0, 0, 255, 0, 0);

	// Color
	mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

	// Pixel format
	mInfo.mDevice->SetFVF (D3DFVF_PIXEL);

	// Filling pixels
	FillPixel (&mPixels [0], (int) mP1.x, (int) mP1.y, pR, pG, pB);
	FillPixel (&mPixels [1], (int) mP2.x, (int) mP2.y, pR, pG, pB);

	// Blitting line
	mInfo.mDevice->DrawPrimitiveUP (D3DPT_LINESTRIP, 1, &mPixels, sizeof (PIXEL));
}


/*
==================
Blits quad of the grid of an IND_Surface
==================
*/
inline void IND_Render::BlitGridQuad	(int pAx, int pAy,
										int pBx, int pBy,
										int pCx, int pCy,
										int pDx, int pDy,
										byte pR, byte pG, byte pB, byte pA,
										D3DXMATRIX pWorldMatrix)
{
	BlitGridLine (pAx, pAy, pBx, pBy, pR, pG, pB, pA, pWorldMatrix);
	BlitGridLine (pBx, pBy, pDx, pDy, pR, pG, pB, pA, pWorldMatrix);
	BlitGridLine (pDx, pDy, pCx, pCy, pR, pG, pB, pA, pWorldMatrix);
	BlitGridLine (pCx, pCy, pAx, pAy, pR, pG, pB, pA, pWorldMatrix);
}



// --------------------------------------------------------------------------------
//							         Private methods
// --------------------------------------------------------------------------------

/*
==================
Fill PIXEL structures
==================
*/
inline void IND_Render::FillPixel	(PIXEL *pPixel, 
									int pX, 
									int pY,
									byte pR, 
									byte pG, 
									byte pB)
{
	// Pixel
	pPixel->mX		= (float) pX;
	pPixel->mY		= (float) pY;
	pPixel->mZ		= (float) 0.0f;
	pPixel->mColor	= D3DCOLOR_RGBA (pR, pG, pB, 255);
}

/*
==================
Transformations before drawing a primitive
==================
*/
inline void IND_Render::SetForPrimitive (byte pA)
{
	// Transformation
	SetTransform2d (0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0);

	// Antialiasing
	//if (mAntialiasing)
	//{
	//	exit (0);
		//SetRainbow (IND_ALPHA, 255, 255, 255, pA, 0, 0, 0, 255, 0, 0);
	//}
	//else
	//{
	//MIGUEL MODIFICATION (TRANSPARENCY!!)
	//IF - Totally opaque
	if(pA == 255)
	{
		SetRainbow2d (IND_OPAQUE, 1, 0, 0, IND_FILTER_POINT, 255, 255, 255, pA, 0, 0, 0, 255, 0, 0);
	}
	else // ELSE - Some transparency
	{
		SetRainbow2d (IND_ALPHA, 1, 0, 0, IND_FILTER_POINT, 255, 255, 255, pA, 0, 0, 0, 255, 0, 0);
	}
	//}
	
	// Disable color OP
	mInfo.mDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );

	// Pixel format
	mInfo.mDevice->SetFVF (D3DFVF_PIXEL);
}
