#pragma once
#include "World.h"

class EntityManager;
class TransformSystem;
class StaticRendererSystem;
class DynamicRendererSystem;

class World_Data :
	public World
{
private:
	explicit World_Data(const LPDIRECT3DDEVICE9 graphicDevice);
	explicit World_Data(const World_Data& rhs) = delete;
	virtual ~World_Data() = default;

public:
	virtual void Update(const _double timeDelta) override;
	virtual void Render() override;
	virtual _bool SetUpObjectList(const vector<KObject*>& objectList) override;
	virtual _bool SetUpNaviMesh(const NaviMeshData * naviMeshData) override;
	virtual _bool Clear() override;

private:
	_bool	Initialize();
	_bool	ReadyResources();
	
private:
	EntityManager*			mEntityManager = nullptr;
	TransformSystem*		mTransformSystem = nullptr;
	StaticRendererSystem*	mStaticRendererSystem = nullptr;
	DynamicRendererSystem*	mDynamicRendererSystem = nullptr;

public:
	virtual void	Free()	override;
	static World_Data*	Create(const LPDIRECT3DDEVICE9 graphicDevice);
};

