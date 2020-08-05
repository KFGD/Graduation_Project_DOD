#include "stdafx.h"
#include "DynamicMesh_Object.h"

#include "HierarchyLoader.h"
#include "AnimationCtrl.h"

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

_bool DynamicMesh_Object::UpdateSkinnedMesh(const _int meshContainerIndex)
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

_bool DynamicMesh_Object::SetUpAnimation(const _uint index)
{
	if (nullptr == mAnimationCtrl)
		return false;

	mAnimationCtrl->SetUpAnimation(index);
	return true;
}

_bool DynamicMesh_Object::PlayerAnimation(const _double timeDelta)
{
	if (nullptr == mAnimationCtrl)
		return false;

	mAnimationCtrl->PlayAnimation(timeDelta);

	UpdateCombinedTransformationMatrices(mRootFrame, mPivotMatrix);

	return true;
}

void DynamicMesh_Object::Render(LPDIRECT3DDEVICE9 graphicDevice)
{
	for (D3DXMESHCONTAINER_DERIVED* meshContainer : mMeshContainerList)
	{
		for (_uint i = 0; i < meshContainer->NumMaterials; ++i)
		{
			HRESULT result = graphicDevice->SetTexture(0, meshContainer->pMeshTexture[i]);
			meshContainer->MeshData.pMesh->DrawSubset(i);
		}
	}
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

	UpdateCombinedTransformationMatrices(mRootFrame, mPivotMatrix);

	SetUpCombinedTransformationMatricesPointer(mRootFrame);

	return true;
}

_bool DynamicMesh_Object::UpdateCombinedTransformationMatrices(D3DXFRAME* frame, const _matrix& parentMatrix)
{
	D3DXFRAME_DERIVED*	pFrame_Derived = (D3DXFRAME_DERIVED*)frame;

	pFrame_Derived->CombinedTransformationMatrix = pFrame_Derived->TransformationMatrix * parentMatrix;

	if (nullptr != pFrame_Derived->pFrameFirstChild)
		UpdateCombinedTransformationMatrices(pFrame_Derived->pFrameFirstChild, pFrame_Derived->CombinedTransformationMatrix);
		
	if (nullptr != pFrame_Derived->pFrameSibling)
		UpdateCombinedTransformationMatrices(pFrame_Derived->pFrameSibling, parentMatrix);

	return true;
}

_bool DynamicMesh_Object::SetUpCombinedTransformationMatricesPointer(D3DXFRAME * frame)
{
	if (nullptr != frame->pMeshContainer)
	{
		D3DXMESHCONTAINER_DERIVED* pMeshContainer_Derived = (D3DXMESHCONTAINER_DERIVED*)frame->pMeshContainer;

		for (_ulong i = 0; i < pMeshContainer_Derived->dwNumBones; ++i)
		{
			D3DXFRAME_DERIVED*	pFrame_Derived = (D3DXFRAME_DERIVED*)D3DXFrameFind(mRootFrame, pMeshContainer_Derived->pSkinInfo->GetBoneName(i));

			pMeshContainer_Derived->ppCombinedTransformationMatrices[i] = &pFrame_Derived->CombinedTransformationMatrix;
		}

		mMeshContainerList.push_back(pMeshContainer_Derived);
	}

	if (nullptr != frame->pFrameFirstChild)
		SetUpCombinedTransformationMatricesPointer(frame->pFrameFirstChild);

	if (nullptr != frame->pFrameSibling)
		SetUpCombinedTransformationMatricesPointer(frame->pFrameSibling);

	return true;
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

