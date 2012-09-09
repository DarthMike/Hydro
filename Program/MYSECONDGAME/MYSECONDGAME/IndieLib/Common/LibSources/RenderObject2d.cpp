/*****************************************************************************************
/* File: RenderObject2d.cpp
/* Desc: Blitting of 2d objects using D3D
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

/*!
\defgroup Graphical_Objects Bliting Surfaces, Animations and Fonts and setting the transformations directly
\ingroup Advances
*/
/*@{*/

/*!
\b Parameters:

\arg \b pSu						Pointer to a ::IND_Surface object

\b Operation:

This function blits directly to the screen a ::IND_Surface object. 

In order to change the transformations
and color attributes of the surface you have to use the IND_Render::SetTransform2d() and IND_Render::SetRainbow2d() methods before
calling this function. Remember that you can use IND_Entity2d object for drawing surfaces to the screen without having to use these
advanced methods directly. This method is only useful for advanced users with really concrete purposes.

Using this method is equivalent to using:
- IND_Entity2d::SetSurface()
*/
inline void IND_Render::BlitSurface (IND_Surface *pSu)
{
	// ----- Blitting -----

	mInfo.mDevice->SetFVF (D3DFVF_CUSTOMVERTEX2D);

	int mCont = 0;
	for (int i = 0; i < pSu->GetNumBlocks(); i++)
	{
		// ----- Transform 4 vertices of the quad into world space coordinates -----

		D3DXVECTOR4 mP1, mP2, mP3, mP4;
		Transform4Vertices ((float) pSu->GetVertexArray()[mCont].mX, (float) pSu->GetVertexArray()[mCont].mY,
							(float) pSu->GetVertexArray()[mCont + 1].mX, (float) pSu->GetVertexArray()[mCont + 1].mY,
							(float) pSu->GetVertexArray()[mCont + 2].mX, (float) pSu->GetVertexArray()[mCont + 2].mY,
							(float) pSu->GetVertexArray()[mCont + 3].mX, (float) pSu->GetVertexArray()[mCont + 3].mY,
							&mP1, &mP2, &mP3, &mP4);
		
		D3DXVECTOR3 mP1_f3 (mP1);
		D3DXVECTOR3 mP2_f3 (mP2);
		D3DXVECTOR3 mP3_f3 (mP3);
		D3DXVECTOR3 mP4_f3 (mP4);

		// Calculate the bounding rectangle that we are going to try to discard
		CalculateBoundingRectangle (&mP1_f3, &mP2_f3, &mP3_f3, &mP4_f3);

		// ---- Discard bounding rectangle using frustum culling if possible ----

		if (CullFrustumBox (mP1_f3, mP2_f3))
		{
			if (!pSu->IsHaveGrid())
				mInfo.mDevice->SetTexture (0, pSu->GetTextArray()[i].mTexture);	
			else
				mInfo.mDevice->SetTexture (0, pSu->GetTextArray()[0].mTexture);	

			mInfo.mDevice->DrawPrimitiveUP (D3DPT_TRIANGLESTRIP, 2, pSu->GetVertexArray() + mCont, sizeof (CUSTOMVERTEX2D));					

			mNumRenderedObjects++;
		}
		else
			mNumDiscardedObjects++;
			

		mCont += 4;
	}	
}


