#pragma once
#include "GameObject.h"

class Transform_Object;
class DynamicMesh_Object;
class Shader;

class Bot :
	public GameObject
{
public:
	struct Data
	{
		_vec3	Scale;
		_vec3	Rotation;
		_vec3	Position;

		Data(const _vec3& scale = _vec3(0.f, 0.f, 0.f), const _vec3& rotation = _vec3(0.f, 0.f, 0.f), const _vec3 position = _vec3(0.f, 0.f, 0.f))
			: Scale(scale), Rotation(rotation), Position(position)
		{
		}
	};
private:
	explicit Bot();
	virtual ~Bot() = default;

public:
	virtual void Update(const _double timeDelta) override;
	virtual void LateUpdate(const _double timeDelta) override;
	virtual void Render() override;

private:
	_bool	Initialize(const Bot::Data& data);

private:
	Transform_Object*	mTransform = nullptr;
	DynamicMesh_Object*	mDynamicMesh = nullptr;
	Shader*				mShader = nullptr;

public:
	virtual void Free() override;
	static Bot*	Create(const Bot::Data& data = Bot::Data());
};

