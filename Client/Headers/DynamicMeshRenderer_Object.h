#pragma once
#include "Component_Object.h"

class HierarchyLoader;
class AnimationCtrl;
class Shader;

class DynamicMeshRenderer_Object :
	public Component_Object
{
private:
	explicit DynamicMeshRenderer_Object();
	explicit DynamicMeshRenderer_Object(const DynamicMeshRenderer_Object& rhs);
	virtual ~DynamicMeshRenderer_Object() = default;	

public:
	_bool	UpdateHardwareSkinnedMesh(const _int meshContainerIndex);
	_bool	SetUpAnimation(const _uint index);
	_bool	PlayAnimation(const _double timeDelta);
	void	Render(Shader* shader, const _int meshContainerIndex);

public:

	D3DXMESHCONTAINER_DERIVED*	GetMeshContainer(const _size_t index) { return mMeshContainerList[index]; }
	_size_t						GetMeshContinerSize() const { return mMeshContainerList.size(); }

private:
	_bool	Initialize(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix& pivotMatrix);
	_bool	UpdateCombinedTransformationMatrices(D3DXFRAME* frame, const _matrix& parentMatrix);
	_bool	SetUpCombinedTransformationMatricesPointer(D3DXFRAME* frame);
	_bool	UpdateSoftwareSkinnedMesh(const _int meshContainerIndex);

private:
	void	RenderHardwareSkinningRecursive(Shader* shader, const _matrix& worldMatrix, D3DXFRAME* bone);

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
	static DynamicMeshRenderer_Object*	Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix& pivotMatrix);
	Component_Object*	Clone(void* arg) override;
	virtual void	Free()	override;
};

