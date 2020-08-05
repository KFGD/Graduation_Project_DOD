#pragma once
#include "Component_Object.h"

class HierarchyLoader;
class AnimationCtrl;

class DynamicMesh_Object :
	public Component_Object
{
private:
	explicit DynamicMesh_Object();
	explicit DynamicMesh_Object(const DynamicMesh_Object& rhs);
	virtual ~DynamicMesh_Object() = default;	

public:
	_bool	UpdateSkinnedMesh(const _int meshContainerIndex);
	_bool	SetUpAnimation(const _uint index);
	_bool	PlayerAnimation(const _double timeDelta);
	void	Render(LPDIRECT3DDEVICE9 graphicDevice);

public:
	_size_t	GetMeshContinerSize() const { return mMeshContainerList.size(); }

private:
	_bool	Initialize(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName);
	_bool	UpdateCombinedTransformationMatrices(D3DXFRAME* frame, const _matrix& parentMatrix);
	_bool	SetUpCombinedTransformationMatricesPointer(D3DXFRAME* frame);


private:
	D3DXFRAME*	mRootFrame = nullptr;
	_matrix		mPivotMatrix;
	HierarchyLoader*	mHierarchyLoader = nullptr;
	AnimationCtrl*		mAnimationCtrl = nullptr;

private:
	_bool	mIsClone = false;
	vector<D3DXMESHCONTAINER_DERIVED*>	mMeshContainerList;
	typedef vector<D3DXMESHCONTAINER_DERIVED*>	MESHCONTAINERLIST;
	
public:
	static DynamicMesh_Object*	Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName);
	Component_Object*	Clone(void* arg) override;
	virtual void	Free()	override;
};
