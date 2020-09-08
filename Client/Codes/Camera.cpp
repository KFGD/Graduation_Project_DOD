#include "stdafx.h"
#include "..\Headers\Camera.h"

const _vec3 Camera::AXIS_Y = _vec3(0.f, 1.f, 0.f);

Camera::Camera()
{
}

void Camera::Rotate(const _matrix & rotationMatrix)
{
	_vec3	right = GetRightVector();
	_vec3	up = GetUpVector();
	_vec3	forward = GetForwardVector();

	D3DXVec3TransformNormal(&right, &right, &rotationMatrix);
	D3DXVec3TransformNormal(&up, &up, &rotationMatrix);
	D3DXVec3TransformNormal(&forward, &forward, &rotationMatrix);

	SetRightVector(right);
	SetUpVector(up);
	SetForwardVector(forward);
}

void Camera::Free()
{

}

