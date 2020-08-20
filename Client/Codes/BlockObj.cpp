#include "stdafx.h"
#include "BlockObj.h"

#include "Transform_Object.h"
#include "DynamicMesh_Object.h"

#include "World.h"
#include "PipeLine.h"
#include "Shader.h"

BlockObj::BlockObj()
{
}

void BlockObj::Update(const _double timeDelta)
{
	mTransform->CalculateWorldMatrix();
}

void BlockObj::LateUpdate(const _double timeDelta)
{
	
}

void BlockObj::Render()
{
}

_bool BlockObj::Initialize(const BlockObj::Data & data)
{
	GameObject::AddComponent("Transform", "Transform", (Component_Object**)&mTransform, &Transform_Object::Data(data.Scale, data.Rotation, data.Position));
	GameObject::AddComponent("StaticMesh_GrayBlock", "StaticMesh_GrayBlock", (Component_Object**)&mStaticMesh);
	GameObject::AddComponent("Shader_HardwareInstancing", "Shader_HardwareInstancing", (Component_Object**)&mShader);

	return true;
}

void BlockObj::Free()
{
	GameObject::Free();
}

BlockObj * BlockObj::Create(const BlockObj::Data & data)
{
	BlockObj*	pInstnace = new BlockObj();

	if (false == pInstnace->Initialize(data))
	{
		SafeRelease(pInstnace);
		MSG_BOX("Failed To Create BlockObj Instance");
	}

	return pInstnace;
}

