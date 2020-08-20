#pragma once
#include "World.h"

class PipeLine;
class ComponentManager_Object;

class KObject;
class GameObject;
class Player;
class Bot;
class BlockObj;

class StaticMesh_Object;
class Shader;


class World_Object :
	public World
{
private:
	explicit World_Object(const LPDIRECT3DDEVICE9 graphicDev);
	explicit World_Object(const World& rhs) = delete;
	virtual ~World_Object() = default;

public:
	virtual void	Update(const _double timeDelta) override;
	virtual void	Render() override;



	virtual _bool	SetUpObjectList(const vector<KObject*>& objectList) override;
	virtual _bool	Clear() override;
	
private:
	_bool	Initialize();
	_bool	ReadyComponent();
	void	RenderBlock();
	void	RenderHardwareInstancing(StaticMesh_Object* staticMesh, _int numBlock, Shader* shader);

private:
	PipeLine*	mPipeLine = nullptr;
	ComponentManager_Object*	mComponentManager = nullptr;

private:
	static constexpr _uint			mBlockRenderBatchSize = 100;
	LPDIRECT3DVERTEXBUFFER9			mVertexBuffer = nullptr;
	LPDIRECT3DVERTEXDECLARATION9	mVertexDeclaration = nullptr;

private:
	vector<Player*>		mPlayerList;
	vector<Bot*>		mBotList;
	vector<BlockObj*>	mBlockList;

public:
	virtual void	Free() override;
	static World_Object*	Create(const LPDIRECT3DDEVICE9 graphicDev);
};

