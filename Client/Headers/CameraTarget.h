#pragma once

#include "Defines.h"

class CameraTarget
{
protected:
	explicit	CameraTarget() = default;
	virtual		~CameraTarget() = default;

public:
	virtual		const _vec3&	GetPosition() const = 0;
};
