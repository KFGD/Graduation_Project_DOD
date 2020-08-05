#include "stdafx.h"
#include "PipeLine.h"


IMPLEMENT_SINGLETON(PipeLine)

PipeLine::PipeLine()
{
}

HRESULT PipeLine::SetTransform(D3DTRANSFORMSTATETYPE eType, const _matrix & Matrix)
{
	switch (eType)
	{
	case D3DTS_VIEW:
		mViewMatrix = Matrix;
		D3DXMatrixInverse(&m_ViewInverseMatrix, nullptr, &mViewMatrix);
		break;
	case D3DTS_PROJECTION:
		mProjectionMatrix = Matrix;
		break;
	}

	return NOERROR;
}

_matrix PipeLine::GetTransform(D3DTRANSFORMSTATETYPE eType)
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
	}

	return ResultMatrix;
}

void PipeLine::Free()
{
}
