#include "stdafx.h"
#include "NaviMesh_Object.h"

#include "NaviMeshData.h"
#include "NaviCell_Object.h"

NaviMesh_Object::NaviMesh_Object()
{
}

_bool NaviMesh_Object::Move(const _int curCellIndex, const _vec3 & nextPosition, _int & nextCellIndex, _vec3 & fixPosition)
{
	if (0 > curCellIndex)
		return false;

	if (mNaviCellList.size() <= curCellIndex)
		return false;

	NaviMesh::CellNeighbor neighborIndex;
	NaviCell_Object* curCell = mNaviCellList[curCellIndex];
	
	if (false == curCell->Search(nextPosition, fixPosition, neighborIndex))
	{
		// nextPosition이 현재 Cell이 아닌 경우
		NaviCell_Object* nextCell = curCell->GetNeighborCell(neighborIndex);
		if (nullptr == nextCell)
			return false;

		nextCell->Search(nextPosition, fixPosition, neighborIndex);
	}

	return true;
}

_int NaviMesh_Object::FindCellIndex(const _vec3& position) const
{
	for (_int i = 0; i < mNaviCellList.size(); ++i)
	{
		const NaviCell_Object * const & cell = mNaviCellList[i];
		if (cell->IsInCell(position))
			return i;
	}
	return -1;
}

void NaviMesh_Object::SetNaviMeshData(const NaviMeshData & naviMeshData)
{
	struct LINKINFO
	{
		array<_int, 2>				RefPontIdx;
		vector<pair<NaviCell_Object*, NaviMesh::CellNeighbor>>	AdjCell;
	};

	const NaviMeshData::Info& info = naviMeshData.GetInfo();

	//	NaviCell 생성
	list<LINKINFO>	linkInfolist;
	mNaviCellList.reserve(info.CellInfoList.size());
	for (_int cellIndex = 0; cellIndex < info.CellInfoList.size(); ++cellIndex)
	{
		//	NaviCell 생성
		const NaviMeshData::CellInfo& cellInfo = info.CellInfoList[cellIndex];
		NaviCell_Object* newCell = NaviCell_Object::Create(cellIndex, info.PointList[cellInfo.PointIdx[NaviMesh::POINT_A]].Position, info.PointList[cellInfo.PointIdx[NaviMesh::POINT_B]].Position, info.PointList[cellInfo.PointIdx[NaviMesh::POINT_C]].Position);
		mNaviCellList.emplace_back(newCell);

		//	NeighborInfo 생성
		array<array<int, 2>, NaviMesh::CELL_NEIGHBOR_END> lineInfo;
		lineInfo[NaviMesh::CELL_NEIGHBOR_AB] = { cellInfo.PointIdx[NaviMesh::POINT_A], cellInfo.PointIdx[NaviMesh::POINT_B] };
		lineInfo[NaviMesh::CELL_NEIGHBOR_BC] = { cellInfo.PointIdx[NaviMesh::POINT_B], cellInfo.PointIdx[NaviMesh::POINT_C] };
		lineInfo[NaviMesh::CELL_NEIGHBOR_CA] = { cellInfo.PointIdx[NaviMesh::POINT_C], cellInfo.PointIdx[NaviMesh::POINT_A] };
		
		for (_int i = 0; i < NaviMesh::CELL_NEIGHBOR_END; ++i)
		{
			const array<int, 2>& line = lineInfo[i];
			
			_bool isFind = false;
			for (LINKINFO& linkInfo : linkInfolist)
			{
				if ((linkInfo.RefPontIdx[0] == line[0] && linkInfo.RefPontIdx[1] == line[1]) ||
					(linkInfo.RefPontIdx[0] == line[1] && linkInfo.RefPontIdx[1] == line[0]))
				{
					isFind = true;
					linkInfo.AdjCell.emplace_back(newCell, (NaviMesh::CellNeighbor)i);
					break;
				}
			}

			if (false == isFind)
			{
				LINKINFO newLinkInfo;
				newLinkInfo.RefPontIdx = line;
				newLinkInfo.AdjCell.emplace_back(newCell, (NaviMesh::CellNeighbor)i);
				linkInfolist.emplace_back(newLinkInfo);
			}
		}
	}

	//	NeighborInfo를 활용하여 Neighbor 세팅
	for (LINKINFO& linkInfo : linkInfolist)
	{
		if (2 > linkInfo.AdjCell.size())
			continue;
		pair<NaviCell_Object*, NaviMesh::CellNeighbor>& lhs = linkInfo.AdjCell[0];
		pair<NaviCell_Object*, NaviMesh::CellNeighbor>& rhs = linkInfo.AdjCell[1];

		lhs.first->LinkNeighborCell(lhs.second, rhs.first, rhs.second);
		rhs.first->LinkNeighborCell(rhs.second, lhs.first, lhs.second);
	}
}

_bool NaviMesh_Object::Initialize()
{
	return true;
}

NaviMesh_Object * NaviMesh_Object::Create(LPDIRECT3DDEVICE9 graphicDevice)
{
	NaviMesh_Object*	pInstance = new NaviMesh_Object();

	if (false == pInstance->Initialize())
	{
		MSG_BOX("NaviMesh_Object Created Failed!");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void NaviMesh_Object::Free()
{
	for (NaviCell_Object* cell : mNaviCellList)
		SafeRelease(cell);
	mNaviCellList.clear();
	
}

