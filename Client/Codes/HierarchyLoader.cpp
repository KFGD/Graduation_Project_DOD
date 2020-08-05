#include "stdafx.h"
#include "HierarchyLoader.h"


HierarchyLoader::HierarchyLoader(LPDIRECT3DDEVICE9 graphicDevice)
	: mGraphicDevice(graphicDevice)
{
	SafeAddRef(mGraphicDevice);
}

HRESULT HierarchyLoader::CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame)
{
	D3DXFRAME_DERIVED*		pFrame = new D3DXFRAME_DERIVED;
	ZeroMemory(pFrame, sizeof(D3DXFRAME_DERIVED));

	if (false == SetUp_Name(&pFrame->Name, Name))
		return E_FAIL;

	pFrame->CombinedTransformationMatrix = *D3DXMatrixIdentity(&pFrame->TransformationMatrix);

	*ppNewFrame = pFrame;
	return S_OK;
}

HRESULT HierarchyLoader::CreateMeshContainer(LPCSTR Name, CONST D3DXMESHDATA * pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer)
{
	//D3DXMESHCONTAINER_DERIVED*	pNewMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	//ZeroMemory(pNewMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	//pNewMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	//LPD3DXMESH	pMesh = pMeshData->pMesh;

	//_ulong dwNumFaces = pMesh->GetNumFaces();	// 폴리곤의 개수를 의미

	//pNewMeshContainer->pAdjacency = new _ulong[dwNumFaces * 3]; // 인접한 폴리곤의 개수들까지 메모리 할당을 해놓는다.
	//memcpy(pNewMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * dwNumFaces * 3);

	//_ulong	dwFVF = pMesh->GetFVF();

	//if (!(dwFVF & D3DFVF_NORMAL))
	//{
	//	pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF | D3DFVF_NORMAL, mGraphicDevice, &pNewMeshContainer->MeshData.pMesh);
	//	D3DXComputeNormals(pNewMeshContainer->MeshData.pMesh, pNewMeshContainer->pAdjacency);
	//}
	//else
	//{
	//	pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF, mGraphicDevice, &pNewMeshContainer->MeshData.pMesh);
	//}

	//pNewMeshContainer->NumMaterials = (NumMaterials == 0 ? 1 : NumMaterials);

	//pNewMeshContainer->pMaterials = new D3DXMATERIAL[pNewMeshContainer->NumMaterials];
	//ZeroMemory(pNewMeshContainer->pMaterials, sizeof(D3DXMATERIAL) * pNewMeshContainer->NumMaterials);

	//pNewMeshContainer->pMeshTexture = new LPDIRECT3DTEXTURE9[pNewMeshContainer->NumMaterials];
	//ZeroMemory(pNewMeshContainer->pMeshTexture, sizeof(LPDIRECT3DTEXTURE9) * pNewMeshContainer->NumMaterials);

	//if (0 != NumMaterials)
	//{
	//	memcpy(pNewMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * pNewMeshContainer->NumMaterials);

	//	for (_ulong i = 0; i < pNewMeshContainer->NumMaterials; ++i)
	//	{
	//		_tchar		szFullPath[128] = L"";
	//		_tchar		szFileName[128] = L"";

	//		MultiByteToWideChar(CP_ACP,
	//			0,
	//			pNewMeshContainer->pMaterials[i].pTextureFilename,
	//			(_int)strlen(pNewMeshContainer->pMaterials[i].pTextureFilename),
	//			szFileName,
	//			128);

	//		lstrcpy(szFullPath, mFilePath);
	//		lstrcat(szFullPath, szFileName);

	//		D3DXCreateTextureFromFile(mGraphicDevice, szFullPath, &pNewMeshContainer->pMeshTexture[i]);
	//	}
	//}
	//else
	//{
	//	pNewMeshContainer->pMaterials[0].MatD3D.Diffuse = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
	//	pNewMeshContainer->pMaterials[0].MatD3D.Specular = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
	//	pNewMeshContainer->pMaterials[0].MatD3D.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
	//	pNewMeshContainer->pMaterials[0].MatD3D.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
	//	pNewMeshContainer->pMaterials[0].MatD3D.Power = 0.f;

	//	pNewMeshContainer->pMeshTexture = nullptr;
	//}

	//if (nullptr == pSkinInfo)
	//	return S_OK;

	//pNewMeshContainer->pSkinInfo = pSkinInfo;
	//pNewMeshContainer->pSkinInfo->AddRef();

	//pNewMeshContainer->MeshData.pMesh->CloneMeshFVF(pNewMeshContainer->MeshData.pMesh->GetOptions(),
	//	pNewMeshContainer->MeshData.pMesh->GetFVF(),
	//	mGraphicDevice,
	//	&pNewMeshContainer->pOriginalMesh);

	//pNewMeshContainer->dwNumBones = pNewMeshContainer->pSkinInfo->GetNumBones(); // 뼈의 개수를 반환


	//pNewMeshContainer->pOffsetMatrices = new _matrix[pNewMeshContainer->dwNumBones];
	//ZeroMemory(pNewMeshContainer->pOffsetMatrices, sizeof(_matrix) * pNewMeshContainer->dwNumBones);


	//pNewMeshContainer->ppCombinedTransformationMatrices = new _matrix*[pNewMeshContainer->dwNumBones];
	//ZeroMemory(pNewMeshContainer->ppCombinedTransformationMatrices, sizeof(_matrix*) * pNewMeshContainer->dwNumBones);


	//pNewMeshContainer->pRenderMatrices = new _matrix[pNewMeshContainer->dwNumBones];
	//ZeroMemory(pNewMeshContainer->pRenderMatrices, sizeof(_matrix) * pNewMeshContainer->dwNumBones);

	//for (_ulong i = 0; i < pNewMeshContainer->dwNumBones; ++i)
	//{
	//	pNewMeshContainer->pOffsetMatrices[i] = *pNewMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i); // 메쉬를 그리기 위한 뼈에는 인덱스 모두 있다. 이런 뼈들을 순회하면서 행렬 정보를 얻어오는 함수
	//}

	//*ppNewMeshContainer = pNewMeshContainer;

	//return S_OK;
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
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF, mGraphicDevice, &pMeshContainer->MeshData.pMesh);
	}

	SafeRelease(pMesh);

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
	pMeshContainer->ppCombinedTransformationMatrices = new D3DXMATRIX*[pMeshContainer->dwNumBones];
	pMeshContainer->pRenderMatrices = new D3DXMATRIX[pMeshContainer->dwNumBones];

	for (_ulong i = 0; i < pMeshContainer->dwNumBones; i++)
		pMeshContainer->pOffsetMatrices[i] = *pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i);

	if (FAILED(pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), pMeshContainer->MeshData.pMesh->GetFVF(), mGraphicDevice, &pMeshContainer->pOriginalMesh)))
		return E_FAIL;

	*ppNewMeshContainer = pMeshContainer;

	return S_OK;
}

HRESULT HierarchyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree)
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

HRESULT HierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
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

	SafeDeleteArray(pMeshContainer);
	return S_OK;
}

_bool HierarchyLoader::Initialize(const _tchar * filePath)
{
	mFilePath = filePath;
	return true;
}

_bool HierarchyLoader::SetUp_Name(char ** ppSourName, const char * pDestName)
{
	if (nullptr == pDestName)
		return true;

	_size_t length = strlen(pDestName);

	*ppSourName = new char[length + 1];

	memcpy(*ppSourName, pDestName, length + 1);

	return true;
}

HierarchyLoader * HierarchyLoader::Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath)
{
	HierarchyLoader*	pInstance = new HierarchyLoader(graphicDevice);
	if (false == pInstance->Initialize(filePath))
	{
		MSG_BOX("Failed To Create HierarchyLoader Instance");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void HierarchyLoader::Free()
{
	mFilePath = nullptr;
	SafeRelease(mGraphicDevice);
}
