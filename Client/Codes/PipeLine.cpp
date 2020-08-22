#include "stdafx.h"
#include "PipeLine.h"


IMPLEMENT_SINGLETON(PipeLine)

PipeLine::PipeLine()
{
}

_bool PipeLine::Ready(LPDIRECT3DDEVICE9 graphicDevice)
{
	::ZeroMemory(&mViewPort, sizeof(D3DVIEWPORT9));
	graphicDevice->GetViewport(&mViewPort);

	return true;
}

HRESULT PipeLine::SetTransform(D3DTRANSFORMSTATETYPE eType, const _matrix & Matrix)
{
	switch (eType)
	{
	case D3DTS_VIEW:
		mViewMatrix = Matrix;
		D3DXMatrixInverse(&mViewInverseMatrix, nullptr, &mViewMatrix);
		break;
	case D3DTS_PROJECTION:
		mProjectionMatrix = Matrix;
		D3DXMatrixInverse(&mProjectionInverseMatirx, nullptr, &mProjectionMatrix);
		break;
	}

	return NOERROR;
}

_matrix PipeLine::GetTransform(D3DTRANSFORMSTATETYPE eType) const
{
	_matrix		ResultMatrix;

	switch (eType)
	{
	case D3DTS_VIEW:
		ResultMatrix = mViewMatrix;
		break;
	case D3DTS_PROJECTION:
		ResultMatrix = mProjectionMatrix;
		break;
	default:
		MSG_BOX("GetTransform Error");
		break;
	}

	return ResultMatrix;
}

PipeLine::RayCast PipeLine::ConvertScreenPosToWorldRayCast(const _vec2& screenPos)
{
	//	Clip Space
	const _vec3 clipPos = { screenPos.x / (mViewPort.Width * 0.5f) - 1.f, screenPos.y / (mViewPort.Height * -0.5f) + 1.f, 0.f };

	//	View Space
	_vec3 viewPos = *D3DXVec3TransformCoord(&viewPos, &clipPos, &mProjectionInverseMatirx);
	
	_vec3 rayPos(0.f, 0.f, 0.f);
	_vec3 rayDir(viewPos - rayPos);
	D3DXVec3Normalize(&rayDir, &rayDir);

	//	World Space
	D3DXVec3TransformCoord(&rayPos, &rayPos, &mViewInverseMatrix);
	D3DXVec3TransformNormal(&rayDir, &rayDir, &mViewInverseMatrix);

	return RayCast(rayPos, rayDir);
}

void PipeLine::Free()
{
}
