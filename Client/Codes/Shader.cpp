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
		NULL,	// ���̴� ��� �� ������ ��ũ������
		NULL,	// ���̴� ��� �� ������ �������
		D3DXSHADER_DEBUG, // ���̴� ����� �ɼ� �÷���(�ܺζ��̺귯�� ������ �����ؾ������� ���� ����ؼ� �ɼ� ����)
		NULL,	// ���� ���ҽ� ������ ��� ������ ���ΰ�(null�� �־��ָ� �⺻ ������ ��ġ�� ���� : �׷��� ī��)
		&mEffect,
		&m_pErrMsg)))	// ������� �Ұ��������� ���� �� ��� �߻����� ��� ���ڿ� ���·� ���� ��Ȳ�� ������ �� �ִ�.
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