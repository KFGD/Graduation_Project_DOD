#include "stdafx.h"
#include "StaticMesh_Object.h"

#include "Shader.h"

StaticMesh_Object::StaticMesh_Object()
{
}

StaticMesh_Object::StaticMesh_Object(const StaticMesh_Object & rhs)
	: Component_Object(rhs)
	, mMesh(rhs.mMesh)
	, mAdjacencyBuff(rhs.mAdjacencyBuff)
	, mMaterialBuff(rhs.mMaterialBuff)
	, mNumMaterials(rhs.mNumMaterials)
	, mTextures(rhs.mTextures)
	, mIsClone(true)
{
	SafeAddRef(mMesh);
	SafeAddRef(mAdjacencyBuff);
	SafeAddRef(mMaterialBuff);

	mMaterials = (D3DXMATERIAL*)mMaterialBuff->GetBufferPointer();

	for (_size_t i = 0; i < mNumMaterials; ++i)
		SafeAddRef(mTextures[i]);
}

void StaticMesh_Object::Render(Shader * shader)
{
	shader->BeginShader(nullptr);
	shader->BeginPass(0);
	
	shader->EndPass();
	shader->EndShader();
}

_bool StaticMesh_Object::GetVertexBuffer(LPDIRECT3DVERTEXBUFFER9 & vertexBuffer)
{
	const HRESULT hr = mMesh->GetVertexBuffer(&vertexBuffer);
	return FAILED(hr);
}

_bool StaticMesh_Object::GetIndexBuffer(LPDIRECT3DINDEXBUFFER9 & indexBuffer)
{
	const HRESULT hr = mMesh->GetIndexBuffer(&indexBuffer);
	return FAILED(hr);
}

_ulong StaticMesh_Object::GetVertexSize() const
{
	return mMesh->GetNumBytesPerVertex();
}

_ulong StaticMesh_Object::GetVertexNum() const
{
	return mMesh->GetNumVertices();
}

_ulong StaticMesh_Object::GetFacesNum() const
{
	return mMesh->GetNumFaces();
}

LPDIRECT3DTEXTURE9 StaticMesh_Object::GetTexutre(const _ulong index)
{
	return mTextures[index];
}

_bool StaticMesh_Object::Initialize(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix& pivotMatrix)
{
	_tchar fullPath[MAX_PATH] = L"";
	lstrcpy(fullPath, filePath);
	lstrcat(fullPath, fileName);

	LPD3DXMESH	pMesh = nullptr;

	HRESULT hr = 0;
	hr = D3DXLoadMeshFromX(fullPath, D3DXMESH_MANAGED, graphicDevice, &mAdjacencyBuff, &mMaterialBuff, nullptr, &mNumMaterials, &pMesh);
	if (FAILED(hr))
		return false;

	mMaterials = (D3DXMATERIAL*)mMaterialBuff->GetBufferPointer();

	mTextures = new LPDIRECT3DTEXTURE9[mNumMaterials];
	::ZeroMemory(mTextures, sizeof(LPDIRECT3DTEXTURE9) * mNumMaterials);

	for (_size_t i = 0; i < mNumMaterials; ++i)
	{
		lstrcpy(fullPath, filePath);

		_tchar textureFileName[MAX_PATH] = L"";
		MultiByteToWideChar(CP_ACP, 0, mMaterials[i].pTextureFilename, (_int)strlen(mMaterials[i].pTextureFilename), textureFileName, MAX_PATH);

		lstrcat(fullPath, textureFileName);

		hr = D3DXCreateTextureFromFile(graphicDevice, fullPath, &mTextures[i]);
		if (FAILED(hr))
			return false;
	}

	_ulong fvf = pMesh->GetFVF();

	if (0 == (fvf & D3DFVF_NORMAL))
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), fvf | D3DFVF_NORMAL, graphicDevice, &mMesh);
		D3DXComputeNormals(mMesh, (_ulong*)mAdjacencyBuff->GetBufferPointer());
	}
	else
	{
		SafeAddRef(pMesh);
		mMesh = pMesh;
	}

	SafeRelease(pMesh);


	mPivotMatrix = pivotMatrix;
	
	D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
	mMesh->GetDeclaration(decl);

	_ushort normalOffset = 0;
	for (_size_t i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (D3DDECLUSAGE_NORMAL == decl[i].Usage)
		{
			normalOffset = decl[i].Offset;
			break;
		}
	}

	const _ulong numVertices = mMesh->GetNumVertices();
	const _ulong stride = D3DXGetFVFVertexSize(mMesh->GetFVF());

	_byte*	vertices = nullptr;
	mMesh->LockVertexBuffer(0, (void**)&vertices);

	for (_ulong i = 0; i < numVertices; ++i)
	{
		_vec3* pos = (_vec3*)(vertices + (i * stride));
		_vec3* normal = (_vec3*)(vertices + (i * stride) + normalOffset);
		D3DXVec3TransformCoord(pos, pos, &mPivotMatrix);
		D3DXVec3TransformNormal(normal, normal, &mPivotMatrix);
	}

	mMesh->UnlockVertexBuffer();


	return true;
}

StaticMesh_Object * StaticMesh_Object::Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix& pivotMatrix)
{
	StaticMesh_Object*	pInstance = new StaticMesh_Object();
	if (false == pInstance->Initialize(graphicDevice, filePath, fileName, pivotMatrix))
	{
		MSG_BOX("Failed To Create StaticMesh_Object Instance");
		SafeRelease(pInstance);
	}

	return pInstance;
}

Component_Object * StaticMesh_Object::Clone(void * arg)
{
	return new StaticMesh_Object(*this);
}

void StaticMesh_Object::Free()
{
	for (_size_t i = 0; i < mNumMaterials; ++i)
		SafeRelease(mTextures[i]);

	if (false == mIsClone)
		SafeDeleteArray(mTextures);

	SafeRelease(mAdjacencyBuff);
	SafeRelease(mMaterialBuff);
	SafeRelease(mMesh);

	Component_Object::Free();
}
