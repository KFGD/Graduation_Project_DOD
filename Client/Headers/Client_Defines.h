#pragma once

namespace Program
{
	enum Mode : int
	{
		GAME,
		CREATIVE
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