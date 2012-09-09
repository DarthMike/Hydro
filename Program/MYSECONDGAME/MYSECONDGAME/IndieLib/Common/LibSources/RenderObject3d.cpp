/*****************************************************************************************
/* File: RenderObject3d.cpp
/* Desc: Blitting of 3d objects using D3D
/* Modification of the Keith Ditchburn class
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
#include "IND_SurfaceManager.h"
#include "IND_Render.h"


// --------------------------------------------------------------------------------
//							         Public methods
// --------------------------------------------------------------------------------

/*!
\defgroup Graphical_3d_Objects Bliting 3d Animated 3d Models and setting the transformations directly
\ingroup Advances
*/
/*@{*/

/*!
\b Parameters:

\arg \b p3dMesh						Pointer to a ::IND_3dMesh object

\b Operation: 

This function blits directly to the screen a ::IND_3dMesh object. 

In order to change the transformations
and color attributes of the 3d mesh you have to use the IND_Render::SetTransform3d() and IND_Render::SetRainbow()
methods before calling to this function. Remember that you can use IND_Entity3d object for drawing 3d meshes to 
the screen without having to use this advanced methods directly. This method is only useful for advanced users 
with really concrete purposes.

Using this method is equivalent to using:
- IND_Entity3d::Set3dMesh()
*/
inline void IND_Render::Blit3dMesh	(IND_3dMesh *p3dMesh)
{
	D3DXMATRIX mMatrix;
	mInfo.mDevice->GetTransform (D3DTS_WORLD, &mMatrix);

	FrameMove (p3dMesh, GetFrameTime() * 0.001f , &mMatrix);

	if (p3dMesh->m3dMesh.m_frameRoot)
		DrawFrame (p3dMesh->m3dMesh.m_frameRoot);
}


/*!
\b Parameters:

\arg \b p3dMesh						Pointer to a ::IND_3dMesh object
\arg \b pIndex						Index of the animation

\b Operation:  

This function sets the animation sequence that will be rendered. When you use this method, the transition between the
current animation and the following will not be abrubt. IndieLib will fade out the effect of the first animation 
and fade in the second. Use IND_3dMesh::SetTransitionSpeed() in order set the speed of that transition.
*/
void IND_Render::Set3dMeshSequence (IND_3dMesh *p3dMesh, unsigned int pIndex)
{	
	if (pIndex == p3dMesh->m3dMesh.m_currentAnimationSet)
		return;

	if (pIndex >= (unsigned int) p3dMesh->m3dMesh.mAttributes.mNumSequences )
		pIndex = 0;

	// Remember current animation
	p3dMesh->m3dMesh.m_currentAnimationSet = pIndex;

	// Get the animation set from the controller
	LPD3DXANIMATIONSET set;
	p3dMesh->m3dMesh.m_animController->GetAnimationSet(p3dMesh->m3dMesh.m_currentAnimationSet, &set );	

	// Note: for a smooth transition between animation sets we can use two tracks and assign the new set to the track
	// not currently playing then insert Keys into the KeyTrack to do the transition between the tracks
	// tracks can be mixed together so we can gradually change into the new animation

	// Alternate tracks
	DWORD newTrack = ( p3dMesh->m3dMesh.m_currentTrack == 0 ? 1 : 0 );

	// Assign to our track
	p3dMesh->m3dMesh.m_animController->SetTrackAnimationSet( newTrack, set );
    set->Release();	

	// Clear any track events currently assigned to our two tracks
	p3dMesh->m3dMesh.m_animController->UnkeyAllTrackEvents( p3dMesh->m3dMesh.m_currentTrack );
    p3dMesh->m3dMesh.m_animController->UnkeyAllTrackEvents( newTrack );

	// Add an event key to disable the currently playing track kMoveTransitionTime seconds in the future
    p3dMesh->m3dMesh.m_animController->KeyTrackEnable( p3dMesh->m3dMesh.m_currentTrack, FALSE, p3dMesh->m3dMesh.m_currentTime + p3dMesh->m3dMesh.kMoveTransitionTime );
	// Add an event key to change the speed right away so the animation completes in kMoveTransitionTime seconds
    p3dMesh->m3dMesh.m_animController->KeyTrackSpeed( p3dMesh->m3dMesh.m_currentTrack, 0.0f, p3dMesh->m3dMesh.m_currentTime, p3dMesh->m3dMesh.kMoveTransitionTime, D3DXTRANSITION_LINEAR );
	// Add an event to change the weighting of the current track (the effect it has blended with the secon track)
    p3dMesh->m3dMesh.m_animController->KeyTrackWeight( p3dMesh->m3dMesh.m_currentTrack, 0.0f, p3dMesh->m3dMesh.m_currentTime, p3dMesh->m3dMesh.kMoveTransitionTime, D3DXTRANSITION_LINEAR );

	// Enable the new track
    p3dMesh->m3dMesh.m_animController->SetTrackEnable( newTrack, TRUE );
	// Add an event key to set the speed of the track
    p3dMesh->m3dMesh.m_animController->KeyTrackSpeed( newTrack, 1.0f, p3dMesh->m3dMesh.m_currentTime, p3dMesh->m3dMesh.kMoveTransitionTime, D3DXTRANSITION_LINEAR );
	// Add an event to change the weighting of the current track (the effect it has blended with the first track)
	// As you can see this will go from 0 effect to total effect(1.0f) in kMoveTransitionTime seconds and the first track goes from 
	// total to 0.0f in the same time.
    p3dMesh->m3dMesh.m_animController->KeyTrackWeight( newTrack, 1.0f, p3dMesh->m3dMesh.m_currentTime, p3dMesh->m3dMesh.kMoveTransitionTime, D3DXTRANSITION_LINEAR );

	// Remember current track
    p3dMesh->m3dMesh.m_currentTrack = newTrack;
}
/*@}*/	

