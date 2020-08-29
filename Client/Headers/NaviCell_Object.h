#pragma once
#include "Base.h"

#include "Defines.h"
#include "Client_Defines.h"

class NaviCell_Object :
	public Base
{

private:
	explicit NaviCell_Object(const _int index, const _vec3& pointA, const _vec3& pointB, const _vec3& pointC);
	virtual ~NaviCell_Object() = default;

public:
	_bool	Search(const _vec3& nextPosition, _vec3& fixPosition, NaviMesh::CellNeighbor& neighborIndex) const;
	_bool	IsInCell(const _vec3& position) const;

public:
	_bool	LinkNeighborCell(const NaviMesh::CellNeighbor myIndex, NaviCell_Object* neightborCell, const NaviMesh::CellNeighbor neighborIndex);
	void	GetPointPositionOfLine(const NaviMesh::CellNeighbor lineIndex, _vec3& point0, _vec3& point1);
	NaviCell_Object*	GetNeighborCell(const NaviMesh::CellNeighbor neighborIndex) { return mNaviCellNeighbors[neighborIndex]; }

private:
	_bool	Initialize();
	void	CalculateTriangleInfo();
	void	CalculateCircumCenter();
	void	CalculateCircumNormal();

private:
	_int	mIndex = 0;
	array<_vec3, NaviMesh::POINT_END>	mPoints;
	array<NaviCell_Object*, NaviMesh::CELL_NEIGHBOR_END>	mNaviCellNeighbors;

private:
	//	Triangle
	_vec3	mNormalVector;
	array<_vec3, NaviMesh::CELL_NEIGHBOR_END>	mMidPoints;
	
	//	Circumscribed
	_vec3		mCircumCenter;
	array<_vec2, NaviMesh::CELL_NEIGHBOR_END>	mCircumNormalXZ;

public:
	static NaviCell_Object*	Create(const _int index, const _vec3& pointA, const _vec3& pointB, const _vec3& pointC);
	virtual void	Free()	override;

};

