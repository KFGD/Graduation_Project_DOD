#pragma once
#include "Base.h"

#include "Defines.h"

class Scene;

class World :
	public Base
{
public:
	enum class STATE { START, RUN, PAUSE, RESUME, FINISH };

protected:
	explicit World(const LPDIRECT3DDEVICE9 graphicDev);
	explicit World(const World& rhs) = delete;
	virtual ~World() = default;

public:
	virtual void	Update(const _double timeDelta) = 0;
	virtual void	Render() = 0;
	virtual _bool	Clear() = 0;
public:
	void	SetState(const STATE sceneState);
	STATE	GetState() const { return mWorldState; }
	LPDIRECT3DDEVICE9	GetGraphicDevice() { return mGraphicDevice; }

protected:
	LPDIRECT3DDEVICE9	GetGraphicDev() { return mGraphicDevice; }

private:
	LPDIRECT3DDEVICE9	mGraphicDevice = nullptr;

private:
	STATE	mWorldState = STATE::START;
	Scene*	mScene = nullptr;

public:
	virtual void	Free()	override;
};