/*!
\b Parameters:

\arg \b pSu						Pointer to a ::IND_Surface object

\b Operation:

This function blits directly to the screen the grid of an ::IND_Surface object. 

Using this method is equivalent to using both of these methods:
- IND_Entity2dManager::RenderGridAreas()
- IND_Entity2d::ShowGridAreas()
*/
inline void IND_Render::BlitGrid (IND_Surface *pSu, byte pR, byte pG, byte pB, byte pA)
{
	D3DXMATRIX mMatWorld;
	mInfo.mDevice->GetTransform (D3DTS_WORLD, &mMatWorld);

	for (int i = 0; i < pSu->GetNumBlocks() * 4; i += 4)
	{
		// ----- Transform 4 vertices of the quad into world space coordinates -----

		D3DXVECTOR4 mP1, mP2, mP3, mP4;
		Transform4Vertices ((float) pSu->GetVertexArray()[i].mX, (float) pSu->GetVertexArray()[i].mY,
							(float) pSu->GetVertexArray()[i + 1].mX, (float) pSu->GetVertexArray()[i + 1].mY,
							(float) pSu->GetVertexArray()[i + 2].mX, (float) pSu->GetVertexArray()[i + 2].mY,
							(float) pSu->GetVertexArray()[i + 3].mX, (float) pSu->GetVertexArray()[i + 3].mY,
							&mP1, &mP2, &mP3, &mP4);
		
		D3DXVECTOR3 mP1_f3 (mP1);
		D3DXVECTOR3 mP2_f3 (mP2);
		D3DXVECTOR3 mP3_f3 (mP3);
		D3DXVECTOR3 mP4_f3 (mP4);

		// Calculate the bounding rectangle that we are going to try to discard
		CalculateBoundingRectangle (&mP1_f3, &mP2_f3, &mP3_f3, &mP4_f3);

		// ---- Discard bounding rectangle using frustum culling if possible ----

		if (CullFrustumBox (mP1_f3, mP2_f3))
		{
			BlitGridQuad	((int) pSu->GetVertexArray()[i].mX, (int) pSu->GetVertexArray()[i].mY,
							(int) pSu->GetVertexArray()[i+1].mX, (int) pSu->GetVertexArray()[i+1].mY,
							(int) pSu->GetVertexArray()[i+2].mX, (int) pSu->GetVertexArray()[i+2].mY,
							(int) pSu->GetVertexArray()[i+3].mX, (int) pSu->GetVertexArray()[i+3].mY,
							pR, pG, pB, pA,
							mMatWorld);
		}
	}
}


