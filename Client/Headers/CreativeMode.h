#pragma once
#include "Base.h"

#include "Defines.h"
#include "Client_Defines.h"

class KObject;

class CreativeMode :
	public Base
{
	typedef Game::ObjectType Type;

private:
	explicit CreativeMode();
	virtual ~CreativeMode() = default;
	
public:
	void	Active();
	void	InActive();

public:
	void	Update();


private:
	void	UpdateDisplayObjectListUI();
	void	UpdateEditorUI();

private:
	void	UpdateObjectList();
	void	UpdateDisplayList();
	void	ClearObjectList();
	void	ClearDisplayObjectList();

private:
	//	Display Object List
	vector<KObject*>	mObjectList;
	_int	mSelectedObjectListIndex;
	array<_bool, Type::End> mbDisplayObjectFilter;
	vector<pair<_int, KObject*>>	mDisplayObjectList;

	typedef pair<_int, KObject*>	DISPLAY_PAIR;

	//	Editor
	Type	mCurSelectedObjectType = Type::Player;

public:
	static CreativeMode*	Create();
	virtual void	Free() override;
};

