#pragma once
#include "Base.h"

#include "Defines.h"

class HierarchyLoader final :
	public Base, public ID3DXAllocateHierarchy
{
private:
	explicit HierarchyLoader(LPDIRECT3DDEVICE9 graphicDevice);
	virtual ~HierarchyLoader() = default;

public:
	virtual HRESULT CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame) override;
	virtual HRESULT CreateMeshContainer(LPCSTR Name, CONST D3DXMESHDATA * pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer) override;
	virtual HRESULT DestroyFrame(LPD3DXFRAME pFrameToFree) override;
	virtual HRESULT DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree) override;

public:
	void	SetMeasured(const _bool isMeasured);

private:
	_bool	Initialize(const _tchar* filePath);
	_bool	SetUp_Name(char** srcName, const char* destName);

private:
	LPDIRECT3DDEVICE9	mGraphicDevice = nullptr;
	const _tchar* mFilePath = nullptr;

private:
	_bool	mIsMeasured = false;
	_size_t mFrameCount = 0;
	_size_t mMeshContainerCount = 0;

private:
	_int	mFrameIdx = 0;
	_int	mMeshContainerIdx = 0;
	vector<D3DXFRAME_DERIVED>			mFrameList;
	vector<D3DXMESHCONTAINER_DERIVED>	mMeshContainerList;

public:
	static HierarchyLoader*	Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar* filePath);
	virtual void Free() override;
};