#pragma once
#include "Base.h"
#include "Mode.h"

#include "Defines.h"
#include "Client_Defines.h"

class KObject;

class DynamicMesh_Object;
class StaticMesh_Object;
class Shader;
class NaviMeshData;

class CreativeMode :
	public Mode
{
	enum TargetMode
	{
		OBJECT_MODE,
		NAVI_MODE
	};
private:
	explicit CreativeMode();
	virtual ~CreativeMode() = default;
	
public:
	virtual void	Active(IWorldController* worldController)	override;
	virtual void	InActive(IWorldController* worldController)	override;
	virtual void	Update(IWorldController* worldController)	override;
	virtual void	Render(LPDIRECT3DDEVICE9 graphicDevice)		override;

private:
	_bool	PickingObject(_int& selectedIndex, _vec3& hitWorldPos);

private:
	void	UpdateSelectTargetMode();
	void	UpdateFileUI();

private:
	//	Object

	//	UI
	void	UpdateDisplayObjectListUI(IWorldController* worldController);
	void	UpdateCreateUI();

	//	Mapping
	void	MappingObjectToObjectUI(const _int objectIndex);
	void	MappingEditUIToEditObject(const _int objectIndex);
	
	//	Logic

	//	Function
	void	CreateObject();
	void	ClearObjectList();

private:
	//	Navi

	//	UI
	void	UpdateNaviMeshDataUI();

	//	Mapping
	void	MappingNaviMeshDataToNaviMeshUI();
	void	MappingNaviMeshUIToNaviMeshData();
	
private:
	_bool	Initialize(LPDIRECT3DDEVICE9 graphicDevice);
	void	InitSampleData();
	void	RenderSkinnedMesh(Shader* shader, KObject*& object, DynamicMesh_Object* dynamicMesh);
	void	RenderInstanceMesh(LPDIRECT3DDEVICE9 graphicDevice, vector<KObject*>& objectList, StaticMesh_Object* staticMesh);
	void	RenderHardwareInstancing(LPDIRECT3DDEVICE9 graphicDevice, StaticMesh_Object* staticMesh, _int numBlock, Shader* shader);
	
private:
	vector<KObject*>	mObjectList;

private:
	//	TargetMode
	TargetMode	mCurMode;

private:
	//	File Selector
	char mFilePath[MAX_PATH];

private:
	//	Display Object List
	_int	mSelectedObjectListIndex;

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

	_float				mMultiCreateGap = 0.f;
	_int				mMultiCreateCount = 0;
	
private:
	//	NaviMeshData
	NaviMeshData*		mNaviMeshData = nullptr;
	_bool				mIsNaviMeshCreate;
	_int				mSelectedNaviPointIndex = 0;
	_vec3				mSelectedNaviPointPosition;
	_int				mSelectedNaviCellIndex = 0;
	_bool				mIsSelectedNaviCell;

private:
	Shader*				mInstancingShader = nullptr;
	Shader*				mMeshShader = nullptr;
	Shader*				mSelectedMeshShader = nullptr;

	DynamicMesh_Object*	mPlayerMesh = nullptr;
	DynamicMesh_Object*	mBotMesh = nullptr;
	StaticMesh_Object*	mBlockMesh = nullptr;

private:
	static constexpr _uint			mBlockRenderBatchSize = 500;
	LPDIRECT3DVERTEXBUFFER9			mVertexBuffer;
	
	LPDIRECT3DVERTEXDECLARATION9	mVertexDeclaration = nullptr;

public:
	static CreativeMode*	Create(LPDIRECT3DDEVICE9 graphicDevice);
	virtual void	Free() override;
};


