#pragma once
#include "GameObject.h"

class Transform_Object;
class DynamicMesh_Object;
class Shader;

class Player :
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

public:
	explicit Player();
	virtual ~Player() = default;

public:
	virtual void	SetUp(World_Object* world)	override;
	virtual void	Update(const _double timeDelta) override;
	virtual void	LateUpdate(const _double timeDelta) override;
	virtual void	Render() override;

private:
	_bool	Initialize(const Player::Data& data);

private:
	Transform_Object*	mTransform = nullptr;
	DynamicMesh_Object*	mDynamicMesh = nullptr;
	Shader*				mShader = nullptr;

private:
	_int	mCellIndex = 0;
	_double	mTimeDelta = 0.0;

public:
	virtual void	Free() override;
	static Player*	Create(const Player::Data& data = Player::Data());

};

