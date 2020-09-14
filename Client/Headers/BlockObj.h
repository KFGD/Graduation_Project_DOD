#pragma once
#include "GameObject.h"

class Transform_Object;
class StaticMeshRenderer_Object;
class Shader;

class BlockObj :
	public GameObject
{
public:
	struct Data
	{
		_vec3	Scale;
		_vec3	Rotation;
		_vec3	Position;

		Data(const _vec3& scale = _vec3(0.f, 0.f, 0.f), const _vec3& rotation = _vec3(0.f, 0.f, 0.f), const _vec3& position = _vec3(0.f, 0.f, 0.f))
			: Scale(scale), Rotation(rotation), Position(position)
		{
		}
	};

private:
	explicit BlockObj();
	virtual ~BlockObj() = default;

public:
	virtual void Update(const _double timeDelta) override;
	virtual void LateUpdate(const _double timeDelta) override;
	virtual void Render() override;

public:
	Transform_Object*	GetTransform() { return mTransform; }
	StaticMeshRenderer_Object*	GetStaticMesh() { return mStaticMesh; }
	Shader*				GetSahder() { return mShader; }

private:
	_bool	Initialize(const BlockObj::Data& data);

private:
	Transform_Object*	mTransform = nullptr;
	StaticMeshRenderer_Object*	mStaticMesh = nullptr;
	Shader*				mShader = nullptr;

public:
	virtual void Free() override;
	static BlockObj*	Create(const BlockObj::Data& data = BlockObj::Data());

};

