/*****************************************************************************************
/* File: RenderTransform2d.cpp
/* Desc: Transformations applied before blitting a 2d object usind D3D
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
#include "Defines.h"
#include "IND_Render.h"


// --------------------------------------------------------------------------------
//							         Public methods
// --------------------------------------------------------------------------------


/*!
\b Parameters:

\arg \b pX              Upper left X coordinate of the Viewport 
\arg \b pY              Upper left Y coordinate of the Viewport 
\arg \b pWidth          Viewport width
\arg \b pHeight         Viewport height

\b Operation:  

This function returns 1 (true) if a 2d ViewPort is created in the specified area.

The ViewPort is the drawing region. By default, IndieLib is initialized with a Viewport
that has the same area as the window. Using this method you can define different viewports before
calling to IND_Entity2dManager::RenderEntities2d().

After using this method, all the bliting methods will render the graphical objects inside the Viewport.
Objects or blocks drawn outside the viewport will be discarded.

This method returns 0 (false) if the user tries to create a Viewport outside the window area.
*/
inline bool IND_Render::SetViewPort2d (int pX, 
									 int pY, 
									 int pWidth, 
									 int pHeight)
{
	// ----- If the region is outside the framebuffer return error -----

	if (pX +  pWidth > mInfo.mFBWidth)	return 0;
	if (pX < 0)							return 0;
	if (pY + pHeight > mInfo.mFBHeight)	return 0;
	if (pY < 0)							return 0;

	// ----- Viewport characteristics -----

	mInfo.mViewPortX	  = pX;
	mInfo.mViewPortY	  = pY;
	mInfo.mViewPortWidth  = pWidth;
	mInfo.mViewPortHeight = pHeight;

	// ----- World matrix -----

	D3DXMATRIX mMatWorld;
	mInfo.mDevice->SetTransform (D3DTS_WORLD, D3DXMatrixIdentity (&mMatWorld));

	// ----- View region  -----

	D3DVIEWPORT9 mViewData = { pX, pY, pWidth, pHeight, -2048.0f, 2048.0f };
	mInfo.mDevice->SetViewport (&mViewData);

	// ----- 2d Sets -----

	mInfo.mDevice->SetRenderState (D3DRS_NORMALIZENORMALS, false); 
	mInfo.mDevice->SetRenderState (D3DRS_ZENABLE, D3DZB_FALSE);
	mInfo.mDevice->SetRenderState (D3DRS_ZWRITEENABLE, false);
	mInfo.mDevice->SetRenderState (D3DRS_LIGHTING, false);

	// ----- Texture clamp on -----

	mInfo.mDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP); 
	mInfo.mDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP); 
	
	return 1;
}