// --------------------------------------------------------------------------------
//							         Private methods
// --------------------------------------------------------------------------------

/*
==================
Called to render a frame in the hierarchy
device - the Direct3D device object
frame - frame to render
==================
*/
void IND_Render::DrawFrame(LPD3DXFRAME pFrame)
{
	// Draw all mesh containers in this frame
    LPD3DXMESHCONTAINER meshContainer = pFrame->pMeshContainer;
    while (meshContainer)
    {
        DrawMeshContainer(meshContainer, pFrame);
        meshContainer = meshContainer->pNextMeshContainer;
    }

	// Recurse for sibblings
    if (pFrame->pFrameSibling != NULL)
        DrawFrame(pFrame->pFrameSibling);

    // Recurse for children
	if (pFrame->pFrameFirstChild != NULL)
        DrawFrame(pFrame->pFrameFirstChild);
}


/*
==================
Called to render a mesh
device - the Direct3D device object
meshContainerBase - the mesh container
frameBase - frame containing the mesh
==================
*/
void IND_Render::DrawMeshContainer(LPD3DXMESHCONTAINER meshContainerBase, LPD3DXFRAME frameBase)
{
	// Cast to our extended frame type
	D3DXFRAME_EXTENDED *frame = (D3DXFRAME_EXTENDED*)frameBase;		

	// Cast to our extended mesh container
	D3DXMESHCONTAINER_EXTENDED *meshContainer = (D3DXMESHCONTAINER_EXTENDED*)meshContainerBase;
	
	// Set the world transform
    GetDevice()->SetTransform(D3DTS_WORLD, &frame->exCombinedTransformationMatrix);

	// Loop through all the materials in the mesh rendering each subset
    for (unsigned int iMaterial = 0; iMaterial < meshContainer->NumMaterials; iMaterial++)
    {
		// use the material in our extended data rather than the one in meshContainer->pMaterials[iMaterial].MatD3D
		GetDevice()->SetMaterial( &meshContainer->exMaterials[iMaterial] );
		GetDevice()->SetTexture( 0, meshContainer->exTextures[iMaterial] );

		// Select the mesh to draw, if there is skin then use the skinned mesh else the normal one
		LPD3DXMESH pDrawMesh = (meshContainer->pSkinInfo) ? meshContainer->exSkinMesh: meshContainer->MeshData.pMesh;

		// Finally Call the mesh draw function
        pDrawMesh->DrawSubset(iMaterial);
    }
}


