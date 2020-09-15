#include "stdafx.h"
#include "HierarchyLoader_Object.h"

#include <fstream>


HierarchyLoader_Object::HierarchyLoader_Object(LPDIRECT3DDEVICE9 graphicDevice)
	: mGraphicDevice(graphicDevice)
{
	SafeAddRef(mGraphicDevice);
}

HRESULT HierarchyLoader_Object::CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame)
{
	D3DXFRAME_DERIVED*		pFrame = new D3DXFRAME_DERIVED;
	ZeroMemory(pFrame, sizeof(D3DXFRAME_DERIVED));

	if (false == SetUp_Name(&pFrame->Name, Name))
		return E_FAIL;

	D3DXMatrixIdentity(&pFrame->TransformationMatrix);
	D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);

	*ppNewFrame = pFrame;
	return S_OK;
}

HRESULT HierarchyLoader_Object::CreateMeshContainer(LPCSTR Name, CONST D3DXMESHDATA * pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer)
{
	D3DXMESHCONTAINER_DERIVED*	pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	ZeroMemory(pMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	if (false == SetUp_Name(&pMeshContainer->Name, Name))
		return E_FAIL;

	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
	
	LPD3DXMESH		pMesh = pMeshData->pMesh;
	SafeAddRef(pMesh);

	pMeshContainer->pAdjacency = new _ulong[pMesh->GetNumFaces() * 3];
	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * pMesh->GetNumFaces() * 3);
	
	_ulong dwFVF = pMesh->GetFVF();

	if (false == (dwFVF & D3DFVF_NORMAL))
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF | D3DFVF_NORMAL, mGraphicDevice, &pMeshContainer->MeshData.pMesh);

		D3DXComputeNormals(pMeshContainer->MeshData.pMesh, pMeshContainer->pAdjacency);
	}
	else
	{
		pMeshContainer->MeshData.pMesh = pMesh;
		SafeAddRef(pMeshContainer->MeshData.pMesh);
	}

	SafeRelease(pMesh);

	pMeshContainer->pOriginalMesh = pMesh;	// 추가
	SafeAddRef(pMeshContainer->pOriginalMesh);	// 추가

	pMeshContainer->NumMaterials = NumMaterials;

	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * pMeshContainer->NumMaterials);

	pMeshContainer->pMeshTexture = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
	ZeroMemory(pMeshContainer->pMeshTexture, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);

	_tchar		szFullPath[MAX_PATH] = L"";

	for (size_t i = 0; i < pMeshContainer->NumMaterials; ++i)
	{
		lstrcpy(szFullPath, mFilePath);

		_tchar	szTextureFileName[MAX_PATH] = L"";

		MultiByteToWideChar(CP_ACP, 0, pMeshContainer->pMaterials[i].pTextureFilename, (int)strlen(pMeshContainer->pMaterials[i].pTextureFilename)
			, szTextureFileName, MAX_PATH);

		lstrcat(szFullPath, szTextureFileName);

		if (FAILED(D3DXCreateTextureFromFile(mGraphicDevice, szFullPath, &pMeshContainer->pMeshTexture[i])))
			return E_FAIL;
	}

	pMeshContainer->pSkinInfo = pSkinInfo;
	SafeAddRef(pMeshContainer->pSkinInfo);

	pMeshContainer->dwNumBones = pMeshContainer->pSkinInfo->GetNumBones();

	pMeshContainer->pOffsetMatrices = new D3DXMATRIX[pMeshContainer->dwNumBones];
	::ZeroMemory(pMeshContainer->pOffsetMatrices, sizeof(D3DXMATRIX) * pMeshContainer->dwNumBones);

	pMeshContainer->ppCombinedTransformationMatrices = new D3DXMATRIX*[pMeshContainer->dwNumBones];
	::ZeroMemory(pMeshContainer->ppCombinedTransformationMatrices, sizeof(D3DXMATRIX*) * pMeshContainer->dwNumBones);

	pMeshContainer->pRenderMatrices = new D3DXMATRIX[pMeshContainer->dwNumBones];
	::ZeroMemory(pMeshContainer->pRenderMatrices, sizeof(D3DXMATRIX) * pMeshContainer->dwNumBones);

	for (_ulong i = 0; i < pMeshContainer->dwNumBones; i++)
		pMeshContainer->pOffsetMatrices[i] = *pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i);

	//////////////////////////////////////////////////////////////////////////
	//
	//	Hardware Skinning
	//
	//////////////////////////////////////////////////////////////////////////
	SafeRelease(pMeshContainer->MeshData.pMesh);
	pMeshContainer->MeshData.pMesh = nullptr;
	
	pMeshContainer->dwNumPaletteEntries = min(26, pMeshContainer->dwNumBones);
		
	HRESULT result = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh(
		pMeshContainer->pOriginalMesh, 
		D3DXMESHOPT_VERTEXCACHE | D3DXMESH_MANAGED,
		pMeshContainer->dwNumPaletteEntries, 
		pMeshContainer->pAdjacency, 
		NULL, NULL, NULL,
		&pMeshContainer->dwNumInfl, 
		&pMeshContainer->dwNumAttributeGroups,
		&pMeshContainer->pBoneCombinationBuf, 
		&pMeshContainer->MeshData.pMesh);
	
	// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
	DWORD newFVF = (pMeshContainer->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL |
		D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;

	if (newFVF != pMeshContainer->MeshData.pMesh->GetFVF())
	{
		LPD3DXMESH pMesh = nullptr;
		result = pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), newFVF,
			mGraphicDevice, &pMesh);
		if (!FAILED(result))
		{
			SafeRelease(pMeshContainer->MeshData.pMesh);
			pMeshContainer->MeshData.pMesh = pMesh;
			pMesh = nullptr;
		}
	}

	D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
	result = pMeshContainer->MeshData.pMesh->GetDeclaration(pDecl);
	
	LPD3DVERTEXELEMENT9 pDeclCur = pDecl;

	while (0xff != pDeclCur->Stream)
	{
		if ((D3DDECLUSAGE_BLENDINDICES == pDeclCur->Usage) && (0 == pDeclCur->UsageIndex))
			pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
		++pDeclCur;
	}

	result = pMeshContainer->MeshData.pMesh->UpdateSemantics(pDecl);
	
	//////////////////////////////////////////////////////////////////////////
	//
	// ~ Hardware Skinning
	//
	//////////////////////////////////////////////////////////////////////////
	
	pMeshContainer->pOriginalMesh->CloneMeshFVF(pMeshContainer->pOriginalMesh->GetOptions(), pMeshContainer->pOriginalMesh->GetFVF(), mGraphicDevice, &pMeshContainer->pSoftwareMesh);

	*ppNewMeshContainer = pMeshContainer;

	return S_OK;
}

