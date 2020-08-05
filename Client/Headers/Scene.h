#pragma once
#include "Base.h"

#include "Defines.h"

class World;

class Scene :
	public Base
{
public:
	enum class SCENE_TYPE { Object_Oriented, Data_Oriented };

protected:
	explicit Scene();
	virtual ~Scene() = default;

public:
	virtual void	Start() = 0;
	virtual void	Update(const _double timeDelta) = 0;
	virtual void	LateUpdate(const _double timeDelta) = 0;
	virtual void	Render() = 0;

public:
	virtual SCENE_TYPE GetType() const = 0;

public:
	void	SetUp(World* world);
	
public:
	virtual void	Free() override;
};

