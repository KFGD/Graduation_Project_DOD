#include "stdafx.h"
#include "TransformSystem.h"

#include "StaticRendererSystem.h"
#include "DynamicRendererSystem.h"

IMPLEMENT_SINGLETON(TransformSystem)

TransformSystem::TransformSystem()
{
}

_bool TransformSystem::ReadySystem(const _size_t entitySize)
{
	if (0 != mEntitySize)
		return false;

	mEntitySize = entitySize;

	mComponentList.resize(mEntitySize);

	return true;
}

void TransformSystem::ClearSystem()
{
	mComponentList.clear();
	mComponentList.shrink_to_fit();

	mEntitySize = 0;
}

void TransformSystem::Update(const _double timeDelta)
{
	for(_uniqueId i = 0; i < mEntitySize; ++i)
	{
		Component& component = mComponentList[i];

		_matrix scale = *D3DXMatrixScaling(&scale, component.Scale.x, component.Scale.y, component.Scale.z);
		_matrix rotation_x = *D3DXMatrixRotationX(&rotation_x, D3DXToRadian(component.Rotation.x));
		_matrix rotation_y = *D3DXMatrixRotationY(&rotation_y, D3DXToRadian(component.Rotation.y));
		_matrix rotation_z = *D3DXMatrixRotationZ(&rotation_z, D3DXToRadian(component.Rotation.z));
		_matrix position = *D3DXMatrixTranslation(&position, component.Position.x, component.Position.y, component.Position.z);

		component.WorldMatrix = scale * rotation_x * rotation_y * rotation_z * position;
	}

	StaticRendererSystem* const staticRendererSystem = StaticRendererSystem::GetInstance();
	for (_uniqueId i = 0; i < mEntitySize; ++i)
		staticRendererSystem->SetWorldMatrix(i, mComponentList[i].WorldMatrix);

	DynamicRendererSystem* const dynamicRendererSystem = DynamicRendererSystem::GetInstance();
	for (_uniqueId i = 0; i < mEntitySize; ++i)
		dynamicRendererSystem->SetWorldMatrix(i, mComponentList[i].WorldMatrix);
}

void TransformSystem::LateUpdate(const _double timeDelta)
{
}

_bool TransformSystem::AttachComponent(const _uniqueId entityId, const _vec3& scale, const _vec3& rotation, const _vec3& position)
{
	mComponentList[entityId].Scale = scale;
	mComponentList[entityId].Rotation = rotation;
	mComponentList[entityId].Position = position;
	return true;
}

void TransformSystem::SetScale(const _uniqueId entityId, const _vec3 & scale)
{
	mComponentList[entityId].Scale = scale;
}

void TransformSystem::SetRotationX(const _uniqueId entityId, const _float radian)
{
	mComponentList[entityId].Rotation.x = radian;
}

void TransformSystem::SetRotationY(const _uniqueId entityId, const _float radian)
{
	mComponentList[entityId].Rotation.y = radian;
}

void TransformSystem::SetRotationZ(const _uniqueId entityId, const _float radian)
{
	mComponentList[entityId].Rotation.z = radian;
}

void TransformSystem::SetPosition(const _uniqueId entityId, const _vec3 & position)
{
	mComponentList[entityId].Position = position;
}

const _vec3 & TransformSystem::GetPosition(const _uniqueId entityId) const
{
	return mComponentList[entityId].Position;
}

const _matrix & TransformSystem::GetWorldMatrix(const _uniqueId entityId) const
{
	return mComponentList[entityId].WorldMatrix;
}

void TransformSystem::Free()
{
	ClearSystem();

	ComponentSystem::Free();
}
