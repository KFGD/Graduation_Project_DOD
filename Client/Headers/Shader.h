#pragma once
#include "Component_Object.h"

class Shader :
	public Component_Object
{
private:
	explicit Shader();
	explicit Shader(const Shader& rhs);
	virtual ~Shader() = default;

public:
	LPD3DXEFFECT Get_EffectHandle(void) {
		return mEffect;
	}

public:
	HRESULT SetTechnique(D3DXHANDLE hTechnique);
	HRESULT SetValue(D3DXHANDLE hParameter, const void* pValue, _uint iSize);
	HRESULT SetTexture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture);
	HRESULT SetBool(D3DXHANDLE hParameter, _bool Value);

public:
	HRESULT CommitChanges();
	HRESULT BeginShader(_uint* numPasses);
	HRESULT BeginPass(_uint passIndex);
	HRESULT EndShader();
	HRESULT EndPass();

private:
	_bool	Initialize(LPDIRECT3DDEVICE9 graphicDev, const _tchar* filePath);

private:
	LPD3DXEFFECT				mEffect = nullptr;
	LPD3DXBUFFER				m_pErrMsg = nullptr;

public:
	static Shader*	Create(LPDIRECT3DDEVICE9 graphicDev, const _tchar* filePath);
	virtual Component_Object * Clone(void * arg) override;
	virtual void	Free() override;

};

