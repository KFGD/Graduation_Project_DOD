#pragma once
#include "Base.h"
#include "Mode.h"

#include "Defines.h"
#include "Client_Defines.h"

class KObject;

class CreativeMode :
	public Base, public IMode
{

private:
	explicit CreativeMode();
	virtual ~CreativeMode() = default;
	
public:
	virtual void	Active(IWorldController* worldController)	override;
	virtual void	InActive()	override;
	virtual void	Update()	override;
	
private:
	void	UpdateDisplayObjectListUI();
	void	UpdateEditorUI();

private:
	void	UpdateObjectList();
	void	UpdateDisplayList();
	void	ClearObjectList();
	void	ClearDisplayObjectList();

private:
	void	InitSampleData();

private:
	vector<KObject*>	mObjectList;

private:
	//	Display Object List
	_int	mSelectedObjectListIndex;
	array<_bool, Game::Type_End> mbDisplayObjectFilter;
	vector<pair<_int, KObject*>>	mDisplayObjectList;

	typedef pair<_int, KObject*>	DISPLAY_PAIR;

private:
	//	Editor
	Game::ObjectType	mCurSelectedObjectType = Game::Player;

public:
	static CreativeMode*	Create();
	virtual void	Free() override;
};

