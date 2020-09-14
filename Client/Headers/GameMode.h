#pragma once
#include "Base.h"
#include "Mode.h"

#include "Defines.h"
#include "Client_Defines.h"

class KObject;
class World;
class NaviMeshData;

class GameMode :
	public Mode
{
	typedef Game::ObjectType Type;

private:
	explicit GameMode();
	virtual ~GameMode() = default;

public:
	virtual void	Active()									override;
	virtual void	InActive()									override;
	virtual void	Update_UI(const _double timeDelta)			override;
	virtual void	Update_Object(const _double timeDelta)		override;
	virtual void	Render(LPDIRECT3DDEVICE9 graphicDevice)		override;

public:
	void	SetObjectList(const vector<KObject*>& objectList);
	void	SetNaviMeshData(const NaviMeshData* naviMeshData);

private:
	_bool	Initialize(LPDIRECT3DDEVICE9 graphicDevice);
	void	ClearObjectList();

private:
	vector<KObject*>	mObjectList;
	array<World*, Game::WorldEnd>	mWorlds;

private:
	Game::WorldType		mCurWorldType;


public:
	static GameMode*	Create(LPDIRECT3DDEVICE9 graphicDevice);
	virtual void	Free() override;

};