/*!
\b Parameters:

\arg \b pSu						Pointer to a ::IND_Surface object
\arg \b pX, \b pY				Upper left coordinate of the region
\arg \b pWidth, \b pHeight		Width and Height of the region

\b Operation:

This function returns 1 (true) if it blits directly to the screen a region of a ::IND_Surface object.

This method is useful when we want to render only a certain region of a ::IND_Surface.

If the region that we chose is out of the range of the sprite, the function will return false and no
region will be rendered. 

Special remark: this function only works with ::IND_Surface objects that only have ONE texture 
assigned (you can check this using::IND_Surface::GetNumTextures() method). So, it will work only 
with images that are power of two and lower than the maximum texture size allowed by your card 
(you can check this parameter using ::IND_Render::GetMaxTextureSize()). The method will return 0 
otherwise.

In order to change the transformations and color attributes of the surface you have to use the IND_Render::SetTransform2d() and IND_Render::SetRainbow2d() methods before
calling to this function. Remember that you can use IND_Entity2d object for drawing surfaces to the screen without having to use this
advanced methods directly. This method is only useful for advanced users with really concrete purposes.

Using this method is equivalent to using both of these methods:
- IND_Entity2d::SetSurface()
- IND_Entity2d::SetRegion()
*/
inline bool IND_Render::BlitRegionSurface (IND_Surface *pSu, 
										  int pX, 
										  int pY, 
										  int pWidth, 
										  int pHeight)
{
	// If the region is the same as the image area, we blit normally
	if (!pX && !pY && (pWidth == pSu->GetWidth()) && (pHeight == pSu->GetHeight()))
	{
		BlitSurface (pSu);
		return 1;
	}

	if (pSu->GetNumTextures() > 1)
		return 0;

	if (pX < 0 || pX + pWidth > pSu->GetWidth())	return 0;
	if (pY < 0 || pY + pHeight > pSu->GetHeight())  return 0;

	// ----- Transform 4 vertices of the quad into world space coordinates -----

	D3DXVECTOR4 mP1, mP2, mP3, mP4;
	Transform4Vertices ((float) pWidth, (float) 0,
						(float) pWidth, (float) pHeight,
						(float) 0, (float) 0,
						(float) 0, (float) pHeight,
						&mP1, &mP2, &mP3, &mP4);
		
	D3DXVECTOR3 mP1_f3 (mP1);
	D3DXVECTOR3 mP2_f3 (mP2);
	D3DXVECTOR3 mP3_f3 (mP3);
	D3DXVECTOR3 mP4_f3 (mP4);

	// Calculate the bounding rectangle that we are going to try to discard
	CalculateBoundingRectangle (&mP1_f3, &mP2_f3, &mP3_f3, &mP4_f3);

	// ---- Discard bounding rectangle using frustum culling if possible ----

	if (!CullFrustumBox (mP1_f3, mP2_f3))
	{	
		mNumDiscardedObjects++;
		return 1;	
	}

	mNumRenderedObjects++;

	// Prepare the quad that is going to be blitted
	// Calculates the position and mapping coords for that block
	FillVertex2d (&mVertices2d [0], pWidth,	0,			( (float) (pX + pWidth) / pSu->GetWidthBlock()	),	 ( 1.0f - ((float) (pY				+ pSu->GetSpareY() )			/ pSu->GetHeightBlock()  ) )	);
	FillVertex2d (&mVertices2d [1], pWidth,	pHeight,	( (float) (pX + pWidth) / pSu->GetWidthBlock()	),	 ( 1.0f - ((float) (pY + pHeight	+ pSu->GetSpareY() )			/ pSu->GetHeightBlock()  ) )	);
	FillVertex2d (&mVertices2d [2], 0,		0,			( (float) pX			/ pSu->GetWidthBlock()	),	 ( 1.0f - ((float) (pY				+ pSu->GetSpareY() )			/ pSu->GetHeightBlock()  ) )	);
	FillVertex2d (&mVertices2d [3], 0,		pHeight,	( (float) pX			/ pSu->GetWidthBlock()	),	 ( 1.0f - ((float) (pY + pHeight	+ pSu->GetSpareY() )			/ pSu->GetHeightBlock()  ) )	);	
	
	// Quad blitting
	mInfo.mDevice->SetFVF (D3DFVF_CUSTOMVERTEX2D);
	mInfo.mDevice->SetTexture (0, pSu->GetTextArray() [0].mTexture);
	mInfo.mDevice->DrawPrimitiveUP (D3DPT_TRIANGLESTRIP, 2, &mVertices2d, sizeof (CUSTOMVERTEX2D));
	
	return 1;
}


