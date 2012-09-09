/*****************************************************************************************
/* File: IND_3dMeshManager.cpp
/* Desc: Manager de mallas 3d
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
#include "Defines.h"
#include "XMeshHierarchy.h"
#include "IND_3dMeshManager.h"


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pImageManager			Pointer to the manager IND_ImageManager correctly initialized
\arg \b pRender					Pointer to the manager IND_Render correctly initialized

\b Operation:   

This function returns 1 (true) if the adminstrator is successfully initialized.
Must be called before using any method.
*/
bool IND_3dMeshManager::Init (IND_Render *pRender)
{
	End ();
	InitVars ();

	// Checking IND_render
	if (pRender->IsOk())
	{
		Debug->Header ("Checking IND_Render", 1);
		mRender = pRender;	
		mOk = true;
	}
	else
	{
		Debug->Header ("IND_Render is not correctly initialized", 2);
		mOk = false;
		return mOk;
	}
	
	Debug->Header ("Initializing IND_3dMeshManager", 5);
	Debug->Header ("IND_3dMeshManager OK", 6);

	return mOk;
}


/*!
\b Operation:

This function frees the manager and all the objects that it contains.
*/
void IND_3dMeshManager::End()
{
	if (mOk)
	{
		// Free the subject if it has already loaded a mesh
		Debug->Header ("Finalizing IND_3dMeshManager", 5);
		Debug->Header ("Freeing 3d Meshes" , 5);
		FreeVars ();	
		Debug->Header ("3d meshes freed", 6);
		Debug->Header ("IND_3dMeshManager finalized", 6);
	
		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//									  Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pNew3dMesh				Pointer to a new IND_3dMesh object
\arg \b pName					Name of the 3d mesh
\arg \b pPath					Path where the 3d mesh is stored

\b Operation: 

This function adds a 3d Mesh from a .x file. You can use ::IND_Entity3d in order to display the 3d mesh on the screen and to change its
attributes.
*/
bool IND_3dMeshManager::Add (IND_3dMesh *pNew3dMesh, char *pName, char *pPath)
{
	Debug->Header ("Loading 3d Mesh", 5);
	Debug->Header ("File name:", 3);
	Debug->DataChar (pName, 1);

	if (!mOk)
	{	
		WriteMessage ();
		return 0;
	}

	// Create our mesh hierarchy class to control the allocation of memory - only used temporarily
	XMeshHierarchy *memoryAllocator = new XMeshHierarchy;
	memoryAllocator->mPath = pPath;

	// This is the function that does all the .x file loading. We provide a pointer to an instance of our 
	// memory allocator class to handle memory allocation during the frame and mesh loading
	HRESULT hr = D3DXLoadMeshHierarchyFromX (pName, D3DXMESH_MANAGED, mRender->GetDevice(), 
			memoryAllocator, NULL, &pNew3dMesh->m3dMesh.m_frameRoot, &pNew3dMesh->m3dMesh.m_animController);

	delete memoryAllocator;
	memoryAllocator = 0;
	
	if (hr != D3D_OK)
	{
		Debug->Header ("3d Mesh not found or error loading the mesh", 2);
		return 0;
	}

	// Name
	pNew3dMesh->m3dMesh.mAttributes.mName = pName;

	// If the x file contains any animation remember how many sets there are
	if (pNew3dMesh->m3dMesh.m_animController)
	{
		pNew3dMesh->m3dMesh.mAttributes.mNumSequences = pNew3dMesh->m3dMesh.m_animController->GetMaxNumAnimationSets();
	}

	// Bones for skining
	if(pNew3dMesh->m3dMesh.m_frameRoot)
	{
		// Set the bones up
		SetupBoneMatrices ((D3DXFRAME_EXTENDED*) pNew3dMesh->m3dMesh.m_frameRoot, NULL, pNew3dMesh);

		// Create the bone matrices array for use during FrameMove to hold the final transform
		pNew3dMesh->m3dMesh.m_boneMatrices  = new D3DXMATRIX [pNew3dMesh->m3dMesh.m_maxBones];
		ZeroMemory (pNew3dMesh->m3dMesh.m_boneMatrices, sizeof(D3DXMATRIX)*pNew3dMesh->m3dMesh.m_maxBones);
	}

	// ----- Sets first sequence -----

	if (pNew3dMesh->m3dMesh.m_animController)
		mRender->Set3dMeshSequence (pNew3dMesh, 0);

	// ----- Puts the object into the manager -----

	AddToList (pNew3dMesh);

	Debug->Header("3d Mesh loaded", 6);	

	return 1;
}


/*!
\b Parameters:

\arg \b pMe                 Pointer to an 3d Mesh object.

\b Operation:               

This function returns 1 (true) if the 3d Mesh object passed as parameter exists and is
deleted from the manager successfully.
*/
bool IND_3dMeshManager::Delete (IND_3dMesh *pMe)
{	
	Debug->Header ("Freeing 3d Mesh", 5);

	if (!mOk || !pMe)
	{	
		WriteMessage ();
		return 0;
	}

	// Search object
	bool mIs = 0;
	list <IND_3dMesh*>::iterator mListIter;
	for (mListIter  = mList3dMesh->begin();
		 mListIter != mList3dMesh->end();
		 mListIter++)
	{	
		if ((*mListIter) == pMe) 
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

	Debug->Header	("Freeing 3d mesh:", 3);
	Debug->DataChar (pMe->GetMeshName (), 1);

	// Quit from list
	DelFromList (pMe);
	
	// Free mesh
	if (pMe->m3dMesh.m_animController)
	{
		pMe->m3dMesh.m_animController->Release();
		pMe->m3dMesh.m_animController = 0;
	}

	// free hierarchy
	if (pMe->m3dMesh.m_frameRoot)
	{
		// Create a mesh heirarchy class to control the removal of memory for the frame heirarchy
		XMeshHierarchy memoryAllocator;
		D3DXFrameDestroy (pMe->m3dMesh.m_frameRoot, &memoryAllocator);
		pMe->m3dMesh.m_frameRoot = 0;
	}

	// Free bones
	if (pMe->m3dMesh.m_boneMatrices)
	{
		DisposeArray (pMe->m3dMesh.m_boneMatrices);
	}

	// Free the pointer
	Dispose (pMe);

	Debug->Header ("Ok", 6);

	return 1;
}


// --------------------------------------------------------------------------------
//									  Private methods
// --------------------------------------------------------------------------------

/*
==================
We need to go through the hierarchy and set the combined matrices calls recursively
pFrame - current frame
pParentMatrix - the parent frame matrix
==================
*/
void IND_3dMeshManager::SetupBoneMatrices (D3DXFRAME_EXTENDED *pFrame, LPD3DXMATRIX pParentMatrix, IND_3dMesh *pNew3dMesh)
{
	// Cast to our extended structure first
	D3DXMESHCONTAINER_EXTENDED* pMesh = (D3DXMESHCONTAINER_EXTENDED*) pFrame->pMeshContainer;

	// If this frame has a mesh
	if (pMesh)
	{
		// We need to remember which is the first mesh in the hierarchy for later when we 
		// update (FrameMove)
		if(!pNew3dMesh->m3dMesh.m_firstMesh)
			pNew3dMesh->m3dMesh.m_firstMesh = pMesh;
		
		// if there is skin info, then setup the bone matrices
		if(pMesh->pSkinInfo && pMesh->MeshData.pMesh)
		{
			// Create a copy of the mesh to skin into later
			D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
			if (FAILED(pMesh->MeshData.pMesh->GetDeclaration(Declaration)))
				return;

			pMesh->MeshData.pMesh->CloneMesh(D3DXMESH_MANAGED, 
				Declaration, mRender->GetDevice(), 
				&pMesh->exSkinMesh);

			// Max bones is calculated for later use (to know how big to make the bone matrices array)
			pNew3dMesh->m3dMesh.m_maxBones = max (pNew3dMesh->m3dMesh.m_maxBones, (int)pMesh->pSkinInfo->GetNumBones());

			// For each bone work out its matrix
			for (unsigned int i = 0; i < pMesh->pSkinInfo->GetNumBones(); i++)
			{   
				// Find the frame containing the bone
				D3DXFRAME_EXTENDED* pTempFrame = (D3DXFRAME_EXTENDED*)D3DXFrameFind (pNew3dMesh->m3dMesh.m_frameRoot, 
						pMesh->pSkinInfo->GetBoneName(i));

				// set the bone part - point it at the transformation matrix
				pMesh->exFrameCombinedMatrixPointer[i] = &pTempFrame->exCombinedTransformationMatrix;
			}

		}
	}

	// Pass on to sibblings
	if(pFrame->pFrameSibling)
		SetupBoneMatrices((D3DXFRAME_EXTENDED*)pFrame->pFrameSibling, pParentMatrix, pNew3dMesh);

	// Pass on to children
	if(pFrame->pFrameFirstChild)
		SetupBoneMatrices((D3DXFRAME_EXTENDED*)pFrame->pFrameFirstChild, &pFrame->exCombinedTransformationMatrix, pNew3dMesh);
}


/*
==================
Inserts object into the manager
==================
*/
void IND_3dMeshManager::AddToList (IND_3dMesh *pNew3dMesh)
{
	mList3dMesh->push_back (pNew3dMesh);
}


/*
==================
Deletes object from the manager
==================
*/
void IND_3dMeshManager::DelFromList (IND_3dMesh *pMe)
{
	mList3dMesh->remove (pMe);
	pMe = 0;
}


/*
==================
Initialization error message
==================
*/
void IND_3dMeshManager::WriteMessage ()
{
	Debug->Header ("This operation can not be done", 3);
	Debug->DataChar ("", 1);
	Debug->Header ("Invalid Id or IND_3dMeshManager not correctly initialized", 2);
}


/*
==================
Init manager vars
==================
*/
void IND_3dMeshManager::InitVars ()
{
	mList3dMesh = new list <IND_3dMesh*>;
}


/*
==================
Free memory
==================
*/
void IND_3dMeshManager::FreeVars ()
{
	// Deletes all the manager entities
	list <IND_3dMesh*>::iterator mListIter;

	for (mListIter  = mList3dMesh->begin();
			mListIter != mList3dMesh->end();
			mListIter++)
	{
		Debug->Header	("Freeing 3d mesh:", 3);
		Debug->DataChar ((*mListIter)->GetMeshName (), 1);

		// Free mesh
		if ((*mListIter)->m3dMesh.m_animController)
		{
			(*mListIter)->m3dMesh.m_animController->Release();
			(*mListIter)->m3dMesh.m_animController = 0;
		}

		// Free heirarchy
		if ((*mListIter)->m3dMesh.m_frameRoot)
		{
			// Create a mesh heirarchy class to control the removal of memory for the frame heirarchy
			XMeshHierarchy memoryAllocator;
			D3DXFrameDestroy ((*mListIter)->m3dMesh.m_frameRoot, &memoryAllocator);
			(*mListIter)->m3dMesh.m_frameRoot = 0;
		}

		// Free bones
		if ((*mListIter)->m3dMesh.m_boneMatrices)
		{
			DisposeArray ((*mListIter)->m3dMesh.m_boneMatrices);
		}
	}

	// Clear list
	mList3dMesh->clear ();

	// Free list
	Dispose (mList3dMesh);
}
