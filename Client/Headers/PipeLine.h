#pragma once
#include "Base.h"

#include "Defines.h"

class PipeLine :
	public Base
{
public:
	struct RayCast
	{
		//	World Space
		_vec3	RayPos;
		_vec3	RayDir;

		RayCast(const _vec3& rayPos = _vec3(0.f, 0.f, 0.f), const _vec3& rayDir = _vec3(0.f, 0.f, 0.f))
			: RayPos(rayPos), RayDir(rayDir)
		{}
	};

	DECLARE_SINGLETON(PipeLine)
private:
	explicit PipeLine();
	virtual ~PipeLine() = default;
public:
	_bool	Ready(LPDIRECT3DDEVICE9 graphicDevice);
	HRESULT SetTransform(D3DTRANSFORMSTATETYPE eType, const _matrix& Matrix);
	_matrix GetTransform(D3DTRANSFORMSTATETYPE eType) const;
	_vec3 Get_Eye() {
		return *(_vec3*)&mViewInverseMatrix.m[3][0];
	}
	const _matrix& Get_ViewInverseMatrix() const { return mViewInverseMatrix; }
	PipeLine::RayCast	ConvertScreenPosToWorldRayCast(const _vec2& screenPos);
private:
	D3DVIEWPORT9	mViewPort;
	_matrix		mViewMatrix;
	_matrix		mViewInverseMatrix;
	_matrix		mProjectionMatrix;
	_matrix		mProjectionInverseMatirx;
public:
	virtual void Free();
};

