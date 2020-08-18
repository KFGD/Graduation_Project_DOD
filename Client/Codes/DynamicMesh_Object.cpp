#include "stdafx.h"
#include "DynamicMesh_Object.h"

#include "HierarchyLoader.h"
#include "AnimationCtrl.h"

#include "Shader.h"
#include "PipeLine.h"

#include <iostream>

//FILE* pFile;

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

_bool DynamicMesh_Object::UpdateSoftwareSkinnedMesh(const _int meshContainerIndex)
{
	for (_ulong i = 0; i < mMeshContainerList[meshContainerIndex]->dwNumBones; ++i)
		mMeshContainerList[meshContainerIndex]->pRenderMatrices[i] = mMeshContainerList[meshContainerIndex]->pOffsetMatrices[i] * *mMeshContainerList[meshContainerIndex]->ppCombinedTransformationMatrices[i];

	void	*pSourVtx = nullptr, *pDestVtx = nullptr;

	mMeshContainerList[meshContainerIndex]->pOriginalMesh->LockVertexBuffer(0, &pSourVtx);
	mMeshContainerList[meshContainerIndex]->MeshData.pMesh->LockVertexBuffer(0, &pDestVtx);

	mMeshContainerList[meshContainerIndex]->pSkinInfo->UpdateSkinnedMesh(mMeshContainerList[meshContainerIndex]->pRenderMatrices, nullptr, pSourVtx, pDestVtx);

	mMeshContainerList[meshContainerIndex]->pOriginalMesh->UnlockVertexBuffer();
	mMeshContainerList[meshContainerIndex]->MeshData.pMesh->UnlockVertexBuffer();

	return true;
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

void DynamicMesh_Object::Render(LPDIRECT3DDEVICE9 graphicDevice, const _int meshContainerIndex)
{
	D3DXMESHCONTAINER_DERIVED* meshContainer = mMeshContainerList[meshContainerIndex];
	for (_uint i = 0; i < meshContainer->NumMaterials; ++i)
	{
		graphicDevice->SetTexture(0, meshContainer->pMeshTexture[i]);
		meshContainer->MeshData.pMesh->DrawSubset(i);
	}

}

void DynamicMesh_Object::RenderHardwareSkinning(Shader* shader, const _int meshContainerIndex, const _matrix& worldMatrix)
{
	D3DXMESHCONTAINER_DERIVED* meshContainer = mMeshContainerList[meshContainerIndex];
	LPD3DXBONECOMBINATION boneComb = (LPD3DXBONECOMBINATION)(meshContainer->pBoneCombinationBuf->GetBufferPointer());

	shader->BeginShader(nullptr);
	shader->BeginPass(0);
	for (_int i = 0; i < (_int)meshContainer->dwNumAttributeGroups; ++i)
	{
		_int temp = meshContainer->dwNumInfl - 1;
		shader->Get_EffectHandle()->SetMatrixArray("gMatrixPalette", meshContainer->pRenderMatrices, meshContainer->dwNumPaletteEntries);
		shader->SetValue("gNumBoneInfluences", &temp, sizeof(unsigned long));

		const _int mtrlIndex = boneComb[i].AttribId;
		shader->SetTexture("gDiffuseTexture", meshContainer->pMeshTexture[mtrlIndex]);
		shader->CommitChanges();

		meshContainer->MeshData.pMesh->DrawSubset(i);

	}
	shader->EndPass();
	shader->EndShader();

}

void DynamicMesh_Object::RenderHardwareSkinning(Shader * shader, const _matrix & worldMatrix)
{
	//shader->BeginShader(nullptr);
	//shader->BeginPass(0);

	//fopen_s(&pFile, "test.txt", "w");

	RenderHardwareSkinningRecursive(shader, worldMatrix, mRootFrame);

	//fclose(pFile);

	//shader->EndPass();
	//shader->EndShader();
}


_bool DynamicMesh_Object::Initialize(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName)
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

	_matrix scale = *D3DXMatrixScaling(&scale, 0.01f, 0.01f, 0.01f);

	mPivotMatrix = scale;

	SetUpCombinedTransformationMatricesPointer(mRootFrame);
	UpdateCombinedTransformationMatrices(mRootFrame, mPivotMatrix);

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


			for (_int i = 0; i < meshContainer->dwNumAttributeGroups; ++i)
			{
				//fprintf(pFile, "$$$$$$$$$$$$$$ %s\n", meshContainer->Name);
				//fprintf(pFile, "@@@@@@@@@@@@@@ %d / %d\n", i, meshContainer->dwNumAttributeGroups);
				for (_int paletteEntry = 0; paletteEntry < (_int)meshContainer->dwNumPaletteEntries; ++paletteEntry)
				{
					_int matrixIndex = boneComb[i].BoneId[paletteEntry];

					if (UINT_MAX != matrixIndex)
					{
						
						D3DXMatrixMultiply(&matTemp, &meshContainer->pOffsetMatrices[matrixIndex], meshContainer->ppCombinedTransformationMatrices[matrixIndex]);
						meshContainer->pRenderMatrices[paletteEntry] = matTemp;

						//fprintf(pFile, "####### %d \n", matrixIndex);
						//fprintf(pFile, "%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f\n", matTemp._11, matTemp._12, matTemp._13, matTemp._14, matTemp._21, matTemp._22, matTemp._23, matTemp._24, matTemp._31, matTemp._32, matTemp._33, matTemp._34, matTemp._41, matTemp._42, matTemp._43, matTemp._44);
						/*fprintf(pFile, "%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f\n", 
							meshContainer->pOffsetMatrices[matrixIndex]._11, meshContainer->pOffsetMatrices[matrixIndex]._12, meshContainer->pOffsetMatrices[matrixIndex]._13, meshContainer->pOffsetMatrices[matrixIndex]._14,
							meshContainer->pOffsetMatrices[matrixIndex]._21, meshContainer->pOffsetMatrices[matrixIndex]._22, meshContainer->pOffsetMatrices[matrixIndex]._23, meshContainer->pOffsetMatrices[matrixIndex]._24,
							meshContainer->pOffsetMatrices[matrixIndex]._31, meshContainer->pOffsetMatrices[matrixIndex]._32, meshContainer->pOffsetMatrices[matrixIndex]._33, meshContainer->pOffsetMatrices[matrixIndex]._34,
							meshContainer->pOffsetMatrices[matrixIndex]._41, meshContainer->pOffsetMatrices[matrixIndex]._42, meshContainer->pOffsetMatrices[matrixIndex]._43, meshContainer->pOffsetMatrices[matrixIndex]._44);*/
						/*fprintf(pFile, "%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f\n", 
							meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_11, meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_12, meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_13, meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_14,
							meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_21, meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_22, meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_23, meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_24,
							meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_31, meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_32, meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_33, meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_34,
							meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_41, meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_42, meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_43, meshContainer->ppCombinedTransformationMatrices[matrixIndex]->_44);*/
					}
				}
				_ulong temp = meshContainer->dwNumInfl - 1;

				shader->Get_EffectHandle()->SetMatrixArray("gMatrixPalette", meshContainer->pRenderMatrices, meshContainer->dwNumPaletteEntries);
				
				shader->Get_EffectHandle()->SetValue("gNumBoneInfluences", &temp, sizeof(_ulong));
				shader->SetTexture("gDiffuseTexture", meshContainer->pMeshTexture[boneComb[i].AttribId]);
				//shader->CommitChanges();
				
				//fprintf(pFile, "=========== %d \n", meshContainer->dwNumPaletteEntries);
				//fprintf(pFile, "----------- %d \n", temp);

				shader->BeginShader(nullptr);
				//shader->Get_EffectHandle()->Begin(nullptr, D3DXFX_DONOTSAVESTATE);
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

DynamicMesh_Object * DynamicMesh_Object::Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName)
{
	DynamicMesh_Object*	pInstance = new DynamicMesh_Object();
	if (false == pInstance->Initialize(graphicDevice, filePath, fileName))
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

