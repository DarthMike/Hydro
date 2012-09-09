/*****************************************************************************************
/* File: IND_Image.h
/* Desc: Image object
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

#ifndef _IND_IMAGE_
#define _IND_IMAGE_


// --------------------------------------------------------------------------------
//									 IND_Image
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Image IND_Image
\ingroup Objects
IND_Image class managed by IND_ImageManager for loading, saving and applying filters to images, click in IND_Image to see all the methods of this class.
*/
/*@{*/

/*!
\b IND_Image is an image object from the class ::IND_ImageManager. Read the explanation in ::IND_ImageManager for more details.
*/
class DLL_EXP IND_Image
{
public:

	// ----- Public methods ------

	bool Clear			(byte pR, byte pG, byte pB, byte pA);
	bool GetPixel		(int pX, int pY, byte *pR, byte *pG, byte *pB, byte *pA);
	bool PutPixel		(int pX, int pY, byte pR, byte pG, byte pB, byte pA);
	bool FlipH			();
	bool FlipV			();
	bool Convert		(IND_Format pFormat);
	bool SetAlpha		(byte pR, byte pG, byte pB);
	bool PasteImage		(IND_Image *pIm, int pX, int pY);
	bool GaussianBlur	(int pIter);
	bool Contrast		(float pCont);
	bool Equalize		();
	bool SetGamma		(float pGamma);
	bool Negative		();
	bool Noisify		(float pNoise);
	bool Pixelize		(int pPixSize);
	bool Sharpen		(float pFactor, int pIter);
	bool Scale			(int pSizeX, int pSizeY); 
	bool EdgeDetect1	();
	bool EdgeDetect2	();
	bool Emboss			();
	bool Rotate			(float pAngle);
	bool InvertAlpha	();
	bool Saturation		(float pSaturation);

	// ----- Public gets ------

	//! This function returns the width in pixels of the image.
	int IsImageLoaded	()	{ if (mImage.mDevilId == -1) return 0; else return 1;	}
	//! This function returns the width in pixels of the image.
	int GetWidth		()	{ return mImage.mWidth;						}
	//! This function returns the height in pixels of the image
	int GetHeight		()	{ return mImage.mHeight;					}	
	//! This function returns the bytes per pixel of the image.
	int GetBpp			()	{ return mImage.mBpp;						}
	//! This function returns the image format in an integer. See ::IND_Format.
	int	GetFormatInt	()	{ return mImage.mFormat;					}
	// !This function returns the format from the image in a text string. See ::IND_Format.
	char *GetFormatChar ()	{ return FormatToChar (GetFormatInt	());				}
	//! This function returns the extension that had the image when it was stored in a file. For example if the image was "loover.jpg" the function will return "jpg".
	char *GetExtension	()	{ return mImage.mExt;						}
	//! This function returns the pointer to the memory bytes which forms the image. It is a function very useful for accessing to the image directy and for modifying it or reading it.     
	byte *GetPointer	()	{ return mImage.mPointer;					}
	//! This function returns the name of the image in a string of chars.
	char *GetName		()	{ return mImage.mName;						}
	
private:

	// ----- Structures ------

	//TYPE
	struct structImage
	{	
		int			mWidth;				// Image width
		int			mHeight;			// Image height
		int			mBpp;				// Bytes per pixel
		IND_Format	mFormat;			// Image format
		char		*mExt;				// Image file extension
		byte		*mPointer;			// Image data pointer
		char		*mName;				// Image name
		int	mDevilId;					// Devil ID

		structImage ()
		{
			mWidth = mHeight = mBpp = mFormat = 0;
			mExt = 0;
			mPointer = 0;
			mName = 0;
			mDevilId = -1;
		}
	};
	typedef struct structImage IMAGE;

	IMAGE mImage;

	// ----- Private sets ------

	void SetWidth		(int pWidth)	{ mImage.mWidth = pWidth;		}
	void SetHeight		(int pHeight)	{ mImage.mHeight = pHeight;		}	
	void SetBpp			(int pBpp)		{ mImage.mBpp = pBpp;			}
	void SetFormatInt	(int pFormat)	{ mImage.mFormat = pFormat;		}
	void SetExtension	(char *pExt)	{ mImage.mExt = pExt;			}
	void SetPointer		(byte *pPtr)	{ mImage.mPointer = pPtr;		}
	void SetName		(char *pName)	{ mImage.mName = pName;			}
	void SetDevilId		(UINT pDevId) 	{ mImage.mDevilId = pDevId;					}
	UINT GetDevilId	()					{ return mImage.mDevilId;					}

	// ----- Private methods -----

	void SetAlphaChannel				(byte pR, byte pG, byte pB);
	char *FormatToChar					(IND_Format pFormat);
	
	// ----- Friends -----
	
	friend class IND_ImageManager;
};
/*@}*/

#endif // _IND_IMAGE_
