#include "stdafx.h"
#include "KObject.h"


KObject::KObject(const Info& info)
	: mInfo(info)
{
}

const char * KObject::GetObjectTypeName() const
{
	switch (mInfo.Objecttype) 
	{
	case Type::Player:
		return "Player";

	case Type::Bot:
		return "Bot";

	case Type::Block:
		return "Block";

	default:
		return "";
	}
}

void KObject::SetInfo(const KObject::Info & info)
{
	mInfo = info;
	CalculateWorldMatirx();
}

_bool KObject::Initialize()
{
	CalculateWorldMatirx();
	return true;
}

void KObject::CalculateWorldMatirx()
{
	_matrix scale = *D3DXMatrixScaling(&scale, mInfo.Transform.Scale.x, mInfo.Transform.Scale.y, mInfo.Transform.Scale.z);
	_matrix rotation_x = *D3DXMatrixRotationX(&rotation_x, D3DXToRadian(mInfo.Transform.Rotation.x));
	_matrix rotation_y = *D3DXMatrixRotationY(&rotation_y, D3DXToRadian(mInfo.Transform.Rotation.y));
	_matrix rotation_z = *D3DXMatrixRotationZ(&rotation_z, D3DXToRadian(mInfo.Transform.Rotation.z));
	_matrix position = *D3DXMatrixTranslation(&position, mInfo.Transform.Position.x, mInfo.Transform.Position.y, mInfo.Transform.Position.z);

	mWorldMatrix = scale * rotation_x * rotation_y * rotation_z * position;

	D3DXMatrixInverse(&mWorldInverseMatrix, nullptr, &mWorldMatrix);
}

KObject * KObject::Create(const Info & objectInfo)
{
	KObject*	pInstance = new KObject(objectInfo);
	if (false == pInstance->Initialize())
	{
		MSG_BOX("Failed To Create KObject Instance");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void KObject::Free()
{
}
