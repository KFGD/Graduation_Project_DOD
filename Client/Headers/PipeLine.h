#pragma once
#include "Base.h"

#include "Defines.h"

class PipeLine :
	public Base
{
	DECLARE_SINGLETON(PipeLine)
private:
	explicit PipeLine();
	virtual ~PipeLine() = default;
public:
	HRESULT SetTransform(D3DTRANSFORMSTATETYPE eType, const _matrix& Matrix);
	_matrix GetTransform(D3DTRANSFORMSTATETYPE eType);
	_vec3 Get_Eye() {
		return *(_vec3*)&m_ViewInverseMatrix.m[3][0];
	}
	const _matrix& Get_ViewInverseMatrix() const { return m_ViewInverseMatrix; }
private:
	_matrix		mViewMatrix;
	_matrix		m_ViewInverseMatrix;
	_matrix		mProjectionMatrix;
public:
	virtual void Free();
};

