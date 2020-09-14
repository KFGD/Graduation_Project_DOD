#include "stdafx.h"
#include "TransformSystem.h"

IMPLEMENT_SINGLETON(TransformSystem)

TransformSystem::TransformSystem()
{
}

_bool TransformSystem::Ready(const _size_t entitySize, const _size_t componentSize)
{
	if (0 != mEntitySize || 0 != mComponentSize)
		return false;

	mEntitySize = entitySize;
	mComponentSize = componentSize;

	mIndexTable.resize(entitySize, componentSize);

	for (_uniqueId i = 0; i < componentSize; ++i)
		mIndexQueue.emplace(i);

	mComponentList.resize(componentSize + 1);	// 마지막은 별도의 용도

	return true;
}

void TransformSystem::Clear()
{
	mComponentList.clear();
	mComponentList.shrink_to_fit();

	mIndexQueue.swap(queue<_uniqueId>());

	mIndexTable.clear();
	mIndexTable.shrink_to_fit();

	mEntitySize = 0;
	mComponentSize = 0;
}

void TransformSystem::Update(const _double timeDelta)
{
	for(_size_t i = 0; i < mComponentSize; ++i)
	{
		Component& component = mComponentList[i];

		_matrix scale = *D3DXMatrixScaling(&scale, component.Scale.x, component.Scale.y, component.Scale.z);
		_matrix rotation_x = *D3DXMatrixRotationX(&rotation_x, D3DXToRadian(component.Rotation.x));
		_matrix rotation_y = *D3DXMatrixRotationY(&rotation_y, D3DXToRadian(component.Rotation.y));
		_matrix rotation_z = *D3DXMatrixRotationZ(&rotation_z, D3DXToRadian(component.Rotation.z));
		_matrix position = *D3DXMatrixTranslation(&position, component.Position.x, component.Position.y, component.Position.z);

		component.WorldMatrix = scale * rotation_x * rotation_y * rotation_z;
	}
}

void TransformSystem::LateUpdate(const _double timeDelta)
{
}

_bool TransformSystem::AttachComponent(const _uniqueId entityId)
{
	if (0 == mIndexQueue.size())
		return false;

	const int index = mIndexQueue.front();
	mIndexQueue.pop();

	mIndexTable[entityId] = index;

	return true;
}

void TransformSystem::SetScale(const _uniqueId entityId, const _vec3 & scale)
{
	const int index = mIndexTable[entityId];
	mComponentList[index].Scale = scale;
}

void TransformSystem::SetRotationX(const _uniqueId entityId, const _float radian)
{
	const int index = mIndexTable[entityId];
	mComponentList[index].Rotation.x = radian;
}

void TransformSystem::SetRotationY(const _uniqueId entityId, const _float radian)
{
	const int index = mIndexTable[entityId];
	mComponentList[index].Rotation.y = radian;
}

void TransformSystem::SetRotationZ(const _uniqueId entityId, const _float radian)
{
	const int index = mIndexTable[entityId];
	mComponentList[index].Rotation.z = radian;
}

void TransformSystem::SetPosition(const _uniqueId entityId, const _vec3 & position)
{
	const int index = mIndexTable[entityId];
	mComponentList[index].Position = position;
}

const _vec3 & TransformSystem::GetPosition(const _uniqueId entityId) const
{
	const int index = mIndexTable[entityId];
	return mComponentList[index].Position;
}

const _matrix & TransformSystem::GetWorldMatrix(const _uniqueId entityId) const
{
	const int index = mIndexTable[entityId];
	return mComponentList[index].WorldMatrix;
}

void TransformSystem::Free()
{
	Clear();
}
