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
	//for (_int i = 0; i < NaviMesh::CELL_NEIGHBOR_END; ++i)
	//	if (0 > D3DXVec3Dot(&_vec3(nextPosition - mMidPoints[i]), &mCircumNormalXZ[i]))
	//	{
	//		neighborIndex = (NaviMesh::CellNeighbor)i;
	//		return false;
	//	}

	const _vec2 positionXZ(nextPosition.x, nextPosition.z);
	for (_int i = 0; i < NaviMesh::CELL_NEIGHBOR_END; ++i)
	{
		const _vec2 midPointXZ(mMidPoints[i].x, mMidPoints[i].z);
		if (0 > D3DXVec2Dot(&_vec2(positionXZ - midPointXZ), &mCircumNormalXZ[i]))
		{
			neighborIndex = (NaviMesh::CellNeighbor)i;
			return false;
		}

	}

	// 평면의 방정식: a(x - x1) + b(y - y1) + c(z - z1) = 0
	//	b(y - y1) = -a(x - x1) - c(z - z1)
	//	by = -a(x - x1) + by1 - c(z - z1)
	//	y = (-a(x - x1) + by1 - c(z - z1)) / b
	fixPosition = nextPosition;
	fixPosition.y = (-1.f * mNormalVector.x * (fixPosition.x - mCircumCenter.x) + mNormalVector.y * mCircumCenter.y - 1.f * (fixPosition.z - mCircumCenter.z)) / mNormalVector.y;

	return true;
}

_bool NaviCell_Object::IsInCell(const _vec3 & position) const
{
	const _vec2 positionXZ(position.x, position.z);
	for (_int i = 0; i < NaviMesh::CELL_NEIGHBOR_END; ++i)
	{
		const _vec2 midPointXZ(mMidPoints[i].x, mMidPoints[i].z);
		if (0 > D3DXVec2Dot(&_vec2(positionXZ - midPointXZ), &mCircumNormalXZ[i]))
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
		return true;

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
	CalculateCircumCenter();
	CalculateCircumNormal();
	return true;
}

void NaviCell_Object::CalculateTriangleInfo()
{
	//	NormalVector
	D3DXVec3Cross(&mNormalVector, &_vec3(mPoints[NaviMesh::POINT_B] - mPoints[NaviMesh::POINT_A]), &_vec3(mPoints[NaviMesh::POINT_C] - mPoints[NaviMesh::POINT_A]));
	
	//	MidPoints
	mMidPoints[NaviMesh::CELL_NEIGHBOR_AB] = (mPoints[NaviMesh::POINT_A] + mPoints[NaviMesh::POINT_B]) * 0.5f;
	mMidPoints[NaviMesh::CELL_NEIGHBOR_BC] = (mPoints[NaviMesh::POINT_B] + mPoints[NaviMesh::POINT_C]) * 0.5f;
	mMidPoints[NaviMesh::CELL_NEIGHBOR_CA] = (mPoints[NaviMesh::POINT_C] + mPoints[NaviMesh::POINT_A]) * 0.5f;
}

void NaviCell_Object::CalculateCircumCenter()
{	
	//	Cartesian coordinates from cross- and dot-products
	//	Wiki: https://en.wikipedia.org/wiki/Circumscribed_circle#Cartesian_coordinates_2
	_vec3 crossVector = *D3DXVec3Cross(&crossVector, &_vec3(mPoints[NaviMesh::POINT_A] - mPoints[NaviMesh::POINT_B]), &_vec3(mPoints[NaviMesh::POINT_B] - mPoints[NaviMesh::POINT_C]));
	const _float denominator = 2.f * powf(D3DXVec3Length(&crossVector), 2.f);

	const _float a_numerator = powf(D3DXVec3Length(&_vec3(mPoints[NaviMesh::POINT_B] - mPoints[NaviMesh::POINT_C])), 2.f) * D3DXVec3Dot(&_vec3(mPoints[NaviMesh::POINT_A] - mPoints[NaviMesh::POINT_B]), &_vec3(mPoints[NaviMesh::POINT_A] - mPoints[NaviMesh::POINT_C]));
	const _float b_numerator = powf(D3DXVec3Length(&_vec3(mPoints[NaviMesh::POINT_A] - mPoints[NaviMesh::POINT_C])), 2.f) * D3DXVec3Dot(&_vec3(mPoints[NaviMesh::POINT_B] - mPoints[NaviMesh::POINT_A]), &_vec3(mPoints[NaviMesh::POINT_B] - mPoints[NaviMesh::POINT_C]));
	const _float c_numerator = powf(D3DXVec3Length(&_vec3(mPoints[NaviMesh::POINT_A] - mPoints[NaviMesh::POINT_B])), 2.f) * D3DXVec3Dot(&_vec3(mPoints[NaviMesh::POINT_C] - mPoints[NaviMesh::POINT_A]), &_vec3(mPoints[NaviMesh::POINT_C] - mPoints[NaviMesh::POINT_B]));

	const _float a = a_numerator / denominator;
	const _float b = b_numerator / denominator;
	const _float c = c_numerator / denominator;

	mCircumCenter = a * mPoints[NaviMesh::POINT_A] + b * mPoints[NaviMesh::POINT_B] + c * mPoints[NaviMesh::POINT_C];
}

void NaviCell_Object::CalculateCircumNormal()
{
	const _vec2 circumCenterXZ(mCircumCenter.x, mCircumCenter.z);
	for (_int i = 0; i < NaviMesh::CELL_NEIGHBOR_END; ++i)
	{
		const _vec2 midPointsXZ(mMidPoints[i].x, mMidPoints[i].z);
		D3DXVec2Normalize(&mCircumNormalXZ[i], &_vec2(circumCenterXZ - midPointsXZ));
	}
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
