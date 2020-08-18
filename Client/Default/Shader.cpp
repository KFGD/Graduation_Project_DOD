#include "stdafx.h"
#include "Shader.h"


Shader::Shader()
{
}

Shader::Shader(const Shader & rhs)
	: mEffect(rhs.mEffect)
{
	SafeAddRef(mEffect);
}

HRESULT Shader::SetTechnique(D3DXHANDLE hTechnique)
{
	if (nullptr == mEffect)
		return E_FAIL;

	return mEffect->SetTechnique(hTechnique);
}

HRESULT Shader::SetValue(D3DXHANDLE hParameter, const void * pValue, _uint iSize)
{
	if (nullptr == mEffect)
		return E_FAIL;

	return mEffect->SetValue(hParameter, pValue, iSize);
}

HRESULT Shader::SetTexture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture)
{
	if (nullptr == mEffect)
		return E_FAIL;

	return mEffect->SetTexture(hParameter, pTexture);
}

HRESULT Shader::SetBool(D3DXHANDLE hParameter, _bool Value)
{
	if (nullptr == mEffect)
		return E_FAIL;

	return mEffect->SetBool(hParameter, Value);
}

HRESULT Shader::CommitChanges()
{
	if (nullptr == mEffect)
		return E_FAIL;

	return mEffect->CommitChanges();
}

HRESULT Shader::BeginShader(_uint * numPasses)
{
	if (nullptr == mEffect)
		return E_FAIL;

	return mEffect->Begin(numPasses, 0);
}

HRESULT Shader::BeginPass(_uint passIndex)
{
	if (nullptr == mEffect)
		return E_FAIL;

	return mEffect->BeginPass(passIndex);
}

HRESULT Shader::EndShader()
{
	if (nullptr == mEffect)
		return E_FAIL;

	return mEffect->End();
}

HRESULT Shader::EndPass()
{
	if (nullptr == mEffect)
		return E_FAIL;

	return mEffect->EndPass();
}

_bool Shader::Initialize(LPDIRECT3DDEVICE9 graphicDev, const _tchar * filePath)
{
	if (FAILED(D3DXCreateEffectFromFile(graphicDev,
		filePath,
		NULL,	// 쉐이더 사용 시 포함할 메크로파일
		NULL,	// 쉐이더 사용 시 포함할 헤더파일
		D3DXSHADER_DEBUG, // 쉐이더 디버깅 옵션 플래그(외부라이브러리 파일을 포함해야하지만 만약 대비해서 옵션 설정)
		NULL,	// 각종 리소스 정보를 어디에 저장할 것인가(null을 넣어주면 기본 값으로 장치에 저장 : 그래픽 카드)
		&mEffect,
		&m_pErrMsg)))	// 디버깅은 불가능하지만 에러 및 경고가 발생했을 경우 문자열 형태로 문제 상황을 저장할 수 있다.
		return false;

	return true;
}

Shader * Shader::Create(LPDIRECT3DDEVICE9 graphicDev, const _tchar * filePath)
{
	Shader*		pInstance = new Shader;

	if (false == pInstance->Initialize(graphicDev, filePath))
	{
		MSG_BOX("Shader Created Failed");
		SafeRelease(pInstance);
	}

	return pInstance;
}

Component_Object * Shader::Clone(void * arg)
{
	return new Shader(*this);
}

void Shader::Free()
{
	SafeRelease(mEffect);
}