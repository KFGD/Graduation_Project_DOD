#include "stdafx.h"
#include "NaviCell_Object.h"

NaviCell_Object::NaviCell_Object(const _int index, const _vec3 & pointA, const _vec3 & pointB, const _vec3 & pointC)
	: mIndex(index)
	, mPoints{pointA, pointB, pointC}
	, mNaviCellNeighbors{nullptr, nullptr, nullptr}
{
}

_bool NaviCell_Object::Search(const _vec3 & nextPosition, _vec3& fixPosition, NaviMesh::CellNeighbor & neighborIndex) const
{
	const _vec2 positionXZ(nextPosition.x, nextPosition.z);
	for (_int i = 0; i < NaviMesh::CELL_NEIGHBOR_END; ++i)
	{
		const _vec2 midPointXZ(mMidPoints[i].x, mMidPoints[i].z);
		_vec2 dirXZ(positionXZ - midPointXZ);
		D3DXVec2Normalize(&dirXZ, &dirXZ);
		if (0 > D3DXVec2Dot(&dirXZ, &mLineNormalXZ[i]))
		{
			neighborIndex = (NaviMesh::CellNeighbor)i;
			return false;
		}

	}

	fixPosition = nextPosition;
	//	����� ������: aX + bY +cZ + D = 0
	//	Y = (aX + cZ + D) / b * -1.f
	const _float distance = -1.f * (mNormalVector.x * mInscribedCenter.x + mNormalVector.y * mInscribedCenter.y + mNormalVector.z * mInscribedCenter.z);
	fixPosition.y = (mNormalVector.x * fixPosition.x + mNormalVector.z * fixPosition.z + distance) / mNormalVector.y * -1.f;

	return true;
}

_bool NaviCell_Object::IsInCell(const _vec3 & position) const
{
	const _vec2 positionXZ(position.x, position.z);
	for (_int i = 0; i < NaviMesh::CELL_NEIGHBOR_END; ++i)
	{
		_vec2 midPointXZ(mMidPoints[i].x, mMidPoints[i].z);
		_vec2 dirXZ(positionXZ - midPointXZ);
		D3DXVec2Normalize(&dirXZ, &dirXZ);
		if (0 > D3DXVec2Dot(&dirXZ, &mLineNormalXZ[i]))
			return false;
	
	}
	return true;
}

_bool NaviCell_Object::LinkNeighborCell(const NaviMesh::CellNeighbor myIndex, NaviCell_Object* neightborCell, const NaviMesh::CellNeighbor neighborIndex)
{
	_vec3 myPoint[2];
	_vec3 neighborPoint[2];

	::ZeroMemory(myPoint, sizeof(myPoint));
	::ZeroMemory(neighborPoint, sizeof(neighborPoint));

	GetPointPositionOfLine(myIndex, myPoint[0], myPoint[1]);
	neightborCell->GetPointPositionOfLine(neighborIndex, neighborPoint[0], neighborPoint[1]);

	if ((myPoint[0] == neighborPoint[0] && myPoint[1] == neighborPoint[1]) ||
		(myPoint[0] == neighborPoint[1] && myPoint[1] == neighborPoint[0]))
	{
		mNaviCellNeighbors[myIndex] = neightborCell;
		return true;
	}

	return false;
}

void NaviCell_Object::GetPointPositionOfLine(const NaviMesh::CellNeighbor lineIndex, _vec3 & point0, _vec3 & point1)
{
	switch (lineIndex)
	{
	case NaviMesh::CELL_NEIGHBOR_AB:
		point0 = mPoints[NaviMesh::POINT_A];
		point1 = mPoints[NaviMesh::POINT_B];
		break;

	case NaviMesh::CELL_NEIGHBOR_BC:
		point0 = mPoints[NaviMesh::POINT_B];
		point1 = mPoints[NaviMesh::POINT_C];
		break;

	case NaviMesh::CELL_NEIGHBOR_CA:
		point0 = mPoints[NaviMesh::POINT_C];
		point1 = mPoints[NaviMesh::POINT_A];
		break;

	default:
		break;
	}
}

