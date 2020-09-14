#include "stdafx.h"
#include "Transform_Object.h"


Transform_Object::Transform_Object(const Data & data)
	: mSRT(data)
{
}

void Transform_Object::CalculateWorldMatrix(const _matrix & pivotMatrix)
{
	_matrix scale = *D3DXMatrixScaling(&scale, mSRT.Scale.x, mSRT.Scale.y, mSRT.Scale.z);
	_matrix rotation_x = *D3DXMatrixRotationX(&rotation_x, D3DXToRadian(mSRT.Rotation.x));
	_matrix rotation_y = *D3DXMatrixRotationY(&rotation_y, D3DXToRadian(mSRT.Rotation.y));
	_matrix rotation_z = *D3DXMatrixRotationZ(&rotation_z, D3DXToRadian(mSRT.Rotation.z));
	_matrix position = *D3DXMatrixTranslation(&position, mSRT.Position.x, mSRT.Position.y, mSRT.Position.z);

	mWorldMatrix = scale * rotation_x * rotation_y * rotation_z * position;
}

void Transform_Object::Free()
{
	Component_Object::Free();
}

Transform_Object * Transform_Object::Create(const Data & data)
{
	return new Transform_Object(data);
}

Component_Object * Transform_Object::Clone(void * arg)
{
	if (nullptr == arg)
		return new Transform_Object(this->mSRT);
	return new Transform_Object(*(Data*)arg);
}