/*
==================
Called each frame update with the time and the current world matrix
elapsedTime - time passed
matWorld - current world matrix for the model
==================
*/
void IND_Render::FrameMove (IND_3dMesh *p3dMesh, float elapsedTime, const D3DXMATRIX *matWorld)
{
	// Adjust animation speed
	elapsedTime /= p3dMesh->m3dMesh.m_speedAdjust;

	// Advance the time and set in the controller
    if (p3dMesh->m3dMesh.m_animController != NULL)
        p3dMesh->m3dMesh.m_animController->AdvanceTime (elapsedTime, NULL);

	p3dMesh->m3dMesh.m_currentTime += elapsedTime;

	// Now update the model matrices in the hierarchy
    UpdateFrameMatrices(p3dMesh->m3dMesh.m_frameRoot, matWorld);

	// If the model contains a skinned mesh update the vertices
	D3DXMESHCONTAINER_EXTENDED *pMesh = p3dMesh->m3dMesh.m_firstMesh;

	if (pMesh && pMesh->pSkinInfo)
	{	
		unsigned int Bones = pMesh->pSkinInfo->GetNumBones();

		// Create the bone matrices that transform each bone from bone space into character space
		// (via exFrameCombinedMatrixPointer) and also wraps the mesh around the bones using the bone offsets
		// in exBoneOffsetsArray
		for (unsigned int i = 0; i < Bones; ++i)
			D3DXMatrixMultiply(&p3dMesh->m3dMesh.m_boneMatrices[i],&pMesh->exBoneOffsets[i], pMesh->exFrameCombinedMatrixPointer[i]);

		// We need to modify the vertex positions based on the new bone matrices. This is achieved
		// by locking the vertex buffers and then calling UpdateSkinnedMesh. UpdateSkinnedMesh takes the
		// original vertex data (in pMesh->MeshData.pMesh), applies the matrices and writes the new vertices
		// out to skin mesh (pMesh->exSkinMesh). 

		// UpdateSkinnedMesh uses software skinning which is the slowest way of carrying out skinning 
		// but is easiest to describe and works on the majority of graphic devices. 
		// Other methods exist that use hardware to do this skinning - see the notes and the 
		// DirectX SDK skinned mesh sample for more details
		void *srcPtr=0;
		pMesh->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&srcPtr);

		void *destPtr=0;
		pMesh->exSkinMesh->LockVertexBuffer(0, (void**)&destPtr);

		// Update the skinned mesh 
		pMesh->pSkinInfo->UpdateSkinnedMesh(p3dMesh->m3dMesh.m_boneMatrices, NULL, srcPtr, destPtr);

		// Unlock the meshes vertex buffers
		pMesh->exSkinMesh->UnlockVertexBuffer();
		pMesh->MeshData.pMesh->UnlockVertexBuffer();
	}	
}


/*
==================
Called to update the frame matrices in the hierarchy to reflect current animation stage
frameBase - frame being looked at
parentMatrix - the matrix of our parent (if we have one)
==================
*/
void IND_Render::UpdateFrameMatrices(const D3DXFRAME *frameBase, const D3DXMATRIX *parentMatrix)
{
    D3DXFRAME_EXTENDED *currentFrame = (D3DXFRAME_EXTENDED*)frameBase;

	// If parent matrix exists multiply our frame matrix by it
    if (parentMatrix != NULL)
        D3DXMatrixMultiply(&currentFrame->exCombinedTransformationMatrix, &currentFrame->TransformationMatrix, parentMatrix);
    else
        currentFrame->exCombinedTransformationMatrix = currentFrame->TransformationMatrix;

	// If we have a sibling recurse 
    if (currentFrame->pFrameSibling != NULL)
        UpdateFrameMatrices(currentFrame->pFrameSibling, parentMatrix);

	// If we have a child recurse 
    if (currentFrame->pFrameFirstChild != NULL)
        UpdateFrameMatrices(currentFrame->pFrameFirstChild, &currentFrame->exCombinedTransformationMatrix);
}
