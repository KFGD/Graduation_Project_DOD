#pragma once
#include "Component_Object.h"

class Transform_Object :
	public Component_Object
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
	explicit Transform_Object(const Data& data);
	virtual ~Transform_Object() = default;

public:
	void	SetScale(const _vec3& scale) { memcpy(&mSRT.Scale, &scale, sizeof(_vec3)); }
	void	SetRotationX(const _float radian) { mSRT.Rotation.x = radian; }
	void	SetRotationY(const _float radian) { mSRT.Rotation.y = radian; }
	void	SetRotationZ(const _float radian) { mSRT.Rotation.z = radian; }
	void	SetPosition(const _vec3& position) { memcpy(&mSRT.Position, &position, sizeof(_vec3)); }
	const _matrix&	GetWorldMatrix() const { return mWorldMatrix; }

public:
	void	CalculateWorldMatrix(const _matrix& pivotMatrix = IDENTITY_MATRIX);

private:
	Data	mSRT;
	_matrix		mWorldMatrix;

public:
	virtual void	Free() override;
	static Transform_Object*	Create(const Data& data = Data());
	virtual Component_Object*	Clone(void* arg) override;
};
