/*****************************************************************************************
/* File: IND_3dMesh.h
/* Desc: 3d Mesh object
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

#ifndef _IND_3DMESH_
#define _IND_3DMESH_

// ----- Includes -----

#include <d3d9.h> 
#include <d3dx9.h>
#include "XMeshStructures.h"

// --------------------------------------------------------------------------------
//									 IND_3dMesh
// --------------------------------------------------------------------------------

/*!
\defgroup IND_3dMesh IND_3dMesh
\ingroup Objects
IND_3dMesh class managed by IND_3dMeshManager for rendering 3d meshes. Click in IND_3dMesh to see all the methods of this class.
*/
/*@{*/

/*!
\b IND_3dMesh is an 3d mesh object from the class ::IND_3dMeshManager. Read the explanation in ::IND_3dMeshManager for having more details.
*/
class DLL_EXP IND_3dMesh
{
public:

	// ----- Public sets ------
	
	void				SetTransitionSpeed		(float pSpeed);
	void				SetAnimationSpeed		(float pSpeed);

	// ----- Public gets ------

	//! Return the name of the loaded 3d Mesh
	char				*GetMeshName			()	{ return m3dMesh.mAttributes.mName;				}
	//! Return the number of the sequence that is being rendered at this moment
	unsigned int		GetCurrentSequence		()	{ return m3dMesh.m_currentAnimationSet;			}
	//! Return the number of sequence animations of the 3d Mesh
	int					GetNumSequences			()	{ return m3dMesh.mAttributes.mNumSequences;		}
	//! Return the animation speed value
	float				GetAnimationSpeed		()	{ return m3dMesh.m_speedAdjust;					}
	//! Return the transition speed value
	float				GetTransitionSpeed		()	{ return m3dMesh.kMoveTransitionTime;			}

private:

	// ----- Private ------

	// Attributes
	struct structAttributes
	{
		char *mName;
		int mNumSequences;

		structAttributes ()
		{
			mName = 0;
			mNumSequences = 0;
		}
	};
	typedef struct structAttributes ATTRIBUTES;

	// TYPE
	struct struct3dMesh
	{
		// Direct3D objects required for animation
		LPD3DXFRAME                 m_frameRoot;
		LPD3DXANIMATIONCONTROLLER   m_animController;
		D3DXMESHCONTAINER_EXTENDED *m_firstMesh;

		// Bone data
		D3DXMATRIX *m_boneMatrices;
		int m_maxBones;

		// Animation variables
		unsigned int m_currentAnimationSet;	
		unsigned int m_currentTrack;
		float m_currentTime;
		float m_speedAdjust;

		// The time to change from one animation set to another
		// To see how the merging works - increase this time value to slow it down
		float kMoveTransitionTime;

		// Attributes
		ATTRIBUTES		mAttributes;

		struct3dMesh ()
		{
			m_maxBones = m_currentAnimationSet = m_currentTrack = 0;
			m_speedAdjust = 1.0f;
			m_currentTime = 0.0f;
			m_boneMatrices = 0;
			kMoveTransitionTime = 0.25f;
			m_firstMesh = 0;
		}
	};
	typedef struct struct3dMesh MESH3D;

	MESH3D m3dMesh;
	
	// ----- Friends -----
	
	friend class IND_Render;
	friend class IND_3dMeshManager;
};
/*@}*/

#endif // _IND_3DMESH_
