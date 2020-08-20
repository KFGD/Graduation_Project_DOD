#include "stdafx.h"
#include "World.h"

World::World(const LPDIRECT3DDEVICE9 graphicDev)
	: mGraphicDevice(graphicDev)
{
	SafeAddRef(mGraphicDevice);
}

void World::SetState(const STATE sceneState)
{
	mWorldState = sceneState;
}

void World::Free()
{
	SafeRelease(mGraphicDevice);
}