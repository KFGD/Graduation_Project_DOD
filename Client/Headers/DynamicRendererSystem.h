#pragma once
#include "ComponentSystem.h"

class PipeLine;
class Shader;
class AnimationCtrl;
class DynamicMesh;

class DynamicRendererSystem :
	public ComponentSystem
{
	DECLARE_SINGLETON(DynamicRendererSystem)
public:

	struct Component
	{
		AnimationCtrl*	Controller;
		_matrix			WorldMatrix;

		Component(AnimationCtrl* animationCtrl = nullptr, _matrix& worldMatrix = IDENTITY_MATRIX)
			: Controller(animationCtrl), WorldMatrix(WorldMatrix)
		{}
	};

private:
	explicit DynamicRendererSystem();
	virtual ~DynamicRendererSystem() = default;

public:
	_bool	ReadyResources(LPDIRECT3DDEVICE9 graphicDevice);
	_bool	ReadySystem(const _size_t entitySize, const _size_t componentSize);
	void	ClearResources();
	void	ClearSystem();

public:
	virtual void	Update(const _double timeDelta) override;
	virtual void	LateUpdate(const _double timeDelta) override;
	void			Render(LPDIRECT3DDEVICE9 graphicDevice);

public:
	_bool	AttachComponent(const _uniqueId entityId, const char* meshName);
	_bool	AddDynamicMesh(const char* meshName, DynamicMesh* dynamicMesh);
	void	SetWorldMatrix(const _uniqueId entityId, const _matrix& worldMatrix);

private:
	//////////////////////
	//
	//	System
	//
	//////////////////////
	_size_t	mEntitySize = 0;
	_size_t	mComponentSize = 0;

	queue<_uniqueId>	mIndexQueue;
	vector<_uniqueId>	mIndexTable;
	vector<Component>	mComponentList;

	unordered_map<_uint, vector<_uniqueId>>	mRenderingInfoMap;	// entityId List
	
	_double		mTimeDelta = 0.0;

private:
	//////////////////////
	//
	//	Resources
	//
	//////////////////////
	unordered_map<_uint, DynamicMesh*>	mDynamicMeshMap;
	Shader*		mShader = nullptr;

private:
	PipeLine*	mPipeLine = nullptr;

public:
	virtual void	Free()	override;
};

