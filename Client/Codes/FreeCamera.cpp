#include "stdafx.h"
#include "FreeCamera.h"

#include "PipeLine.h"
#include "InputDevice.h"
#include "KeyManager.h"

FreeCamera::FreeCamera()
{
}

void FreeCamera::Active(CameraController * cameraController)
{

}

void FreeCamera::InActive(CameraController * cameraController)
{
}

void FreeCamera::Update(const _double deltaTime)
{

	InputDevice* inputDevice = InputDevice::GetInstance();
	KeyManager*	keyManager = KeyManager::GetInstance();

	_vec3	dir;
	::ZeroMemory(&dir, sizeof(_vec3));

	if (keyManager->KeyPresseing(KeyManager::KEY_W))
		dir += GetForwardVector();

	if (keyManager->KeyPresseing(KeyManager::KEY_S))
		dir -= GetForwardVector();

	if (keyManager->KeyPresseing(KeyManager::KEY_A))
		dir -= GetRightVector();

	if (keyManager->KeyPresseing(KeyManager::KEY_D))
		dir += GetRightVector();

	if (_vec3(0.f, 0.f, 0.f) != dir)
	{
		const _vec3 curPos = GetPosition();
		const _vec3 nextPos = curPos + dir * mMoveSpeed * (_float)deltaTime;
		SetPosition(nextPos);
	}

	POINT	mouse{ WINCX >> 1, WINCY >> 1 };
	ClientToScreen(g_hWnd, &mouse);
	SetCursorPos(mouse.x, mouse.y);

	_float mouseMove[2] = { (_float)inputDevice->GetDIMouseMove(InputDevice::DIMM_X), (_float)inputDevice->GetDIMouseMove(InputDevice::DIMM_Y) };

	for (_int i = 0; i < 2; ++i)
	{
		_vec3 axis[2] = { Camera::AXIS_Y, GetRightVector() };

		const _float moveValue = mouseMove[i] * (_float)deltaTime;

		_matrix	rotationMatrix;
		D3DXMatrixRotationAxis(&rotationMatrix, &axis[i], moveValue);

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


	_matrix matView;
	D3DXMatrixInverse(&matView, nullptr, &mWorldMatrix);

	PipeLine::GetInstance()->SetTransform(D3DTS_VIEW, matView);
}

void FreeCamera::LateUpdate(const _double deltaTime)
{
}

void FreeCamera::ResetRotation()
{
	SetRightVector(_vec3(1.f, 0.f, 0.f));
	SetUpVector(_vec3(0.f, 1.f, 0.f));
	SetForwardVector(_vec3(0.f, 0.f, 1.f));
}

_bool FreeCamera::Initialize()
{
	mMoveSpeed = 3.f;

	_matrix viewMatrix;
	D3DXMatrixLookAtLH(&viewMatrix, &_vec3(0.f, 3.f, -3.f), &_vec3(0.f, 0.f, 0.f), &Camera::AXIS_Y);
	D3DXMatrixInverse(&mWorldMatrix, nullptr, &viewMatrix);

	return true;
}

FreeCamera * FreeCamera::Create()
{
	FreeCamera*	pInstance = new FreeCamera();
	if (false == pInstance->Initialize())
	{
		MSG_BOX("FreeCamera Created Failed!");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void FreeCamera::Free()
{
	Camera::Free();
}

