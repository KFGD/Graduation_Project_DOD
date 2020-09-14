#pragma once
#include "ComponentSystem.h"

class PipeLine;
class Shader;
class StaticMesh;

class StaticRendererSystem :
	public ComponentSystem
{
	DECLARE_SINGLETON(StaticRendererSystem)

public:
	struct Component
	{
		_matrix		WorldMatrix;

		Component(const _matrix& worldMatrix = IDENTITY_MATRIX)
			: WorldMatrix(WorldMatrix)
		{}
	};

private:
	explicit StaticRendererSystem();
	virtual ~StaticRendererSystem() = default;

public:
	_bool	ReadyResources(LPDIRECT3DDEVICE9 graphicDevice);
	_bool	ReadySystem(const _size_t entitySize, const _size_t componentSize);
	void	ClearResources();
	void	ClearSystem();

public:
	virtual void	Update(const _double timeDelta)		override;
	virtual void	LateUpdate(const _double timeDelta)	override;
	void			Render(LPDIRECT3DDEVICE9 graphicDevice);

public:
	_bool	AttachComponent(const _uniqueId entityId, const char* meshName);
	_bool	AddStaticMesh(const char* meshName, StaticMesh* staticMesh);
	void	SetWorldMatrix(const _uniqueId entityId, const _matrix& worldMatrix);

private:
	void	RenderHardwareInstancing(LPDIRECT3DDEVICE9 graphicDevice, StaticMesh* staticMesh, const _int objectNum);

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
	unordered_map<_uint, StaticMesh*>	mStaticMeshMap;
	Shader*		mShader = nullptr;

private:
	PipeLine*	mPipeLine = nullptr;

private:
	static constexpr _uint			mBlockRenderBatchSize = 100;
	LPDIRECT3DVERTEXBUFFER9			mVertexBuffer = nullptr;
	LPDIRECT3DVERTEXDECLARATION9	mVertexDeclaration = nullptr;

public:
	virtual void Free() override;
};

