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