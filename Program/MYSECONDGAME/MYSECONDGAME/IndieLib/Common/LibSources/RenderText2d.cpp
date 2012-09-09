/*****************************************************************************************
/* File: RenderText2d.cpp
/* Desc: Bitmap fonts text blitting
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
#include "IND_Render.h"


// --------------------------------------------------------------------------------
//							         Public methods
// --------------------------------------------------------------------------------


/*!
\defgroup Graphical_Objects Bliting Surfaces, Animations, Fonts and setting transformations directly
\ingroup Advances
*/
/*@{*/

/*!
\b Parameters:

\arg \b pFo											Pointer to a ::IND_Font object
\arg \b pText										Text to write to the screen
\arg \b pX, \b pY									Position
\arg \b pOffset										Char spacing
\arg \b pLineSpacing								Line spacing
\arg \b pR, \b pG, \b pB							R, G, B components of the tinting color
\arg \b pA											Transparency level. (255 = complety opaque)
\arg \b pFadeR, \b pFadeG, \b pFadeB, \b pFadeA		Fade to a color.											
\arg \b pFilter										::IND_Filter type
\arg \b pSo											Source blending, see (::IND_BlendingType).
\arg \b pDs											Destiny blending, see (::IND_BlendingType).
\arg \b pAlign										Text alignment, see::IND_Align.

\b Operation:

This function blits text directly to the screen using the ::IND_Font object. 

Important: you cannot change the transformation or color attributes of a font using IND_Render::SetTransform2d() or IND_Render::SetRainbow().

Remember that you can use IND_Entity2d object for drawing fonts to the screen without having to use this
advanced method directly. This method is only useful for advanced users with really concrete purposes.

Using this method is equivalent to using a combination of these methods:
- IND_Entity2d::SetFont()
- IND_Entity2d::SetPosition()
- IND_Entity2d::SetText()
- IND_Entity2d::SetCharSpacing()
- IND_Entity2d::SetLineSpacing()
- IND_Entity2d::SetAlign()
*/
inline void IND_Render::BlitText  (IND_Font *pFo, 
								  char *pText, 
								  int pX, 
								  int pY, 
								  int pOffset,
								  int pLineSpacing,
								  float pScaleX,  //MIGUEL: New param
								  float pScaleY,  //MIGUEL: New param
								  byte pR,
								  byte pG,
								  byte pB,
								  byte pA,
								  byte pFadeR,
								  byte pFadeG,
								  byte pFadeB,
								  byte pFadeA,
								  IND_Filter pLinearFilter,
								  IND_BlendingType pSo,
								  IND_BlendingType pDs,
								  IND_Align pAlign)
{
	// ----- Transform -----

	//SetTransform2d (pX, pY, 0, 0, 0, 1, 1, 0, 0, 0, 0, pFo->GetSurface()->GetWidthBlock(), pFo->GetSurface()->GetHeightBlock(), 0);
	//MIGUEL MODIFICATION: Scale enabled for fonts!
	SetTransform2d (pX, pY, 0, 0, 0, pScaleX, pScaleY, 0, 0, 0, 0, pFo->GetSurface()->GetWidthBlock(), pFo->GetSurface()->GetHeightBlock(), 0);
	SetRainbow2d (pFo->mFont.mSurface->GetTypeInt(), 1, 0, 0, pLinearFilter, pR, pG, pB, pA, pFadeR, pFadeG, pFadeB, pFadeA, pSo, pDs);

	// ----- Drawing text	-----

	byte mChar1;
	byte mChar2;
	int mCont1 = 0;
	int mCont2 = 0;
	int mTranslationX = 0;
	int mTranslationY = 0;
	bool mErrorChar;	// Char that doesn't exist
	int mSentencePos;
	int mLongActualSentence;

	D3DXMATRIX mMatTraslation, mMatWorld, mMatWorldOriginal;
	mInfo.mDevice->GetTransform (D3DTS_WORLD, &mMatWorld);
	mInfo.mDevice->GetTransform (D3DTS_WORLD, &mMatWorldOriginal);

	mCont1 = 0;
	mChar1 = pText [mCont1++];

	while (mChar1 != 0)
	{
		// If it's a new line or it's the first line
		if (mChar1 == '\n' || mCont1 == 1)
		{
			// First line
			if (mCont1 == 1) 
				mSentencePos = 0; 
			else 
				mSentencePos = mCont1;
			
			// Set the alignment
			switch (pAlign) 
			{
				case IND_CENTER:
				{	
					//mLongActualSentence = GetLongInPixels (pFo, pText, mSentencePos, pOffset);
					mLongActualSentence = static_cast<int>(GetLongInPixels (pFo, pText, mSentencePos, pOffset) * pScaleX);  //MIGUEL MODIFICATION: Scale enabled for fonts!
					mTranslationX = (int) ( mLongActualSentence / 2);
					break;
				}

				case IND_RIGHT:
				{
					//mLongActualSentence = GetLongInPixels (pFo, pText, mSentencePos, pOffset);
					mLongActualSentence = static_cast<int>(GetLongInPixels (pFo, pText, mSentencePos, pOffset)* pScaleX); //MIGUEL MODIFICATION: Scale enabled for fonts!
					mTranslationX = (int) ( mLongActualSentence);

					break;
				}

				case IND_LEFT:
				{
					break;
				}
			}

			D3DXMatrixTranslation (&mMatTraslation, (float) -mTranslationX, (float) mTranslationY, 0);
			//mTranslationY += (pLineSpacing);
			mTranslationY += static_cast<int>((pLineSpacing * pScaleY));  //MIGUEL MODIFICATION: Scale enabled for fonts!
			D3DXMatrixMultiply (&mMatWorld, &mMatWorldOriginal, &mMatTraslation);
			mInfo.mDevice->SetTransform (D3DTS_WORLD, &mMatWorld);
		}
		
		// It's a normal character
		if (mChar1 != '\n')
		{

			mErrorChar = 0;
			mCont2 = 0;
			mChar2 = pFo->GetLetters() [mCont2++].mLetter;

			// Seek its location in the bitmap
			while (mChar2 != mChar1 && mCont2 < pFo->GetNumChars()) mChar2 = pFo->GetLetters() [mCont2++].mLetter;	
			if (mCont2  ==  pFo->GetNumChars())
				mErrorChar = 1;
			
			mCont2--;
			if (!mErrorChar)
			{
				BlitRegionSurface	(pFo->GetSurface(), 
									pFo->GetLetters() [mCont2].mOffsetX+1, 
									pFo->GetLetters() [mCont2].mOffsetY+1, 
									pFo->GetLetters() [mCont2].mWidthChar-1, 
									pFo->GetLetters() [mCont2].mHeightChar-1);			
			}

		
			// Displacement of the character
			//SetTranslation ((pFo->GetLetters() [mCont2].mWidthChar) + pOffset, 0, &mMatWorld, &mMatTraslation);
			SetTranslation (static_cast<int>(((pFo->GetLetters() [mCont2].mWidthChar) + pOffset) * pScaleX), 0, &mMatWorld, &mMatTraslation);  //MIGUEL MODIFICATION: Scale enabled for fonts!
		}

		// Advance one character
		mChar1 = pText [mCont1++];
	}
}
/*@}*/	