/*!
\b Parameters:

\arg \b pCamera2d				::IND_Camera2d object that defines a camera.

\b Operation:  

This function sets a 2d camera. See the methods of ::IND_Camera2d for information on how you can manipulate the camera.
*/
inline void IND_Render::SetCamera2d (IND_Camera2d *pCamera2d)
{
	D3DXMATRIX mTrans, mMatView, mMatProjection;
	D3DXMatrixIdentity (&mMatView);
	D3DXMatrixIdentity (&mMatProjection);

	// ----- Move camera to origin -----

	int pStrafe = -pCamera2d->mX;
	int pFly = -pCamera2d->mY;
	pCamera2d->Strafe (pStrafe);
	pCamera2d->Fly (pFly);

	// ----- View matrix -----

	pCamera2d->mLook	= D3DXVECTOR3 (0.0f, 0.0f, 1.0f);
	pCamera2d->mUp		= D3DXVECTOR3 (0.0f, 1.0f, 0.0f);
	pCamera2d->mRight	= D3DXVECTOR3 (-1.0f, 0.0f, 0.0f);

	// ----- Rotation -----
		
	// Roll is rotation around the z axis (m_look)
	// Create a matrix that can carry out this rotation
	D3DXMATRIX rollMatrix;
	D3DXMatrixRotationAxis (&rollMatrix, &pCamera2d->mLook, D3DXToRadian(pCamera2d->mAngle));
	// To apply roll we rotate up and right about the look vector (using our roll matrix)
	D3DXVec3TransformCoord (&pCamera2d->mRight, &pCamera2d->mRight, &rollMatrix); 
	D3DXVec3TransformCoord (&pCamera2d->mUp, &pCamera2d->mUp, &rollMatrix);

	// Build the view matrix from the transformed camera axis
	mMatView._11 = pCamera2d->mRight.x; mMatView._12 = pCamera2d->mUp.x; mMatView._13 = pCamera2d->mLook.x;
	mMatView._21 = pCamera2d->mRight.y; mMatView._22 = pCamera2d->mUp.y; mMatView._23 = pCamera2d->mLook.y;
	mMatView._31 = pCamera2d->mRight.z; mMatView._32 = pCamera2d->mUp.z; mMatView._33 = pCamera2d->mLook.z;
	
	mMatView._41 = -D3DXVec3Dot (&pCamera2d->mPos, &pCamera2d->mRight); 
	mMatView._42 = -D3DXVec3Dot (&pCamera2d->mPos, &pCamera2d->mUp);
	mMatView._43 = -D3DXVec3Dot (&pCamera2d->mPos, &pCamera2d->mLook);

	// ---- 2d set ----

	D3DXMatrixRotationZ		(&mTrans, (float) PI);
	D3DXMatrixMultiply		(&mMatView, &mTrans, &mMatView);
	D3DXMatrixTranslation	(&mTrans, -0.5f, -0.5f, 0);
	D3DXMatrixMultiply		(&mMatView, &mTrans, &mMatView);

	// ---- Zoom ----

	if (pCamera2d->mZoom != 1.0f)
	{
		D3DXMatrixScaling (&mTrans, pCamera2d->mZoom, pCamera2d->mZoom, pCamera2d->mZoom);
		D3DXMatrixMultiply (&mMatView, &mMatView, &mTrans);
	}
	// ----- Set transformation -----

	mInfo.mDevice->SetTransform (D3DTS_VIEW, &mMatView);

	// ----- Projection matrix -----

	D3DXMatrixOrthoRH (&mMatProjection, (float) mInfo.mViewPortWidth, (float) mInfo.mViewPortHeight, -2048.0f, 2048.0f);
	mInfo.mDevice->SetTransform (D3DTS_PROJECTION, &mMatProjection);

	// ----- Move camera to the correct position -----

	pCamera2d->Strafe (-pStrafe);
	pCamera2d->Fly (-pFly);
}


/*!
\defgroup Graphical_Objects Bliting Surfaces, Animations, Fonts and setting transformations directly
\ingroup Advances
*/
/*@{*/

