#pragma once
#include "Base.h"

#include "Defines.h"

class NaviMeshData;
class NaviCell_Object;

class NaviMesh_Object :
	public Base
{
private:
	explicit NaviMesh_Object();
	virtual ~NaviMesh_Object() = default;

public:
	_bool	Move(const _int curCellIndex, const _vec3 & nextPosition, _int& nextCellIndex, _vec3& fixPosition);
	_int	FindCellIndex(const _vec3& position) const;

public:
	void	SetNaviMeshData(const NaviMeshData& naviMeshData);

private:
	_bool	Initialize();

private:
	vector<NaviCell_Object*>	mNaviCellList;

public:
	static NaviMesh_Object*	Create(LPDIRECT3DDEVICE9 graphicDevice);
	virtual	void	Free()	override;
};