HRESULT HierarchyLoader_Object::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	SafeDeleteArray(pFrameToFree->Name);

	if (nullptr != pFrameToFree->pMeshContainer)
		DestroyMeshContainer(pFrameToFree->pMeshContainer);

	if (pFrameToFree->pFrameSibling)
		DestroyFrame(pFrameToFree->pFrameSibling);

	if (pFrameToFree->pFrameFirstChild)
		DestroyFrame(pFrameToFree->pFrameFirstChild);

	SafeDelete(pFrameToFree);

	return S_OK;
}

HRESULT HierarchyLoader_Object::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	D3DXMESHCONTAINER_DERIVED*		pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerToFree;

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

	SafeDeleteArray(pMeshContainer);
	return S_OK;
}

_bool HierarchyLoader_Object::Initialize(const _tchar * filePath)
{
	//fopen_s(&pFile2, "adja", "w");
	mFilePath = filePath;
	return true;
}

_bool HierarchyLoader_Object::SetUp_Name(char ** ppSourName, const char * pDestName)
{
	if (nullptr == pDestName)
		return true;

	_size_t length = strlen(pDestName);

	*ppSourName = new char[length + 1];

	memcpy(*ppSourName, pDestName, length + 1);

	return true;
}

HierarchyLoader_Object * HierarchyLoader_Object::Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath)
{
	HierarchyLoader_Object*	pInstance = new HierarchyLoader_Object(graphicDevice);
	if (false == pInstance->Initialize(filePath))
	{
		MSG_BOX("Failed To Create HierarchyLoader Instance");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void HierarchyLoader_Object::Free()
{
	mFilePath = nullptr;
	SafeRelease(mGraphicDevice);
}
