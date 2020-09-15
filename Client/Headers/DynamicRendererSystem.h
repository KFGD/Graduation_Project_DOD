#pragma once
#include "ComponentSystem.h"

class PipeLine;
class Shader;
class DynamicMesh;

class DynamicRendererSystem :
	public ComponentSystem
{
public:
	struct Component
	{
		_matrix		WorldMatrix;

		Component(const _matrix& worldMatrix = IDENTITY_MATRIX)
			: WorldMatrix(WorldMatrix)
		{}
	};

private:
	explicit DynamicRendererSystem();
	virtual ~DynamicRendererSystem() = default;

public:
	virtual void Update(const _double timeDelta) override;
	virtual void LateUpdate(const _double timeDelta) override;

public:
	

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

private:
	//////////////////////
	//
	//	Resources
	//
	//////////////////////
	unordered_map<_uint, DynamicMesh*>	mDynamicMeshMap;
	Shader*		mShader = nullptr;

public:
	virtual void	Free()	override;
};

