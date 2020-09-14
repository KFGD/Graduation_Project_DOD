#pragma once
#include "GameObject.h"

#include "State.h"

class Transform_Object;
class DynamicMeshRenderer_Object;
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

public:
	enum BotState
	{
		IDLE,
		RUN,
		STATE_END
	};

private:
	explicit Bot();
	virtual ~Bot() = default;

public:
	virtual void	SetUp(World_Object* world)	override;
	virtual void	Update(const _double timeDelta) override;
	virtual void	LateUpdate(const _double timeDelta) override;
	virtual void	Render() override;

public:
	void	SetNextState(const BotState nextState);
	void	Move(const _vec3& moveDir);
	void	SetUpAnimation(const _uint index);

private:
	_bool	Initialize(const Bot::Data& data);

private:
	Transform_Object*	mTransform = nullptr;
	DynamicMeshRenderer_Object*	mDynamicMesh = nullptr;
	Shader*				mShader = nullptr;

private:
	array<State<Bot>*, STATE_END>	mArrBotState;
	BotState	mCurState;
	BotState	mNextState;

private:
	//	Animation
	_double	mTimeDelta = 0.0;

private:
	//	Navi Mesh
	_int	mCellIndex = 0;
	

public:
	virtual void Free() override;
	static Bot*	Create(const Bot::Data& data = Bot::Data());
};

