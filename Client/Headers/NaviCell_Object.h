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
	_bool	Move(const _vec3& movePosition, _vec3& nextPosition, NaviMesh::CellNeighbor& neighborIndex) const;
	_bool	Slide(const _vec3 & curPosition, const _vec3 & moveVector, _vec3 & nextPosition) const;
	_bool	IsInCell(const _vec3& position) const;

public:
	_bool				LinkNeighborCell(const NaviMesh::CellNeighbor myIndex, NaviCell_Object* neightborCell, const NaviMesh::CellNeighbor neighborIndex);
	_int				GetCellIndex() const { return mIndex; }
	const _vec3&		GetPointPosition(const NaviMesh::Point pointIndex) const { return mPoints[pointIndex]; }
	void				GetPointPositionOfLine(const NaviMesh::CellNeighbor lineIndex, _vec3& point0, _vec3& point1);
	NaviCell_Object*	GetNeighborCell(const NaviMesh::CellNeighbor neighborIndex) { return mNaviCellNeighbors[neighborIndex]; }
	const _vec3&		GetInscribedCenter() const { return mInscribedCenter; }

private:
	_bool	Initialize();
	void	CalculateTriangleInfo();
	void	CalculateInscribedCenter();

private:
	_int	mIndex = 0;
	array<_vec3, NaviMesh::POINT_END>	mPoints;
	array<NaviCell_Object*, NaviMesh::CELL_NEIGHBOR_END>	mNaviCellNeighbors;

private:
	//	Triangle
	_vec3	mNormalVector;
	array<_vec2, NaviMesh::CELL_NEIGHBOR_END>	mLineNormalXZ;
	array<_vec3, NaviMesh::CELL_NEIGHBOR_END>	mMidPoints;
		
	//	Circumscribed
	_vec3		mInscribedCenter;

public:
	static NaviCell_Object*	Create(const _int index, const _vec3& pointA, const _vec3& pointB, const _vec3& pointC);
	virtual void	Free()	override;

};

