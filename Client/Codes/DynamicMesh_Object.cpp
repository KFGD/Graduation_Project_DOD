#include "stdafx.h"
#include "DynamicMesh_Object.h"

#include "HierarchyLoader.h"
#include "AnimationCtrl.h"

#include "Shader.h"
#include "PipeLine.h"

#include <iostream>

DynamicMesh_Object::DynamicMesh_Object()
{
}

DynamicMesh_Object::DynamicMesh_Object(const DynamicMesh_Object & rhs)
	: Component_Object(rhs)
	, mMeshContainerList(rhs.mMeshContainerList)
	, mRootFrame(rhs.mRootFrame)
	, mPivotMatrix(rhs.mPivotMatrix)
	, mHierarchyLoader(rhs.mHierarchyLoader)
	, mAnimationCtrl(nullptr)
	, mIsClone(true)
{
	SafeAddRef(mHierarchyLoader);
}

_bool DynamicMesh_Object::UpdateHardwareSkinnedMesh(const _int meshContainerIndex)
{
	D3DXMESHCONTAINER_DERIVED* meshContainer = mMeshContainerList[meshContainerIndex];
	LPD3DXBONECOMBINATION boneComb = (LPD3DXBONECOMBINATION)(meshContainer->pBoneCombinationBuf->GetBufferPointer());

	for (_int i = 0; i < (_int)meshContainer->dwNumAttributeGroups; ++i)
	{
		for (_int paletteEntry = 0; paletteEntry < (_int)meshContainer->dwNumPaletteEntries; ++paletteEntry)
		{
			_int matrixIndex = boneComb[i].BoneId[paletteEntry];

			if (UINT_MAX != matrixIndex)
			{
				meshContainer->pRenderMatrices[paletteEntry] = meshContainer->pOffsetMatrices[matrixIndex] * *meshContainer->ppCombinedTransformationMatrices[matrixIndex];
			}
		}
	}

	return true;
}

_bool DynamicMesh_Object::SetUpAnimation(const _uint index)
{
	if (nullptr == mAnimationCtrl)
		return false;

	mAnimationCtrl->SetUpAnimation(index);
	return true;
}

_bool DynamicMesh_Object::PlayAnimation(const _double timeDelta)
{
	if (nullptr == mAnimationCtrl)
		return false;

	mAnimationCtrl->PlayAnimation(timeDelta);

	UpdateCombinedTransformationMatrices(mRootFrame, mPivotMatrix);

	return true;
}

void DynamicMesh_Object::Render(Shader * shader, const _int meshContainerIndex)
{
	D3DXMESHCONTAINER_DERIVED* meshContainer = mMeshContainerList[meshContainerIndex];
	LPD3DXBONECOMBINATION boneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(meshContainer->pBoneCombinationBuf->GetBufferPointer());

	shader->BeginShader(nullptr);
	shader->BeginPass(0);

	for (_int i = 0; i < (_int)meshContainer->dwNumAttributeGroups; ++i)
	{
		for (_int paletteEntry = 0; paletteEntry < (_int)meshContainer->dwNumPaletteEntries; ++paletteEntry)
		{
			_int matrixIndex = boneComb[i].BoneId[paletteEntry];

			if (UINT_MAX != matrixIndex)
				D3DXMatrixMultiply(&meshContainer->pRenderMatrices[paletteEntry], &meshContainer->pOffsetMatrices[matrixIndex], meshContainer->ppCombinedTransformationMatrices[matrixIndex]);
		}
		_ulong numInflu = meshContainer->dwNumInfl - 1;

		shader->Get_EffectHandle()->SetMatrixArray("gMatrixPalette", meshContainer->pRenderMatrices, meshContainer->dwNumPaletteEntries);

		shader->Get_EffectHandle()->SetValue("gNumBoneInfluences", &numInflu, sizeof(_ulong));
		shader->SetTexture("gDiffuseTexture", meshContainer->pMeshTexture[boneComb[i].AttribId]);
		shader->CommitChanges();
		meshContainer->MeshData.pMesh->DrawSubset(i);

	}

	shader->EndPass();
	shader->EndShader();
}