// --------------------------------------------------------------------------------
//							         Private methods
// --------------------------------------------------------------------------------

/*
==================
Returns a long in pixels of the sentence that starts in pPos
A sentence is an array of chars that ends in \n or \0
==================
*/ 
inline int IND_Render::GetLongInPixels (IND_Font *pFo, char *pText, int pPos, int pOffset)
{
	byte mChar1;
	byte mChar2;
	int mCont1 = pPos;
	int mCont2 = 0;
	bool mErrorChar;	// Error when nonexistant char

	int mWidthSentence = 0;
	mChar1 = pText [mCont1++];	
	while (mChar1 != '\0' && mChar1 != '\n') 
	{ 
		mErrorChar = 0;
		mCont2 = 0;
		mChar2 = pFo->GetLetters () [mCont2++].mLetter;

		// Search the position
		while (mChar2 != mChar1 && mCont2 < pFo->GetNumChars()) mChar2 = pFo->GetLetters() [mCont2++].mLetter;	
		if (mCont2  ==  pFo->GetNumChars())
			mErrorChar = 1;

		if (!mErrorChar)
			mWidthSentence += pFo->GetLetters() [mCont2 - 1].mWidthChar + pOffset;
				
		mChar1 = pText [mCont1++]; 
	}

	return mWidthSentence;
}


/*
==================
Set translation (used for translating the chars)
==================
*/ 
inline void IND_Render::SetTranslation (int pX, int pY, D3DXMATRIX *pMatWorld, D3DXMATRIX *pMatTraslation)
{
		D3DXMatrixTranslation (pMatTraslation, (float) pX, (float) pY, 0);
		D3DXMatrixMultiply (pMatWorld, pMatWorld, pMatTraslation);
		mInfo.mDevice->SetTransform (D3DTS_WORLD, pMatWorld);
}