/*!
\b Parameters:

\arg \b pSu							Pointer to a ::IND_Surface object
\arg \b pX, \b pY					Upper left coordinate of the region
\arg \b pWidth, \b pHeight			Width and Height of the region
\arg \b pUDisplace, \b pVDisplace	Horizontal and vertical displacement of the image

\b Operation:

This function returns 1 (true) if it blits directly to the screen a ::IND_Surface object tiling it both
in X and Y coordinates.

This method is useful when we want to render a tiled texture or background.

Special remark: this function only works with ::IND_Surface objects that only have ONE texture 
assigned (you can check this using::IND_Surface::GetNumTextures() method). So, it will work only 
with images that are power of two and lower than the maximum texture size allowed by your card 
(you can check this parameter using ::IND_Render::GetMaxTextureSize()). The method will return 0 
otherwise.

In order to change the transformations and color attributes of the surface you have to use the IND_Render::SetTransform2d() and IND_Render::SetRainbow2d() methods before
calling to this function. Remember that you can use IND_Entity2d object for drawing surfaces to the screen without having to use this
advanced methods directly. This method is only useful for advanced users with really concrete purposes.

Using this method is equivalent to using all of these methods:
- IND_Entity2d::SetSurface()
- IND_Entity2d::SetRegion() 
- IND_Entity2d::ToggleWrap()
- IND_Entity2d::SetWrapDisplacement()
*/
inline bool IND_Render::BlitWrapSurface		(IND_Surface *pSu,  
											int pWidth, 
											int pHeight,
											float pUDisplace,
											float pVDisplace)
{
	if (pSu->GetNumTextures() > 1)
		return 0;

	// ----- Transform 4 vertices of the quad into world space coordinates -----

	D3DXVECTOR4 mP1, mP2, mP3, mP4;
	Transform4Vertices ((float) pWidth, (float) 0,
						(float) pWidth, (float) pHeight,
						(float) 0, (float) 0,
						(float) 0, (float) pHeight,
						&mP1, &mP2, &mP3, &mP4);
		
	D3DXVECTOR3 mP1_f3 (mP1);
	D3DXVECTOR3 mP2_f3 (mP2);
	D3DXVECTOR3 mP3_f3 (mP3);
	D3DXVECTOR3 mP4_f3 (mP4);

	// Calculate the bounding rectangle that we are going to try to discard
	CalculateBoundingRectangle (&mP1_f3, &mP2_f3, &mP3_f3, &mP4_f3);

	// ---- Discard bounding rectangle using frustum culling if possible ----

	if (!CullFrustumBox (mP1_f3, mP2_f3))
	{	
		mNumDiscardedObjects++;
		return 1;	
	}

	mNumRenderedObjects++;

	// Prepare the quad that is going to be blitted
	// Calculates the position and mapping coords for that block
	float mU = (float) pWidth  / pSu->GetWidthBlock  ();
	float mV = (float) pHeight / pSu->GetHeightBlock ();

	FillVertex2d (&mVertices2d [0], pWidth,	0,			 mU - pUDisplace,	pVDisplace);
	FillVertex2d (&mVertices2d [1], pWidth,	pHeight,	 mU - pUDisplace,	-mV + pVDisplace);	
	FillVertex2d (&mVertices2d [2], 0,		0,			 -pUDisplace,		pVDisplace);
	FillVertex2d (&mVertices2d [3], 0,		pHeight,	 -pUDisplace,		-mV + pVDisplace);

	// Quad blitting
	mInfo.mDevice->SetFVF (D3DFVF_CUSTOMVERTEX2D);
	mInfo.mDevice->SetTexture (0, pSu->GetTextArray() [0].mTexture);


	mInfo.mDevice->SetSamplerState (0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	mInfo.mDevice->SetSamplerState (0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	mInfo.mDevice->DrawPrimitiveUP (D3DPT_TRIANGLESTRIP, 2, &mVertices2d, sizeof (CUSTOMVERTEX2D));
	
	mInfo.mDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP); 
	mInfo.mDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP); 

	return 1;
}


