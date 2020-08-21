#pragma once
#include "Base.h"

#include "Defines.h"
#include "Client_Defines.h"

namespace KEngine
{
	struct Transform
	{
		_vec3 Scale;
		_vec3 Rotation;
		_vec3 Position;

		Transform(const _vec3& scale = _vec3(0.f, 0.f, 0.f), const _vec3& rotation = _vec3(0.f, 0.f, 0.f), const _vec3& position = _vec3(0.f, 0.f, 0.f))
			: Scale(scale), Rotation(rotation), Position(position)
		{
		}
	};
}

class KObject :
	public Base
{
	typedef	Game::ObjectType	Type;
public:
	struct Info
	{
		Type Objecttype;
		KEngine::Transform Transform;

		Info(const Type objectType = Game::Player, const KEngine::Transform transform = KEngine::Transform())
			: Objecttype(objectType), Transform(transform)
		{
		}
	};

private: 
	explicit KObject(const Info& info);
	virtual ~KObject() = default;
	
public:
	const Info&		GetInfo() const { return mInfo; }
	const char*		GetObjectTypeName() const;
	const _matrix&	GetWorldMatrix() const { return mWorldMatrix; }
	const _matrix&	GetWorldInverseMatrix() const { return mWorldInverseMatrix; }
	void			SetInfo(const KObject::Info& info);

private:
	_bool		Initialize();
	void		CalculateWorldMatirx();

private:
	Info	mInfo;
	_matrix	mWorldMatrix;
	_matrix	mWorldInverseMatrix;

public:
	static KObject*	Create(const Info& objectInfo);
	virtual void	Free() override;
};

