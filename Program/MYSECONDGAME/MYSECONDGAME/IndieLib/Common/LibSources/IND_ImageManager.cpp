/*****************************************************************************************
/* File: IND_ImageManager.cpp
/* Desc: Image Manager
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
#include "il/il.h"
#include "il/ilu.h"
#include "IND_ImageManager.h"


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
This function returns 1 (true) if the adminstrator is successfully initialized.
Must be called before using any method.
*/
bool IND_ImageManager::Init ()
{
	End ();
	InitVars ();

	Debug->Header ("Initializing ImageManager", 5);
	Debug->Header ("Preparing ImageManager", 1);
	mOk = true;

	Debug->Header ("ImageManager OK", 6);

	return mOk;
}


/*!
\b Operation:

This function frees the manager and all the objects that it contains.
*/
void IND_ImageManager::End()
{
	if (mOk)
	{
		Debug->Header ("Finalizing ImageManager", 5);
		Debug->Header ("Freeing images" , 5);
		FreeVars ();	
		Debug->Header ("Images freed", 6);
		Debug->Header ("ImageManager finalized", 6);
			
		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//									Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pNewImage             Pointer no a new image object.
\arg \b pName                 Image name

\b Operation:

This function returns 1 (true) if the image object passed as a parameter 
exists and is added successfully to the manager.
  
It supports the following graphics formats (thanks to http://openil.sourceforge.net):
bmp, png, tga, jpg and pcx.
*/
bool IND_ImageManager::Add (IND_Image *pNewImage, char *pName)
{
	Debug->Header ("Loading Image", 5);
	Debug->Header ("File name:", 3);
	Debug->DataChar (pName, 1);

	if (!mOk)
	{	
		WriteMessage ();
		return 0;
	}

	// ----- Obtaining and checking file extension -----

	char *mExt = GetExtensionFromName (pName);
	if (CheckExtImage (mExt))
		pNewImage->SetExtension (mExt);
	else
	{
		Debug->Header ("Unknown extension", 2);
		return 0;
	}
	
	Debug->Header ("Extension:", 3);
	Debug->DataChar (mExt, 1);

	// ----- Loading image using Devil -----

	// New image instance
	ILuint mImageName;
	ilGenImages (1, &mImageName);
	ilBindImage (mImageName);
	
	// Load the image
	if (!ilLoadImage (pName))
	{
		Debug->Header ("Image not found", 2);
		return 0;
	}

	// Attributes
	pNewImage->SetWidth		(ilGetInteger (IL_IMAGE_WIDTH));
	pNewImage->SetHeight	(ilGetInteger (IL_IMAGE_HEIGHT)); 
	pNewImage->SetFormatInt	(ilGetInteger (IL_IMAGE_FORMAT));
	pNewImage->SetBpp		(ilGetInteger (IL_IMAGE_BITS_PER_PIXEL));
	pNewImage->SetPointer	(ilGetData ());
	pNewImage->SetName		(pName); 

	// Devil image ID
	pNewImage->SetDevilId (mImageName);

	// ----- Puts the object into the manager -----

	AddToList (pNewImage);

	// ----- Debug -----

	Debug->Header ("Size:", 3);
	Debug->DataInt (pNewImage->GetWidth (), 0);
	Debug->DataChar ("x", 0);
	Debug->DataInt (pNewImage->GetHeight (), 1);

	Debug->Header ("Bpp:", 3);
	Debug->DataInt (pNewImage->GetBpp (), 1);

	Debug->Header ("Fomart:", 3);
	Debug->DataChar (pNewImage->GetFormatChar(), 1);
	
	Debug->Header("Image loaded", 6);	

	return 1;
}


/*!
\b Parameters:

\arg \b pNewImage             Pointer to a new image object
\arg \b pWidth                Width of the new image
\arg \b pHeight               Height of the new image
\arg \b pFormat               New image format. See ::IND_Format.

\b Operation:

This function returns 1 (true) if the image object passed as a parameter 
exists and is added successfully to the manager creating a new procedural image.

By default, the new image is not initialized to any color, you should used the Clear() 
method in order to assing a valid colour.  

*/
bool IND_ImageManager::Add (IND_Image *pNewImage, int pWidht, int pHeight, IND_Format pFormat)
{
	Debug->Header ("Creating Image", 5);

	if (!mOk)
	{	
		WriteMessage ();
		return 0;
	}

	// ----- Creating imagen using Devil -----

	// New image instance
	ILuint mImageName;
	ilGenImages (1, &mImageName);
	ilBindImage (mImageName);
	
	// New image sizing
	iluImageParameter (ILU_FILTER, ILU_NEAREST);
	iluScale (pWidht, pHeight, 3);
	ilConvertImage (pFormat, IL_UNSIGNED_BYTE);

	// Attributes
	pNewImage->SetWidth		(ilGetInteger (IL_IMAGE_WIDTH));
	pNewImage->SetHeight	(ilGetInteger (IL_IMAGE_HEIGHT)); 
	pNewImage->SetFormatInt	(ilGetInteger (IL_IMAGE_FORMAT));
	pNewImage->SetBpp		(ilGetInteger (IL_IMAGE_BITS_PER_PIXEL));
	pNewImage->SetPointer	(ilGetData ());

	// Name
	pNewImage->SetName		("Procedural"); 

	// Devil image ID
	pNewImage->SetDevilId (mImageName);

	// ----- Puts the image in the list  -----

	AddToList (pNewImage);

	// ----- Debug -----

	Debug->Header ("File name:", 3);
	Debug->DataChar (pNewImage->GetName (), 1);

	Debug->Header ("Size:", 3);
	Debug->DataInt (pNewImage->GetWidth (), 0);
	Debug->DataChar ("x", 0);
	Debug->DataInt (pNewImage->GetHeight (), 1);

	Debug->Header ("Bpp:", 3);
	Debug->DataInt (pNewImage->GetBpp (), 1);

	Debug->Header ("Format:", 3);
	Debug->DataChar (pNewImage->GetFormatChar(), 1);
	
	Debug->Header("Image created", 6);	

	return 1;
}


/*!
\b Parameters:

\arg \b pIm                 Pointer to an image object.

\b Operation:               

This function returns 1(true) if the image object passed as parameter exists and it is
deleted from the manager successfully.
*/
bool IND_ImageManager::Delete (IND_Image *pIm)
{	
	Debug->Header ("Freeing image", 5);

	if (!mOk || !pIm)
	{	
		WriteMessage ();
		return 0;
	}

	// Search object
	bool mIs = 0;
	list <IND_Image*>::iterator mImageListIter;
	for (mImageListIter  = mListImages->begin();
		 mImageListIter != mListImages->end();
		 mImageListIter++)
	{	
		if ((*mImageListIter) == pIm) 
		{
			mIs = 1;
			break;
		}
	}

	// Not found
	if (!mIs)
	{
		WriteMessage ();
		return 0;
	}
	
	// ----- Free object -----

	Debug->Header ("File name:", 3);
	Debug->DataChar (pIm->GetName (), 1);

	// Quit from list
	DelFromList (pIm);

	// Free image
	iluDeleteImage (pIm->GetDevilId ());

	// Free the string of the extension
	DisposeArray (pIm->mImage.mExt);

	Debug->Header ("Ok", 6);

	return 1;
}


/*!
\b Parameters:

\arg \b pNewImage             Pointer to a new image object
\arg \b pOldImage             Pointer to the image we want to clone

\b Operation:

This function returns 1 (true) if the image object passed as a parameter 
exists and is added successfully to the manager creating a new image cloning from a previous one.
*/
bool IND_ImageManager::Clone (IND_Image *pNewImage, IND_Image *pOldImage)
{
	Debug->Header ("Duplicating Image", 5);

	if (!mOk || !pNewImage || !pOldImage)
	{	
		WriteMessage ();
		return 0;
	}

	if (!pOldImage->GetDevilId ())
	{	
		WriteMessage ();
		return 0;
	}

	// ----- Image creation using Devil -----

	// New image instance
	ILuint mImageName;
	ilGenImages (1, &mImageName);
	ilBindImage (mImageName);

	// Copy the new image
	ilCopyImage (pOldImage->GetDevilId());
	
	// Devil image ID
	pNewImage->SetDevilId (mImageName);

	// Attributes
	pNewImage->SetWidth		(pOldImage->GetWidth());
	pNewImage->SetHeight	(pOldImage->GetHeight()); 
	pNewImage->SetFormatInt	(pOldImage->GetFormatInt());
	pNewImage->SetBpp		(pOldImage->GetBpp());
	pNewImage->SetPointer	(ilGetData ());
	pNewImage->SetName		("Image copied"); 

	// ----- Put the object into the manager -----

	AddToList (pNewImage);

	// ----- Debug -----
	
	Debug->Header ("File name:", 3);
	Debug->DataChar (pOldImage->GetName (), 1);
	Debug->Header("Image duplicated", 6);	

	return 1;
}


/*!
\b Parameters:

\arg \b pName          Image name

\b Operation:              

This function returns 1 (true) if the image object passed as a parameter exists and 
saves this image in a file with extension and type specified in the name.

It supports the following graphics formats (thanks to http://openil.sourceforge.net):
bmp, png, tga, jpg and pcx.
*/
bool IND_ImageManager::Save (IND_Image *pIm, char *pName)
{
	Debug->Header ("Saving image", 5);

	if (!mOk || !pIm || !pIm->IsImageLoaded())
	{	
		WriteMessage ();
		return 0;
	}

	// ----- Checking extension -----

	char *mExt = GetExtensionFromName (pName);
	if (CheckExtImage (mExt))
	{
		pIm->SetExtension (mExt);
	}
	else
	{
		Debug->Header ("Unknown extension", 2);
		return 0;
	}

	Debug->Header ("Image type:", 3);
	Debug->DataChar (mExt, 1);

	ilBindImage (pIm->GetDevilId ());
	
	Debug->Header ("File name:", 3);
	Debug->DataChar (pName, 1);

	Debug->Header ("Size:", 3);
	Debug->DataInt (pIm->GetWidth (), 0);
	Debug->DataChar ("x", 0);
	Debug->DataInt (pIm->GetHeight (), 1);

	ilSaveImage (pName);

	Debug->Header ("Ok", 6);
	return 1;
}


// --------------------------------------------------------------------------------
//									Private methods
// --------------------------------------------------------------------------------

/*
==================
Obtain file extension
Return "" if the file hasn't got an extension
==================
*/
char *IND_ImageManager::GetExtensionFromName (char *pName)
{
	int i;

	// The extension can be 127 chars large (1 char is for '/0')
	char *mExtImage = new char [128];

	// The search starts at the end of the name
	for (i = strlen (pName); i > -1; i--)
		if (pName [i] == '.') break;

	// Image without extension
	if (pName [i] != '.') return "";

	// Copy the extension
	strcpy (mExtImage, &pName [i+1]);

	return mExtImage;
}


/*
==================
Check if it is a known extension
==================
*/
bool IND_ImageManager::CheckExtImage (char *pExtImage)
{
	for (int i = 0; i < MAX_EXT_IMAGE; i++)
	{
		if (!strcmp (pExtImage, mSupportedExt [i]))
			return 1;
	}

	return 0;
}


/*
==================
Inserts object into the manager
==================
*/
void IND_ImageManager::AddToList (IND_Image *pNewImage)
{
	mListImages->push_back (pNewImage);
}


/*
==================
Deletes object from the manager
==================
*/
void IND_ImageManager::DelFromList	(IND_Image *pIm)
{
	mListImages->remove (pIm);
	pIm = 0;
}


/*
==================
Initialization error message
==================
*/
void IND_ImageManager::WriteMessage ()
{
	Debug->Header ("This operation can not be done", 3);
	Debug->DataChar ("", 1);
	Debug->Header ("Invalid Id or IND_ImageManager not correctly initialized", 2);
}


/*
==================
Init manager vars
==================
*/
void IND_ImageManager::InitVars ()
{
	mListImages = new list <IND_Image*>;

	// Supported extensions
	mSupportedExt [0]  = "bmp";	
	mSupportedExt [1]  = "png";
	mSupportedExt [2]  = "tga";
	mSupportedExt [3]  = "jpg";
	mSupportedExt [4]  = "jpeg";
	mSupportedExt [5]  = "pcx";	
}


/*
==================
Free manager memory
==================
*/
void IND_ImageManager::FreeVars ()
{	
	// Deletes all the manager entities
	list <IND_Image*>::iterator mImageListIter;
	for (mImageListIter  = mListImages->begin();
		 mImageListIter != mListImages->end();
		 mImageListIter++)
	{	
		if ((*mImageListIter)->GetDevilId ()) 
		{
			Debug->Header	("Freeing image:", 3);
			Debug->DataChar ((*mImageListIter)->GetName (), 1);

			// Free image
			iluDeleteImage ((*mImageListIter)->GetDevilId ());

			// Free the string of the extension
			DisposeArray ((*mImageListIter)->mImage.mExt);
		}
	}

	// Clear list
	mListImages->clear ();

	// Free list
	Dispose (mListImages);
} 







