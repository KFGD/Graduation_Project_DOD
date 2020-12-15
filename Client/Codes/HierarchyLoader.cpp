#include "stdafx.h"
#include "HierarchyLoader.h"

HierarchyLoader::HierarchyLoader(LPDIRECT3DDEVICE9 graphicDevice)
	: mGraphicDevice(graphicDevice)
{
	SafeAddRef(mGraphicDevice);
}

HRESULT HierarchyLoader::CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame)
{
	//	준비 단계 처리
	if (!mIsMeasured)
	{
		mFrameCount += 1;
		*ppNewFrame = new D3DXFRAME;
		return S_OK;
	}
	
	//	생성 단계 처리
	{
		//	미리 확보한 메모리 공간에 뼈대 정보 기록
		D3DXFRAME_DERIVED& frame = mFrameList[mFrameIdx];
		::ZeroMemory(&frame, sizeof(D3DXFRAME_DERIVED));

		//	뼈대 정보 초기화
		{
			if (false == SetUp_Name(&frame.Name, Name))
				return E_FAIL;
			D3DXMatrixIdentity(&frame.TransformationMatrix);
			D3DXMatrixIdentity(&frame.CombinedTransformationMatrix);

			*ppNewFrame = &frame;
			mFrameIdx += 1;
		}
	}

	return S_OK;
}

HRESULT HierarchyLoader::CreateMeshContainer(LPCSTR Name, CONST D3DXMESHDATA * pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer)
{
	if (!mIsMeasured)
	{
		mMeshContainerCount += 1;
		*ppNewMeshContainer = new D3DXMESHCONTAINER;
		return S_OK;
	}

	D3DXMESHCONTAINER_DERIVED& meshContainer = mMeshContainerList[mMeshContainerIdx];
	::ZeroMemory(&meshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	if (false == SetUp_Name(&meshContainer.Name, Name))
		return E_FAIL;

	meshContainer.MeshData.Type = D3DXMESHTYPE_MESH;

	LPD3DXMESH		pMesh = pMeshData->pMesh;
	SafeAddRef(pMesh);

	meshContainer.pAdjacency = new _ulong[pMesh->GetNumFaces() * 3];
	memcpy(meshContainer.pAdjacency, pAdjacency, sizeof(_ulong) * pMesh->GetNumFaces() * 3);

	_ulong dwFVF = pMesh->GetFVF();

	if (false == (dwFVF & D3DFVF_NORMAL))
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF | D3DFVF_NORMAL, mGraphicDevice, &meshContainer.MeshData.pMesh);

		D3DXComputeNormals(meshContainer.MeshData.pMesh, meshContainer.pAdjacency);
	}
	else
	{
		meshContainer.MeshData.pMesh = pMesh;
		SafeAddRef(meshContainer.MeshData.pMesh);
	}

	SafeRelease(pMesh);

	meshContainer.pOriginalMesh = pMesh;	// 추가
	SafeAddRef(meshContainer.pOriginalMesh);	// 추가

	meshContainer.NumMaterials = NumMaterials;

	meshContainer.pMaterials = new D3DXMATERIAL[meshContainer.NumMaterials];
	memcpy(meshContainer.pMaterials, pMaterials, sizeof(D3DXMATERIAL) * meshContainer.NumMaterials);

	meshContainer.pMeshTexture = new LPDIRECT3DTEXTURE9[meshContainer.NumMaterials];
	ZeroMemory(meshContainer.pMeshTexture, sizeof(LPDIRECT3DTEXTURE9) * meshContainer.NumMaterials);

	_tchar		szFullPath[MAX_PATH] = L"";

	for (size_t i = 0; i < meshContainer.NumMaterials; ++i)
	{
		lstrcpy(szFullPath, mFilePath);

		_tchar	szTextureFileName[MAX_PATH] = L"";

		MultiByteToWideChar(CP_ACP, 0, meshContainer.pMaterials[i].pTextureFilename, (int)strlen(meshContainer.pMaterials[i].pTextureFilename)
			, szTextureFileName, MAX_PATH);

		lstrcat(szFullPath, szTextureFileName);

		if (FAILED(D3DXCreateTextureFromFile(mGraphicDevice, szFullPath, &meshContainer.pMeshTexture[i])))
			return E_FAIL;
	}

	meshContainer.pSkinInfo = pSkinInfo;
	SafeAddRef(meshContainer.pSkinInfo);

	meshContainer.dwNumBones = meshContainer.pSkinInfo->GetNumBones();

	meshContainer.pOffsetMatrices = new D3DXMATRIX[meshContainer.dwNumBones];
	::ZeroMemory(meshContainer.pOffsetMatrices, sizeof(D3DXMATRIX) * meshContainer.dwNumBones);

	meshContainer.ppCombinedTransformationMatrices = new D3DXMATRIX*[meshContainer.dwNumBones];
	::ZeroMemory(meshContainer.ppCombinedTransformationMatrices, sizeof(D3DXMATRIX*) * meshContainer.dwNumBones);

	meshContainer.pRenderMatrices = new D3DXMATRIX[meshContainer.dwNumBones];
	::ZeroMemory(meshContainer.pRenderMatrices, sizeof(D3DXMATRIX) * meshContainer.dwNumBones);

	for (_ulong i = 0; i < meshContainer.dwNumBones; i++)
		meshContainer.pOffsetMatrices[i] = *meshContainer.pSkinInfo->GetBoneOffsetMatrix(i);

	//////////////////////////////////////////////////////////////////////////
	//
	//	Hardware Skinning
	//
	//////////////////////////////////////////////////////////////////////////
	SafeRelease(meshContainer.MeshData.pMesh);
	meshContainer.MeshData.pMesh = nullptr;

	meshContainer.dwNumPaletteEntries = min(26, meshContainer.dwNumBones);

	HRESULT result = meshContainer.pSkinInfo->ConvertToIndexedBlendedMesh(
		meshContainer.pOriginalMesh,
		D3DXMESHOPT_VERTEXCACHE | D3DXMESH_MANAGED,
		meshContainer.dwNumPaletteEntries,
		meshContainer.pAdjacency,
		NULL, NULL, NULL,
		&meshContainer.dwNumInfl,
		&meshContainer.dwNumAttributeGroups,
		&meshContainer.pBoneCombinationBuf,
		&meshContainer.MeshData.pMesh);

	// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
	DWORD newFVF = (meshContainer.MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL |
		D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;

	if (newFVF != meshContainer.MeshData.pMesh->GetFVF())
	{
		LPD3DXMESH pMesh = nullptr;
		result = meshContainer.MeshData.pMesh->CloneMeshFVF(meshContainer.MeshData.pMesh->GetOptions(), newFVF,
			mGraphicDevice, &pMesh);
		if (!FAILED(result))
		{
			SafeRelease(meshContainer.MeshData.pMesh);
			meshContainer.MeshData.pMesh = pMesh;
			pMesh = nullptr;
		}
	}

	D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
	result = meshContainer.MeshData.pMesh->GetDeclaration(pDecl);

	LPD3DVERTEXELEMENT9 pDeclCur = pDecl;

	while (0xff != pDeclCur->Stream)
	{
		if ((D3DDECLUSAGE_BLENDINDICES == pDeclCur->Usage) && (0 == pDeclCur->UsageIndex))
			pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
		++pDeclCur;
	}

	result = meshContainer.MeshData.pMesh->UpdateSemantics(pDecl);

	//////////////////////////////////////////////////////////////////////////
	//
	// ~ Hardware Skinning
	//
	//////////////////////////////////////////////////////////////////////////

	meshContainer.pOriginalMesh->CloneMeshFVF(meshContainer.pOriginalMesh->GetOptions(), meshContainer.pOriginalMesh->GetFVF(), mGraphicDevice, &meshContainer.pSoftwareMesh);

	*ppNewMeshContainer = &meshContainer;
	mMeshContainerIdx += 1;

	return S_OK;
}