/*!
\b Parameters:

\arg \b pX							Translation in the X coordinate. The (0, 0) position is the 
									upper-left corner of the Viewport
\arg \b pY							Translation in the Y coordinate. The (0, 0) position is the 
									upper-left corner of the Viewport 
\arg \b pAngleX						Rotation in the angle x in degrees
\arg \b pAngleY						Rotation in the angle y in degrees
\arg \b pAngleZ						Rotation in the angle z in degrees
\arg \b pScaleX						Scaling in the x coordinate. 1 for maintaining the original size
\arg \b pScaleY						Scaling in the y coordinate. 1 for maintaining the original size
\arg \b pAxisCalX, \b pAxisCalY		Parameters that indicates the displacement that the graphical
									object undergoes due to the HotSpot. If the HotSpot is not specified, 
									the value should be 0 for both of them. All the transformation 
									will be aplied from the upper-left corner of the object.           
\arg \b pMirrorX					Horizontal mirroring. (true / false) = (activated / deactivated).
\arg \b pMirrorY					Vertical mirroring. (true / false) = (activated / deactivated).
\arg \b pWidth						Width of the graphical object that we are going to blit just after
									applying the transformation. You shoud use the GetWidth() method
									of the object.
\arg \b pHeight						Height of the graphical object that we are going to blit just after
									applying the transformation. You shoud use the GetHeight() method
									on the object.
\arg \b pMatrix						Pointer to a ::IND_Matrix matrix. In this parameter will be
									returned by reference the world matrix transformation that
									will be aplied to the graphical object. This matrix can be useful
									for advanced programmers that need the algebraic description
									of the transformation. It is possible to use the value 0
									if it not necessary to have this matrix information.
\b Operation:  
 
This function sets the 2d transformation (translation, rotation, scaling, mirroring and hotspot)
of the following 2d graphical objects that will be rendered
by the engine. You should use this method before calling to any of the Bliting methods.

Remember that you can use IND_Entity2d object for applying 2d transformations to the graphical
objects without having to use this advanced method directly. This method is only useful for advanced 
users with really concrete purposes.

Using this method is equivalent to using a combination of these methods:
- IND_Entity2d::SetAnimation()
- IND_Entity2d::SetSurface()
- IND_Entity2d::SetPrimitive2d()
- IND_Entity2d::SetFont()
- IND_Entity2d::SetPosition()
- IND_Entity2d::SetAngleXYZ()
- IND_Entity2d::SetScale()
- IND_Entity2d::SetBackCull()
- IND_Entity2d::SetMirrorX()
- IND_Entity2d::SetMirrorY()
- IND_Entity2d::SetFilter()
- IND_Entity2d::SetHotSpot()
- IND_Entity2d::SetRegion()
- IND_Entity2d::ToggleWrap()
*/
inline void IND_Render::SetTransform2d	(int pX, 
										int pY, 
										float pAngleX,
										float pAngleY,
										float pAngleZ,
										float pScaleX,
										float pScaleY,
										int pAxisCalX,
										int pAxisCalY,
										bool pMirrorX,
										bool pMirrorY,
										int pWidth,
										int pHeight,
										IND_Matrix *pMatrix)
{
	// ----- World matrix initialization -----

	D3DXMATRIX mMatWorld, mMatZ, mMatX, mMatY, mMatTraslation, mMatScale;

	mInfo.mDevice->SetTransform (D3DTS_WORLD, D3DXMatrixIdentity (&mMatWorld));

	// ----- Transformation matrix creation -----

	// Mirroring (180º rotations)
	if (pMirrorX || pMirrorY)
	{ 
		if (pMirrorX && !pMirrorY)
		{
			D3DXMatrixTranslation (&mMatWorld, (float) -pWidth, 0, 0);
			D3DXMatrixRotationY (&mMatY, D3DXToRadian (180));
			D3DXMatrixMultiply (&mMatWorld, &mMatWorld, &mMatY);
		}

		if (!pMirrorX && pMirrorY)
		{
			D3DXMatrixTranslation (&mMatWorld, 0, (float) -pHeight, 0);	
			D3DXMatrixRotationX (&mMatX, D3DXToRadian (180));
			D3DXMatrixMultiply (&mMatWorld, &mMatWorld, &mMatX);
		}

		if (pMirrorX && pMirrorY)
		{
		
			D3DXMatrixTranslation (&mMatWorld, (float) -pWidth, 0, 0);		
			D3DXMatrixTranslation (&mMatTraslation, 0, (float) -pHeight, 0);
			D3DXMatrixMultiply	(&mMatWorld, &mMatWorld, &mMatTraslation);
			
			D3DXMatrixRotationY (&mMatY, D3DXToRadian (180));
			D3DXMatrixMultiply (&mMatWorld, &mMatWorld, &mMatY);
			D3DXMatrixRotationX (&mMatX, D3DXToRadian (180));
			D3DXMatrixMultiply (&mMatWorld, &mMatWorld, &mMatX);
		}
	}

	// Hotspot
	if (pAxisCalX != 0 || pAxisCalY != 0)
	{
		D3DXMatrixTranslation (&mMatTraslation, (float) pAxisCalX, (float) pAxisCalY, 0);
		D3DXMatrixMultiply	(&mMatWorld, &mMatWorld, &mMatTraslation);
	}

	// Scaling
	if (pScaleX != 1.0f || pScaleY != 1.0f)
	{
		D3DXMatrixScaling (&mMatScale, pScaleX, pScaleY, 1.0f);
		D3DXMatrixMultiply (&mMatWorld, &mMatWorld, &mMatScale);
	}

	// Rotations
	if (pAngleX != 0.0f)
	{
		D3DXMatrixRotationX (&mMatX, D3DXToRadian (pAngleX));
		D3DXMatrixMultiply (&mMatWorld, &mMatWorld, &mMatX);
	}
	if (pAngleY != 0.0f)
	{
		D3DXMatrixRotationY (&mMatY, D3DXToRadian (pAngleY));
		D3DXMatrixMultiply (&mMatWorld, &mMatWorld, &mMatY);
	}
	if (pAngleZ != 0.0f)
	{
		D3DXMatrixRotationZ (&mMatZ, D3DXToRadian (pAngleZ));
		D3DXMatrixMultiply (&mMatWorld, &mMatWorld, &mMatZ);
	}

	// Translations
	if (pX != 0 || pY != 0)
	{
		D3DXMatrixTranslation (&mMatTraslation, (float) pX, (float) pY, 0);
		D3DXMatrixMultiply (&mMatWorld, &mMatWorld, &mMatTraslation);
	}

	// ----- Return World Matrix (in IndieLib format) -----

	if (pMatrix)
	{
		pMatrix->_11 = mMatWorld._11;
		pMatrix->_12 = mMatWorld._12;
		pMatrix->_13 = mMatWorld._13;
		pMatrix->_14 = mMatWorld._14;
		
		pMatrix->_21 = mMatWorld._21;
		pMatrix->_22 = mMatWorld._22;
		pMatrix->_23 = mMatWorld._23;
		pMatrix->_24 = mMatWorld._24;

		pMatrix->_31 = mMatWorld._31;
		pMatrix->_32 = mMatWorld._32;
		pMatrix->_33 = mMatWorld._33;
		pMatrix->_34 = mMatWorld._34;

		pMatrix->_41 = mMatWorld._41;
		pMatrix->_42 = mMatWorld._42;
		pMatrix->_43 = mMatWorld._43;
		pMatrix->_44 = mMatWorld._44;
	}
		
	// ----- Applies the transformation -----
	
	mInfo.mDevice->SetTransform (D3DTS_WORLD, &mMatWorld);
}


