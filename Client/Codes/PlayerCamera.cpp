#include "stdafx.h"
#include "PlayerCamera.h"

#include "PipeLine.h"
#include "InputDevice.h"
#include "CameraController.h"
#include "CameraTarget.h"

PlayerCamera::PlayerCamera()
{
}

void PlayerCamera::Active(CameraController * cameraController)
{
	mTarget = cameraController->GetCameraTarget();
}

void PlayerCamera::InActive(CameraController * cameraController)
{
	mTarget = nullptr;
}

void PlayerCamera::Update(const _double deltaTime)
{
	if (nullptr == mTarget)
		return;

	POINT	mouse{ WINCX >> 1, WINCY >> 1 };
	ClientToScreen(g_hWnd, &mouse);
	SetCursorPos(mouse.x, mouse.y);

	InputDevice* inputDevice = InputDevice::GetInstance();
	const _long	mouseMove = inputDevice->GetDIMouseMove(InputDevice::DIMM_X);

	if (mouseMove)
		mRadian += D3DXToRadian(mouseMove / 10.f);
	
	const _float absRadian = abs(mRadian);
	if (absRadian > D3DX_PI)
	{
		const _float sign = mRadian / absRadian * -1.f;
		mRadian = mRadian * -1.f - (absRadian - D3DX_PI) * 2.f * sign;
	}

	_matrix matRotate;
	D3DXMatrixRotationY(&matRotate, mRadian);

	_vec3 pos = -1.f * mLook * mDist;
	D3DXVec3TransformCoord(&pos, &pos, &matRotate);
	pos += mTarget->GetPosition() + _vec3(0.f, 1.f, 0.f);

	_vec3 look = mLook;
	D3DXVec3TransformNormal(&look, &look, &matRotate);
	
	_vec3 right;
	//D3DXVec3Cross(&right, &look, &Camera::AXIS_Y);
	D3DXVec3Cross(&right, &Camera::AXIS_Y, &look);
	D3DXVec3Normalize(&right, &right);

	_vec3 up;
	D3DXVec3Cross(&up, &look, &right);
	D3DXVec3Normalize(&up, &up);

	_matrix matWorld;
	D3DXMatrixIdentity(&matWorld);
	(*(_vec3*)matWorld.m[0]) = right;
	(*(_vec3*)matWorld.m[1]) = up;
	(*(_vec3*)matWorld.m[2]) = look;
	(*(_vec3*)matWorld.m[3]) = pos;

	SetWorldMatrix(matWorld);

	_matrix matView;
	D3DXMatrixInverse(&matView, nullptr, &GetWorldMatrix());

	PipeLine::GetInstance()->SetTransform(D3DTS_VIEW, matView);
}

void PlayerCamera::LateUpdate(const _double deltaTime)
{

}

_bool PlayerCamera::Initialize()
{
	mDist = 0.8f;
	mLook = _vec3(0.f, -1.5f, 1.f);
	D3DXVec3Normalize(&mLook, &mLook);

	return true;
}

PlayerCamera * PlayerCamera::Create()
{
	PlayerCamera*	pInstance = new PlayerCamera();
	if (false == pInstance->Initialize())
	{
		MSG_BOX("PlayerCamera Created Failed!");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void PlayerCamera::Free()
{
	Camera::Free();
}