_bool DynamicMesh_Object::Initialize(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix& pivotMatrix)
{
	_tchar fullPath[MAX_PATH] = L"";

	LPD3DXMESH	mesh = nullptr;

	lstrcpy(fullPath, filePath);
	lstrcat(fullPath, fileName);

	mHierarchyLoader = HierarchyLoader::Create(graphicDevice, filePath);
	if (nullptr == mHierarchyLoader)
		return false;

	LPD3DXANIMATIONCONTROLLER	dxAniCtrl = nullptr;

	if (FAILED(D3DXLoadMeshHierarchyFromX(fullPath, D3DXMESH_MANAGED, graphicDevice, mHierarchyLoader, nullptr, &mRootFrame, &dxAniCtrl)))
		return false;

	mAnimationCtrl = AnimationCtrl::Create(dxAniCtrl);
	if (nullptr == mAnimationCtrl)
		return false;

	SafeRelease(dxAniCtrl);

	mPivotMatrix = pivotMatrix;

	SetUpCombinedTransformationMatricesPointer(mRootFrame);
	UpdateCombinedTransformationMatrices(mRootFrame, mPivotMatrix);
	
	for (_int i = 0; i < mMeshContainerList.size(); ++i)
		UpdateSoftwareSkinnedMesh(i);

	//for (_int i = 0; i < mMeshContainerList.size(); ++i)
	//{
	//	LPD3DXMESH mesh = mMeshContainerList[i]->pOriginalMesh;
	//	
	//	const _ulong numVertices = mesh->GetNumVertices();
	//	const _ulong stride = D3DXGetFVFVertexSize(mesh->GetFVF());

	//	D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
	//	mesh->GetDeclaration(decl);

	//	_ushort normalOffset = 0;
	//	for (_size_t i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	//	{
	//		if (D3DDECLUSAGE_NORMAL == decl[i].Usage)
	//		{
	//			normalOffset = decl[i].Offset;
	//			break;
	//		}
	//	}

	//	_byte*	vertices = nullptr;
	//	mesh->LockVertexBuffer(0, (void**)&vertices);

	//	for (_ulong i = 0; i < numVertices; ++i)
	//	{
	//		_vec3* pos = (_vec3*)(vertices + (i * stride));
	//		_vec3* normal = (_vec3*)(vertices + (i * stride) + normalOffset);
	//		D3DXVec3TransformCoord(pos, pos, &mPivotMatrix);
	//		D3DXVec3TransformNormal(normal, normal, &mPivotMatrix);
	//	}

	//	mesh->UnlockVertexBuffer();

	//}

	return true;
}

_bool DynamicMesh_Object::UpdateCombinedTransformationMatrices(D3DXFRAME* frame, const _matrix& parentMatrix)
{
	D3DXFRAME_DERIVED*	pFrame_Derived = (D3DXFRAME_DERIVED*)frame;

	pFrame_Derived->CombinedTransformationMatrix = pFrame_Derived->TransformationMatrix * parentMatrix;

	if (nullptr != pFrame_Derived->pFrameSibling)
		UpdateCombinedTransformationMatrices(pFrame_Derived->pFrameSibling, parentMatrix);

	if (nullptr != pFrame_Derived->pFrameFirstChild)
		UpdateCombinedTransformationMatrices(pFrame_Derived->pFrameFirstChild, pFrame_Derived->CombinedTransformationMatrix);


	return true;
}

_bool DynamicMesh_Object::SetUpCombinedTransformationMatricesPointer(D3DXFRAME * frame)
{
	if (nullptr != frame->pMeshContainer)
	{
		D3DXMESHCONTAINER_DERIVED* pMeshContainer_Derived = (D3DXMESHCONTAINER_DERIVED*)frame->pMeshContainer;

		if (pMeshContainer_Derived->pSkinInfo != nullptr)
		{

			for (_ulong i = 0; i < pMeshContainer_Derived->dwNumBones; ++i)
			{
				LPCSTR name = pMeshContainer_Derived->pSkinInfo->GetBoneName(i);
				D3DXFRAME_DERIVED*	pFrame_Derived = (D3DXFRAME_DERIVED*)D3DXFrameFind(mRootFrame, name);

				pMeshContainer_Derived->ppCombinedTransformationMatrices[i] = &pFrame_Derived->CombinedTransformationMatrix;
			}

			mMeshContainerList.push_back(pMeshContainer_Derived);
		}
	}

	if (nullptr != frame->pFrameFirstChild)
		SetUpCombinedTransformationMatricesPointer(frame->pFrameFirstChild);

	if (nullptr != frame->pFrameSibling)
		SetUpCombinedTransformationMatricesPointer(frame->pFrameSibling);

	return true;
}