HRESULT HierarchyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	if (nullptr != pFrameToFree->pMeshContainer)
		DestroyMeshContainer(pFrameToFree->pMeshContainer);

	if (pFrameToFree->pFrameSibling)
		DestroyFrame(pFrameToFree->pFrameSibling);

	if (pFrameToFree->pFrameFirstChild)
		DestroyFrame(pFrameToFree->pFrameFirstChild);

	if (!mIsMeasured)
		SafeDelete(pFrameToFree);
	else
		SafeDeleteArray(pFrameToFree->Name);

	return S_OK;
}

HRESULT HierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	if (!mIsMeasured)
	{
		SafeDelete(pMeshContainerToFree);
		return S_OK;
	}

	D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerToFree;

	SafeDeleteArray(pMeshContainer->Name);
	SafeRelease(pMeshContainer->MeshData.pMesh);
	SafeDeleteArray(pMeshContainer->pMaterials);
	SafeDeleteArray(pMeshContainer->pAdjacency);
	SafeRelease(pMeshContainer->pSkinInfo);
	SafeRelease(pMeshContainer->pOriginalMesh);

	for (size_t i = 0; i < pMeshContainer->NumMaterials; ++i)
	{
		SafeRelease(pMeshContainer->pMeshTexture[i]);
	}

	SafeDeleteArray(pMeshContainer->pMeshTexture);
	SafeDeleteArray(pMeshContainer->pOffsetMatrices);
	SafeDeleteArray(pMeshContainer->ppCombinedTransformationMatrices);
	SafeDeleteArray(pMeshContainer->pRenderMatrices);

	///////////////////////////
	//	Hardware Skinning
	///////////////////////////

	//SafeDeleteArray(pMeshContainer->pAttributeTable);
	SafeRelease(pMeshContainer->pBoneCombinationBuf);

	///////////////////////////
	//	~Hardware Skinning
	///////////////////////////

	SafeRelease(pMeshContainer->pSoftwareMesh);

	return S_OK;
}

void HierarchyLoader::SetMeasured(const _bool isMeasured)
{
	mIsMeasured = isMeasured;
	
	if (mIsMeasured)
	{
		mFrameIdx = 0;
		mMeshContainerIdx = 0;

		//	뼈대 정보를 저장할 메모리 공간 확보
		mFrameList.resize(mFrameCount);
		mMeshContainerList.resize(mMeshContainerCount);
	}
}

_bool HierarchyLoader::Initialize(const _tchar * filePath)
{
	mFilePath = filePath;
	return true;
}

_bool HierarchyLoader::SetUp_Name(char ** srcName, const char * destName)
{
	if (nullptr == destName)
		return true;

	_size_t length = strlen(destName);

	*srcName = new char[length + 1];

	memcpy(*srcName, destName, length + 1);

	return true;
}

HierarchyLoader * HierarchyLoader::Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath)
{
	HierarchyLoader*	pInstance = new HierarchyLoader(graphicDevice);
	if (false == pInstance->Initialize(filePath))
	{
		MSG_BOX("Failed To Create HiearchyLoader Instance");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void HierarchyLoader::Free()
{
	SafeRelease(mGraphicDevice);
}

