#include "stdafx.h"
#include "DynamicMesh.h"

#include "HierarchyLoader.h"
#include "AnimationCtrl.h"

DynamicMesh::DynamicMesh()
{
}

AnimationCtrl * DynamicMesh::CloneAnimationCtrl()
{
	return mAnimationCtrl->Clone();
}

const vector<D3DXMESHCONTAINER_DERIVED*> DynamicMesh::GetMeshContainerList()
{
	return mMeshContainerList;
}

_bool DynamicMesh::UpdateCombinedTransformationMatrices()
{
	return UpdateCombinedTransformationMatrices(mRootFrame, mPivotMatrix);
}

_bool DynamicMesh::Initialize(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix & pivotMatrix)
{
	_tchar fullPath[MAX_PATH] = L"";

	lstrcpy(fullPath, filePath);
	lstrcat(fullPath, fileName);

	mHierarchyLoader = HierarchyLoader::Create(graphicDevice, filePath);
	if (nullptr == mHierarchyLoader)
		return false;

	//	사전 조사: Frame 개수, MeshContainer 개수
	D3DXLoadMeshHierarchyFromX(fullPath, D3DXMESH_MANAGED, graphicDevice, mHierarchyLoader, nullptr, &mRootFrame, nullptr);
	mHierarchyLoader->DestroyFrame(mRootFrame);
	mRootFrame = nullptr;

	//	Skinned Mesh 생성
	mHierarchyLoader->SetMeasured(true);

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

	return true;
}

_bool DynamicMesh::SetUpCombinedTransformationMatricesPointer(D3DXFRAME * frame)
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

_bool DynamicMesh::UpdateCombinedTransformationMatrices(D3DXFRAME * frame, const _matrix & parentMatrix)
{
	D3DXFRAME_DERIVED*	pFrame_Derived = (D3DXFRAME_DERIVED*)frame;

	pFrame_Derived->CombinedTransformationMatrix = pFrame_Derived->TransformationMatrix * parentMatrix;

	if (nullptr != pFrame_Derived->pFrameSibling)
		UpdateCombinedTransformationMatrices(pFrame_Derived->pFrameSibling, parentMatrix);

	if (nullptr != pFrame_Derived->pFrameFirstChild)
		UpdateCombinedTransformationMatrices(pFrame_Derived->pFrameFirstChild, pFrame_Derived->CombinedTransformationMatrix);

	return true;
}

_bool DynamicMesh::UpdateSoftwareSkinnedMesh(const _int meshContainerIndex)
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

DynamicMesh * DynamicMesh::Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix & pivotMatrix)
{
	DynamicMesh*	pInstance = new DynamicMesh();
	if (false == pInstance->Initialize(graphicDevice, filePath, fileName, pivotMatrix))
	{
		MSG_BOX("Failed To Create DynamicMesh Instance");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void DynamicMesh::Free()
{
	mHierarchyLoader->DestroyFrame(mRootFrame);
	SafeRelease(mAnimationCtrl);
	SafeRelease(mHierarchyLoader);
}