/*!
\b Parameters:
				
\arg \b pAn							Pointer to a ::IND_Animation object
\arg \b pSequence					Number of the sequence to blit (the first sequence is 0)
\arg \b pX, \b pY					Upper left coordinate of the region
\arg \b pWidth, \b pHeight			Width and Height of the region
\arg \b pToggleWrap					Wraping on (1) / off (0)
\arg \b pUDisplace, \b pVDisplace	Horizontal and vertical displacement of the image

\b Operation:

This function blits directly to the screen a certain sequence of a ::IND_Animation object.

Each frame of the animation will be blited to the screen the number of milliseconds that are
defined in the animation script file. The sequecen starts in the frame 0 and finishes in the last frame 
specefied in the animation script. The animation will be displayed only one time, after that
it will stop in the last frame (bliting it permanently).

This functions returns -1 when the animation finishes, 0 if there is any error (for example trying to
blit an invalid IND_Animation pointer) and 1 if is in the middle of the animation and there are no errors.

In order to change the transformations
and color attributes of the animation you have to use the IND_Render::SetTransform2d() and IND_Render::SetRainbow2d() methods before
calling to this function. Remember that you can use IND_Entity2d object for drawing animations to the screen without having to use this
advanced methods directly. This method is only useful for advanced users with really concrete purposes.

Special remark: if you specify a region this function only works with ::IND_Surface objects that only have ONE texture 
assigned (you can check this using::IND_Surface::GetNumTextures() method). So, it will work only 
with images that are power of two and lower than the maximum texture size allowed by your card 
(you can check this parameter using ::IND_Render::GetMaxTextureSize()). The method will return 0 
otherwise.

Using this method is equivalent to using all of these methods:
- IND_Entity2d::SetAnimation()
- IND_Entity2d::SetRegion()
- IND_Entity2d::ToggleWrap()
- IND_Entity2d::SetWrapDisplacement()
*/
inline int IND_Render::BlitAnimation (IND_Animation *pAn, int pSequence, 
									  int pX, int pY,
									  int pWidth, int pHeight,
									  bool pToggleWrap,
									  float pUDisplace,
									  float pVDisplace)
{
	if (pSequence < 0 || pSequence > pAn->GetNumSequences () - 1) return 0;

	int mFinish = 1;
	
	if (!pAn->GetIsActive (pSequence)) 
	{
		pAn->GetSequenceTimer (pSequence)->Start();
		pAn->SetIsActive (pSequence, 1);
	}

	// Current world matrix
	D3DXMATRIX mMatWorld, mTrans;
	mInfo.mDevice->GetTransform (D3DTS_WORLD, &mMatWorld);
	
	// If the time of a frame have passed, go to the next frame
	if (pAn->GetSequenceTimer (pSequence)->GetTicks() > (unsigned long) pAn->GetActualFrameTime (pSequence))
	{
		// Put timer to zero
		pAn->GetSequenceTimer (pSequence)->Start();

		// Point to the next frame increasing the counter
		int i = pAn->GetActualFramePos (pSequence);
		pAn->SetActualFramePos (pSequence, i + 1);
		
		// If the counter is higher than the number of frames of the sequence, we put it to zero
		if (pAn->GetActualFramePos (pSequence) > pAn->GetNumFrames (pSequence) - 1)
		{
			pAn->SetActualFramePos (pSequence, pAn->GetNumFrames (pSequence) - 1); 
			pAn->SetIsActive (pSequence, 0);
			mFinish = -1;
		}
	}

	// ----- OffsetX y OffsetY -----

	D3DXMatrixTranslation	(&mTrans, 
							(float) pAn->GetActualOffsetX (pSequence),
							(float) pAn->GetActualOffsetY (pSequence), 
							0);
	D3DXMatrixMultiply (&mMatWorld, &mMatWorld, &mTrans);
	mInfo.mDevice->SetTransform (D3DTS_WORLD, &mMatWorld);

	// ----- Blitting -----

	// Blits all the IND_Surface (all the blocks)
	if (!pX && !pY && !pWidth && !pHeight)
	{
		BlitSurface (pAn->GetActualSurface (pSequence));
	}
	else
	// Blits a region of the IND_Surface
	if (!pToggleWrap)
	{
		if (pAn->GetActualSurface (pSequence)->GetNumTextures() > 1) 
			return 0;
		BlitRegionSurface (pAn->GetActualSurface (pSequence), pX, pY, pWidth, pHeight);
	}
	// Blits a wrapping IND_Surface
	else
	{
		if (pAn->GetActualSurface (pSequence)->GetNumTextures() > 1) 
			return 0;
		BlitWrapSurface (pAn->GetActualSurface (pSequence), pWidth, pHeight, pUDisplace, pVDisplace);
	}

	return mFinish;
}
/*@}*/


// --------------------------------------------------------------------------------
//							       Private methods
// --------------------------------------------------------------------------------


/*
==================
Fills a D3DFVF_CUSTOMVERTEX2D structure
==================
*/
inline void IND_Render::FillVertex2d	(CUSTOMVERTEX2D *pVertex2d, 
										int pX, 
										int pY,
										float pU,
										float pV)
{
	// Vertex
	pVertex2d->mX		= (float) pX;
	pVertex2d->mY		= (float) pY;
	pVertex2d->mZ		= (float) 0.0f;
	pVertex2d->mU		= (float) pU;
	pVertex2d->mV		= (float) pV;
}





