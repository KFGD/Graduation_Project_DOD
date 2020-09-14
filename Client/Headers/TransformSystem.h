#pragma once
#include "ComponentSystem.h"

class TransformSystem :
	public ComponentSystem
{
	DECLARE_SINGLETON(TransformSystem)

public:
	struct Component
	{
		_vec3	Scale;
		_vec3	Rotation;
		_vec3	Position;

		_matrix	WorldMatrix;

		Component(const _vec3& scale = _vec3(0.f, 0.f, 0.f), const _vec3& rotation = _vec3(0.f, 0.f, 0.f), const _vec3& position = _vec3(0.f, 0.f, 0.f))
			: Scale(scale), Rotation(rotation), Position(position)
		{}
	};

private:
	explicit TransformSystem();
	virtual ~TransformSystem() = default;

public:
	_bool	ReadySystem(const _size_t entitySize);
	void	ClearSystem();

public:
	virtual void	Update(const _double timeDelta)		override;
	virtual void	LateUpdate(const _double timeDelta)	override;

public:
	_bool			AttachComponent(const _uniqueId entityId, const _vec3& scale, const _vec3& rotation, const _vec3& position);
	void			SetScale(const _uniqueId entityId, const _vec3& scale);
	void			SetRotationX(const _uniqueId entityId, const _float radian);
	void			SetRotationY(const _uniqueId entityId, const _float radian);
	void			SetRotationZ(const _uniqueId entityId, const _float radian);
	void			SetPosition(const _uniqueId entityId, const _vec3& position);
	const _vec3&	GetPosition(const _uniqueId entityId) const;
	const _matrix&	GetWorldMatrix(const _uniqueId entityId) const;
	
private:

private:
	_size_t	mEntitySize = 0;

	vector<Component>	mComponentList;

public:
	virtual void	Free()	override;
};

