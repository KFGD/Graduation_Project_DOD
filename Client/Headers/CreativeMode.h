#pragma once
#include "Base.h"
#include "Mode.h"

#include "Defines.h"
#include "Client_Defines.h"

class KObject;

class DynamicMesh_Object;
class StaticMesh_Object;
class Shader;

class CreativeMode :
	public Mode
{

private:
	explicit CreativeMode();
	virtual ~CreativeMode() = default;
	
public:
	virtual void	Active(IWorldController* worldController)	override;
	virtual void	InActive(IWorldController* worldController)	override;
	virtual void	Update(IWorldController* worldController)	override;
	virtual void	Render(LPDIRECT3DDEVICE9 graphicDevice)		override;

private:
	_bool	PickingObject();

private:
	void	UpdateDisplayObjectListUI(IWorldController* worldController);
	void	UpdateCreateUI();

private:
	void	ReloadWorld(IWorldController* worldController);

	//	Edit
	void	MappingObjectToUI(const _int objectIndex);
	void	MappingUIToObject(const _int objectIndex);
	
	void	ClearObjectList();
	
private:
	_bool	Initialize(LPDIRECT3DDEVICE9 graphicDevice);
	void	InitSampleData();
	void	RenderSkinnedMesh(Shader* shader, KObject*& object, DynamicMesh_Object* dynamicMesh);
	void	RenderInstanceMesh(LPDIRECT3DDEVICE9 graphicDevice, vector<KObject*>& objectList, StaticMesh_Object* staticMesh);
	void	RenderHardwareInstancing(LPDIRECT3DDEVICE9 graphicDevice, StaticMesh_Object* staticMesh, _int numBlock, Shader* shader);
	
private:
	vector<KObject*>	mObjectList;

private:
	//	Display Object List
	_int	mSelectedObjectListIndex;
	//array<_bool, Game::Type_End>	mbDisplayObjectFilter;
	//vector<pair<_int, KObject*>>	mDisplayObjectList;

	//typedef pair<_int, KObject*>	DISPLAY_PAIR;

	_vec3				mEditScale;
	_vec3				mEditRotation;
	_vec3				mEditPosition;
	Game::ObjectType	mEditObjectType = Game::Player;

private:
	//	Create
	_vec3				mCreateScale;
	_vec3				mCreateRotation;
	_vec3				mCreatePos;
	Game::ObjectType	mCreateObjectType = Game::Player;
	
private:
	Shader*				mInstancingShader = nullptr;
	Shader*				mMeshShader = nullptr;
	Shader*				mSelectedMeshShader = nullptr;

	DynamicMesh_Object*	mPlayerMesh = nullptr;
	DynamicMesh_Object*	mBotMesh = nullptr;
	StaticMesh_Object*	mBlockMesh = nullptr;

private:
	static constexpr _uint			mBlockRenderBatchSize = 100;
	LPDIRECT3DVERTEXBUFFER9			mVertexBuffer = nullptr;
	LPDIRECT3DVERTEXDECLARATION9	mVertexDeclaration = nullptr;

public:
	static CreativeMode*	Create(LPDIRECT3DDEVICE9 graphicDevice);
	virtual void	Free() override;
};

