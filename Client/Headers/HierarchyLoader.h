#pragma once
#include "Base.h"

#include "Defines.h"

class HierarchyLoader final :
	public Base, public ID3DXAllocateHierarchy
{
public:
	HierarchyLoader(LPDIRECT3DDEVICE9 graphicDevice);
	virtual ~HierarchyLoader() = default;

public:
	virtual HRESULT CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame) override;
	virtual HRESULT CreateMeshContainer(LPCSTR Name, CONST D3DXMESHDATA * pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer) override;
	virtual HRESULT DestroyFrame(LPD3DXFRAME pFrameToFree) override;
	virtual HRESULT DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree) override;

public:
	_bool	Initialize(const _tchar* filePath);

private:
	_bool SetUp_Name(char** ppSourName, const char* pDestName);

private:
	LPDIRECT3DDEVICE9 mGraphicDevice = nullptr;
	const _tchar* mFilePath = nullptr;

public:
	static HierarchyLoader*	Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar* filePath);
	virtual void	Free() override;
};