_bool DynamicMesh_Object::UpdateSoftwareSkinnedMesh(const _int meshContainerIndex)
{
	for (_ulong i = 0; i < mMeshContainerList[meshContainerIndex]->dwNumBones; ++i)
		mMeshContainerList[meshContainerIndex]->pRenderMatrices[i] = mMeshContainerList[meshContainerIndex]->pOffsetMatrices[i] * *mMeshContainerList[meshContainerIndex]->ppCombinedTransformationMatrices[i];

	void	*pSourVtx = nullptr, *pDestVtx = nullptr;

	mMeshContainerList[meshContainerIndex]->pOriginalMesh->LockVertexBuffer(0, &pSourVtx);
	mMeshContainerList[meshContainerIndex]->pSoftwareMesh->LockVertexBuffer(0, &pDestVtx);

	mMeshContainerList[meshContainerIndex]->pSkinInfo->UpdateSkinnedMesh(mMeshContainerList[meshContainerIndex]->pRenderMatrices, nullptr, pSourVtx, pDestVtx);

	mMeshContainerList[meshContainerIndex]->pOriginalMesh->UnlockVertexBuffer();
	mMeshContainerList[meshContainerIndex]->pSoftwareMesh->UnlockVertexBuffer();

	return true;
}

void DynamicMesh_Object::RenderHardwareSkinningRecursive(Shader * shader, const _matrix & worldMatrix, D3DXFRAME * bone)
{
	D3DXFRAME_DERIVED* bone_derived = (D3DXFRAME_DERIVED*)bone;
	
	if (nullptr != bone_derived->pMeshContainer)
	{

		D3DXMESHCONTAINER_DERIVED* meshContainer = (D3DXMESHCONTAINER_DERIVED*)bone_derived->pMeshContainer;
		
		if (nullptr != meshContainer->pSkinInfo)
		{

			_matrix matTemp;
			LPD3DXBONECOMBINATION boneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(meshContainer->pBoneCombinationBuf->GetBufferPointer());


			for (_int i = 0; i < (_int)meshContainer->dwNumAttributeGroups; ++i)
			{
				for (_int paletteEntry = 0; paletteEntry < (_int)meshContainer->dwNumPaletteEntries; ++paletteEntry)
				{
					_int matrixIndex = boneComb[i].BoneId[paletteEntry];

					if (UINT_MAX != matrixIndex)
					{
						
						D3DXMatrixMultiply(&matTemp, &meshContainer->pOffsetMatrices[matrixIndex], meshContainer->ppCombinedTransformationMatrices[matrixIndex]);
						meshContainer->pRenderMatrices[paletteEntry] = matTemp;
					}
				}
				_ulong temp = meshContainer->dwNumInfl - 1;

				shader->Get_EffectHandle()->SetMatrixArray("gMatrixPalette", meshContainer->pRenderMatrices, meshContainer->dwNumPaletteEntries);
				
				shader->Get_EffectHandle()->SetValue("gNumBoneInfluences", &temp, sizeof(_ulong));
				shader->SetTexture("gDiffuseTexture", meshContainer->pMeshTexture[boneComb[i].AttribId]);

				shader->BeginShader(nullptr);
				shader->BeginPass(0);
				meshContainer->MeshData.pMesh->DrawSubset(i);
				shader->EndPass();
				shader->EndShader();

			}
		}
	}

	if(nullptr != bone->pFrameSibling)
		RenderHardwareSkinningRecursive(shader, worldMatrix, bone->pFrameSibling);

	if (nullptr != bone->pFrameFirstChild)
		RenderHardwareSkinningRecursive(shader, worldMatrix, bone->pFrameFirstChild);
	
}

DynamicMesh_Object * DynamicMesh_Object::Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix& pivotMatrix)
{
	DynamicMesh_Object*	pInstance = new DynamicMesh_Object();
	if (false == pInstance->Initialize(graphicDevice, filePath, fileName, pivotMatrix))
	{
		MSG_BOX("Failed To Create DynamicMesh_Object Instance");
		SafeRelease(pInstance);
	}

	return pInstance;
}

Component_Object * DynamicMesh_Object::Clone(void * arg)
{
	DynamicMesh_Object*	pInstance = new DynamicMesh_Object(*this);

	pInstance->mAnimationCtrl = this->mAnimationCtrl->Clone();
	if (nullptr == pInstance->mAnimationCtrl)
	{
		MSG_BOX("DynamicMesh_Object Clone Failed!");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void DynamicMesh_Object::Free()
{
	if (false == mIsClone)
	{
		mHierarchyLoader->DestroyFrame(mRootFrame);
	}

	mMeshContainerList.clear();

	SafeRelease(mAnimationCtrl);
	SafeRelease(mHierarchyLoader);

	Component_Object::Free();
}

