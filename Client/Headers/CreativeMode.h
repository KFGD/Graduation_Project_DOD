#pragma once
#include "Base.h"
#include "Mode.h"

#include "Defines.h"
#include "Client_Defines.h"

class KObject;

class DynamicMeshRenderer_Object;
class StaticMeshRenderer_Object;
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
	virtual void	Active()									override;
	virtual void	InActive()									override;
	virtual void	Update_UI(const _double timeDelta)			override;
	virtual void	Update_Object(const _double timeDelta)		override;
	virtual void	Render(LPDIRECT3DDEVICE9 graphicDevice)		override;

public:
	const vector<KObject*>&	GetObjectList() const { return mObjectList; }
	NaviMeshData*	GetNaviMeshData() const { return mNaviMeshData; }

private:
	_bool	PickingObject(_int& selectedIndex, _vec3& hitWorldPos);

private:
	//	Common
	void	UpdateSelectTargetMode();
	void	UpdateFileUI();

private:
	//	Object

	//	UI
	void	UpdateDisplayObjectListUI();
	void	UpdateCreateUI();

	//	Mapping
	void	MappingObjectToObjectUI(const _int objectIndex);
	void	MappingEditUIToEditObject(const _int objectIndex);
	
	//	Logic
	void	LogicObject();

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

	//	Logic
	void	LogicNaviMeshCreateMode();
	void	LogicNaviMeshEditMode();

private:
	_bool	Initialize(LPDIRECT3DDEVICE9 graphicDevice);
	void	InitSampleData();
	void	RenderSkinnedMesh(Shader* shader, KObject*& object, DynamicMeshRenderer_Object* dynamicMesh);
	void	RenderInstanceMesh(LPDIRECT3DDEVICE9 graphicDevice, vector<KObject*>& objectList, StaticMeshRenderer_Object* staticMesh);
	void	RenderHardwareInstancing(LPDIRECT3DDEVICE9 graphicDevice, StaticMeshRenderer_Object* staticMesh, _int numBlock, Shader* shader);
	
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
	_int				mMultiCreateCountX = 0;
	_int				mMultiCreateCountY = 0;
	_int				mMultiCreateCountZ = 0;
	
private:
	//	NaviMeshData
	NaviMeshData*		mNaviMeshData = nullptr;
	_bool				mIsNaviMeshCreate;
	_int				mSelectedNaviPointIndex = 0;
	_vec3				mSelectedNaviPointPosition;
	_int				mSelectedNaviCellIndex = 0;

private:
	Shader*				mInstancingShader = nullptr;
	Shader*				mMeshShader = nullptr;
	Shader*				mSelectedMeshShader = nullptr;

	DynamicMeshRenderer_Object*	mPlayerMesh = nullptr;
	DynamicMeshRenderer_Object*	mBotMesh = nullptr;
	StaticMeshRenderer_Object*	mBlockMesh = nullptr;

private:
	static constexpr _uint			mBlockRenderBatchSize = 500;
	LPDIRECT3DVERTEXBUFFER9			mVertexBuffer;
	
	LPDIRECT3DVERTEXDECLARATION9	mVertexDeclaration = nullptr;

public:
	static CreativeMode*	Create(LPDIRECT3DDEVICE9 graphicDevice);
	virtual void	Free() override;
};


