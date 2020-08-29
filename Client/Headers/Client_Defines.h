#pragma once

namespace Program
{
	enum Mode : int
	{
		GAME,
		CREATIVE,
		MODE_END
	};

	enum NaviMesh : int 
	{
		NAVIMESH_DISABLED,
		NAVIMESH_ADD,
		NAVIMESH_EDIT
	};
};

namespace CameraType
{
	enum Type : int 
	{
		FREE_CAMERA,
		TYPE_END
	};
};

namespace NaviMesh
{
	enum Point
	{
		POINT_A,
		POINT_B,
		POINT_C,
		POINT_END
	};

	enum CellNeighbor
	{
		CELL_NEIGHBOR_AB,
		CELL_NEIGHBOR_BC,
		CELL_NEIGHBOR_CA,
		CELL_NEIGHBOR_END
	};

	//struct NaviLine
	//{
	//	_int	Idx0;
	//	_int	Idx1;

	//	NaviLine(const _int idx0, const _int idx1)
	//		: Idx0(idx0), Idx1(idx1)
	//	{
	//	}

	//	_bool	operator==(const NaviLine& rhs)
	//	{
	//		return (this->Idx0 == rhs.Idx0 && this->Idx1 == rhs.Idx1)
	//			|| (this->Idx1 == rhs.Idx0 && this->Idx0 == rhs.Idx1);
	//	}
	//};
}

namespace Game
{
	enum WorldType
	{
		Data_Oriented,
		Object_Oriented,
		WorldEnd
	};

	enum ObjectType
	{
		Player,
		Bot,
		Block,
		Type_End
	};
}