/*!
\b Parameters:

\arg \b pType										::IND_Type type of the following graphical object.
\arg \b pCull										Backface culling. (true / false) = (on / off). This feature can
													be deactivated when we are going to rotate the graphical object 
													in the x or y coordinates. This way it is possible to draw for 
													example the leaf of a tree that falls down rotating and shows
													both faces. In cases that you don't want to draw back faces 
													deactivate this for faster render times.
\arg \b pMirrorX									Horizontal mirroring. (true / false) = (activated / deactivated).
\arg \b pMirrorY									Vertical mirroring. (true / false) = (activated / deactivated).
\arg \b pFilter										Type of filter ::IND_Filter. There are two types of filters that 
													can be applied to the graphical object when it suffers a rotation 
													or scaling: 
													- ::IND_FILTER_POINT (Nearest point filter). Less quality, but 
													bigger performance. It is possible to use this one without 
													quality loss in graphical objects that don't rotate or are 
													affected by scaling.
													- ::IND_FILTER_LINEAR (Bilinear filter). More quality, but less 
													performance. Used in graphical objects that be rotated or 
													affected by scaling.
\arg \b pR, pG, pB									Color bytes FadeR, FadeG, FadeB 
													Used for fading the image to a certain color. 
													For example, if we use RGB = (255, 0, 0), 
													the image will progressively become red. 
\arg \b pA											The A byte indicates the level of transparency. 
													If a value of 0 is used, the following graphical 
													object will be completely transparent, as opposed 
													to the value 255 that will cause the object 
													to be drawn completely opaque. It is possible to use all the 
													intermediate values for different levels of 
													transparency.											
\arg \b pFadeR, \b pFadeG, \b pFadeB, \b pFadeA		Bytes FadeR, FadeG, FadeB
													Used for fading the image to a certain color. 
													For example, if we use RGB = (255, 0, 0), 
													the image will progressively become red. 
\arg \b pSo											Indicates the blending source, 
													see (::IND_BlendingType).
\arg \b pDs											Indicates the blending destiny, 
													see (::IND_BlendingType).

Operation:  
 
This functions sets the color and blending attributes of a graphical object. It should
be used before any calling to the Blit methods.

Important: It is not possible to use tinting and fading at the same time, the fading will overide the tinting.

Remember that you can use IND_Entity2d objects for applying color 
transformations to the graphical objects without having to use this advanced method directly. 
This method is only useful for advanced users with really concrete purposes.

Using this method is equivalent to using a combination of these methods:
- IND_Entity2d::SetTransparency()
- IND_Entity2d::SetFade()
- IND_Entity2d::SetTint()
- IND_Entity2d::SetBlendSource()
- IND_Entity2d::SetBlendDest()
*/
inline void IND_Render::SetRainbow2d	(IND_Type pType,
										bool pCull,
										bool pMirrorX,
										bool pMirrorY,
										IND_Filter pFilter,
										byte pR, 
										byte pG, 
										byte pB, 
										byte pA,
										byte pFadeR,
										byte pFadeG,
										byte pFadeB,
										byte pFadeA,
										IND_BlendingType pSo,
										IND_BlendingType pDs) 
{
	// ----- Filters -----

	mInfo.mDevice->SetSamplerState (0, D3DSAMP_MIPFILTER, GetD3DFilter (pFilter)); 
	mInfo.mDevice->SetSamplerState (0, D3DSAMP_MINFILTER, GetD3DFilter (pFilter)); 
	mInfo.mDevice->SetSamplerState (0, D3DSAMP_MAGFILTER, GetD3DFilter (pFilter)); 

	// ----- Back face culling -----

	if (pCull)
		mInfo.mDevice->SetRenderState (D3DRS_CULLMODE, D3DCULL_CCW);
	else
		mInfo.mDevice->SetRenderState (D3DRS_CULLMODE, D3DCULL_NONE);

	// Mirroring (180º rotations)
	if (pMirrorX || pMirrorY)
	{ 
		if (pMirrorX && !pMirrorY)
		{
			// Back face culling
			if (pCull)
				mInfo.mDevice->SetRenderState (D3DRS_CULLMODE, D3DCULL_CW);
			else
				mInfo.mDevice->SetRenderState (D3DRS_CULLMODE, D3DCULL_NONE);
		}

		if (!pMirrorX && pMirrorY)
		{
			// Back face culling
			if (pCull)
				mInfo.mDevice->SetRenderState (D3DRS_CULLMODE, D3DCULL_CW);
			else
				mInfo.mDevice->SetRenderState (D3DRS_CULLMODE, D3DCULL_NONE);
		}
	}

	// ----- Blending -----

	switch (pType)
	{
		case IND_OPAQUE:
		{
			// Alphablending and alpha test = OFF
			mInfo.mDevice->SetRenderState (D3DRS_ALPHABLENDENABLE,	FALSE);
			mInfo.mDevice->SetRenderState (D3DRS_ALPHATESTENABLE,	FALSE);

			// Initializes the tinting and alpha values of previous iterations
			mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
			mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG1,  D3DTA_TEXTURE);
			mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG2,  D3DTA_CURRENT);
			mInfo.mDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1); 
			mInfo.mDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE); 
			mInfo.mDevice->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

			// Tinting
			if (pR != 255 || pG != 255 || pB != 255)
			{
				// Tinting color
				mInfo.mDevice->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (pR, pG, pB, pA));
				
				// Tinting
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_TFACTOR);																	
			}	

			// Alpha
			if (pA != 255)
			{
				// Alpha color
				mInfo.mDevice->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (pR, pG, pB, pA));

				// Alpha-blending = ON
				mInfo.mDevice->SetRenderState (D3DRS_ALPHABLENDENABLE, TRUE);

				// Set source alpha and destination alpha
				mInfo.mDevice->SetRenderState (D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA); 
				mInfo.mDevice->SetRenderState (D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

				// Alpha
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1,  D3DTA_TFACTOR);		
			}

			// Fade to color
			if (pFadeA != 255)
			{
				// Color fade
				mInfo.mDevice->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (pFadeR, pFadeG, pFadeB, pFadeA));

				// Fade
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA);
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_TFACTOR);		
			}	

			if (pSo && pDs)
			{
				// Alpha-blending = ON
				mInfo.mDevice->SetRenderState (D3DRS_ALPHABLENDENABLE, TRUE);
				
				mInfo.mDevice->SetRenderState (D3DRS_SRCBLEND,	GetD3DBlendingType (pSo)); 
				mInfo.mDevice->SetRenderState (D3DRS_DESTBLEND, GetD3DBlendingType (pDs));
			}

			break;
		}

		case IND_ALPHA:
		{
			// Alpha-testing = OFF
			mInfo.mDevice->SetRenderState (D3DRS_ALPHATESTENABLE, FALSE);

			// Initializes the tinting and alpha values of previous iterations
			mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
			mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG1,  D3DTA_TEXTURE);
			mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG2,  D3DTA_CURRENT);
			mInfo.mDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1); 
			mInfo.mDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE); 
			mInfo.mDevice->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

			// Tinting
			if (pR != 255 || pG != 255 || pB != 255)
			{
				// Tinting color
				mInfo.mDevice->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (pR, pG, pB, pA));
				
				// Tinting
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_TFACTOR);																																										
			}

			// Alpha
			if (pA != 255)
			{
				// Color alpha
				mInfo.mDevice->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (pR, pG, pB, pA));

				mInfo.mDevice->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
			}

			// Fade to color
			if (pFadeA != 255)
			{			
				// Color fade
				mInfo.mDevice->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (pFadeR, pFadeG, pFadeB, pFadeA));

				// Fade
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA);
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				mInfo.mDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_TFACTOR);		
			}	

			// Alpha-blending = ON
			mInfo.mDevice->SetRenderState (D3DRS_ALPHABLENDENABLE,	TRUE);

			// Set source alpha and destination alpha
			if (!pSo || !pDs) 
			{
				mInfo.mDevice->SetRenderState (D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA); 
				mInfo.mDevice->SetRenderState (D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);			
			}
			else
			{
				mInfo.mDevice->SetRenderState (D3DRS_SRCBLEND,	GetD3DBlendingType (pSo)); 
				mInfo.mDevice->SetRenderState (D3DRS_DESTBLEND, GetD3DBlendingType (pDs));
			}
		}

		default:
		{
		}
	}
}
