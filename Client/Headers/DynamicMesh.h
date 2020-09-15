#pragma once
#include "Base.h"

#include "Defines.h"

class HierarchyLoader;
class AnimationCtrl;

class DynamicMesh :
	public Base
{
private:
	explicit DynamicMesh();
	explicit DynamicMesh(const DynamicMesh& rhs) = delete;
	virtual ~DynamicMesh() = default;

public:
	AnimationCtrl*								CloneAnimationCtrl();
	const vector<D3DXMESHCONTAINER_DERIVED*>	GetMeshContainerList();
	_bool										UpdateCombinedTransformationMatrices();

private:
	_bool	Initialize(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix& pivotMatrix);
	_bool	SetUpCombinedTransformationMatricesPointer(D3DXFRAME* frame);
	_bool	UpdateCombinedTransformationMatrices(D3DXFRAME* frame, const _matrix& parentMatrix);
	_bool	UpdateSoftwareSkinnedMesh(const _int meshContainerIndex);

private:
	LPD3DXFRAME			mRootFrame = nullptr;
	_matrix				mPivotMatrix;
	HierarchyLoader*	mHierarchyLoader = nullptr;
	AnimationCtrl*		mAnimationCtrl = nullptr;

private:
	vector<D3DXMESHCONTAINER_DERIVED*>	mMeshContainerList;	// 삭제예정

public:
	static DynamicMesh*	Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix& pivotMatrix);
	virtual void	Free()	override;
};

