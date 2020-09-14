#pragma once
#include "Base.h"

#include "Defines.h"

class TransformSystem :
	public Base
{
	DECLARE_SINGLETON(TransformSystem);

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
	_bool	Ready(const _size_t entitySize, const _size_t componentSize);
	void	Clear();

public:
	void	Update(const _double timeDelta);
	void	LateUpdate(const _double timeDelta);

public:
	_bool			AttachComponent(const _uniqueId entityId);
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
	_size_t	mComponentSize = 0;

	vector<_uniqueId>	mIndexTable;
	queue<_uniqueId>	mIndexQueue;
	vector<Component>	mComponentList;

public:
	virtual void	Free()	override;
};