_bool NaviCell_Object::Initialize()
{
	CalculateTriangleInfo();
	CalculateInscribedCenter();
	return true;
}

void NaviCell_Object::CalculateTriangleInfo()
{
	//	NormalVector
	D3DXVec3Cross(&mNormalVector, &_vec3(mPoints[NaviMesh::POINT_B] - mPoints[NaviMesh::POINT_A]), &_vec3(mPoints[NaviMesh::POINT_C] - mPoints[NaviMesh::POINT_A]));
	D3DXVec3Normalize(&mNormalVector, &mNormalVector);

	//	MidPoints
	mMidPoints[NaviMesh::CELL_NEIGHBOR_AB] = (mPoints[NaviMesh::POINT_A] + mPoints[NaviMesh::POINT_B]) * 0.5f;
	mMidPoints[NaviMesh::CELL_NEIGHBOR_BC] = (mPoints[NaviMesh::POINT_B] + mPoints[NaviMesh::POINT_C]) * 0.5f;
	mMidPoints[NaviMesh::CELL_NEIGHBOR_CA] = (mPoints[NaviMesh::POINT_C] + mPoints[NaviMesh::POINT_A]) * 0.5f;

	//	LineNormal
	const _vec3 lineAB = mPoints[NaviMesh::POINT_B] - mPoints[NaviMesh::POINT_A];
	D3DXVec2Normalize(&mLineNormalXZ[NaviMesh::CELL_NEIGHBOR_AB], &_vec2(lineAB.z, lineAB.x * -1.f));

	const _vec3 lineBC = mPoints[NaviMesh::POINT_C] - mPoints[NaviMesh::POINT_B];
	D3DXVec2Normalize(&mLineNormalXZ[NaviMesh::CELL_NEIGHBOR_BC], &_vec2(lineBC.z, lineBC.x * -1.f));

	const _vec3 lineCA = mPoints[NaviMesh::POINT_A] - mPoints[NaviMesh::POINT_C];
	D3DXVec2Normalize(&mLineNormalXZ[NaviMesh::CELL_NEIGHBOR_CA], &_vec2(lineCA.z, lineCA.x * -1.f));

}

void NaviCell_Object::CalculateInscribedCenter()
{	
	//	Cartesian coordinates from cross- and dot-products
	//	Wiki: https://en.wikipedia.org/wiki/Circumscribed_circle#Cartesian_coordinates_2
	_vec3 crossVector = *D3DXVec3Cross(&crossVector, &_vec3(mPoints[NaviMesh::POINT_A] - mPoints[NaviMesh::POINT_B]), &_vec3(mPoints[NaviMesh::POINT_B] - mPoints[NaviMesh::POINT_C]));

	const _float lengthAB = D3DXVec3Length(&_vec3(mPoints[NaviMesh::POINT_A] - mPoints[NaviMesh::POINT_B]));
	const _float lengthBC = D3DXVec3Length(&_vec3(mPoints[NaviMesh::POINT_B] - mPoints[NaviMesh::POINT_C]));
	const _float lengthCA = D3DXVec3Length(&_vec3(mPoints[NaviMesh::POINT_C] - mPoints[NaviMesh::POINT_A]));

	const _vec3		denominator = lengthAB * mPoints[NaviMesh::POINT_C] + lengthBC * mPoints[NaviMesh::POINT_A] + lengthCA * mPoints[NaviMesh::POINT_B];
	const _float	numerator = lengthAB + lengthBC + lengthCA;

	mInscribedCenter = denominator / numerator;
}

NaviCell_Object * NaviCell_Object::Create(const _int index, const _vec3 & pointA, const _vec3 & pointB, const _vec3 & pointC)
{
	NaviCell_Object*	pInstance = new NaviCell_Object(index, pointA, pointB, pointC);

	if (false == pInstance->Initialize())
	{
		MSG_BOX("NaviCell_Object Created Failed!");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void NaviCell_Object::Free()
{
}
