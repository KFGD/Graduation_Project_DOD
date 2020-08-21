#include "stdafx.h"
#include "CameraController.h"

#include "Camera.h"
#include "FreeCamera.h"

IMPLEMENT_SINGLETON(CameraController)

CameraController::CameraController()
{
	mCameraList.fill(nullptr);
}

_bool CameraController::Ready()
{
	mCameraList[CameraType::FREE_CAMERA] = FreeCamera::Create();
	mCurType = CameraType::FREE_CAMERA;

	return true;
}

void CameraController::ChangeCamera(const CameraType::Type type)
{
	if (mCurType == type)
		return;

	if (mCurType != CameraType::TYPE_END)
		mCameraList[mCurType]->InActive(this);
	
	mCurType = type;
	mCameraList[mCurType]->Active(this);
}

Camera * CameraController::GetCamera(const CameraType::Type type)
{
	return mCameraList[mCurType];
}

void CameraController::Update(const _double deltaTime)
{
	if (CameraType::TYPE_END == mCurType)
		return;

	mCameraList[mCurType]->Update(deltaTime);

}

void CameraController::LateUpdate(const _double deltaTime)
{
	if (CameraType::TYPE_END == mCurType)
		return;

	mCameraList[mCurType]->LateUpdate(deltaTime);
}

void CameraController::Free()
{
	for(auto& camera : mCameraList)
		SafeRelease(camera);
	mCameraList.fill(nullptr);
}
