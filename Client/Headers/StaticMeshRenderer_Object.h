#pragma once
#include "Component_Object.h"

class Shader;

class StaticMeshRenderer_Object :
	public Component_Object
{
private:
	explicit	StaticMeshRenderer_Object();
	explicit	StaticMeshRenderer_Object(const StaticMeshRenderer_Object& rhs);
	virtual		~StaticMeshRenderer_Object() = default;

public:
	void	Render(Shader* shader);

public:
	LPD3DXMESH	GetMesh() { return mMesh; }
	_bool		GetVertexBuffer(LPDIRECT3DVERTEXBUFFER9& vertexBuffer);
	LPDIRECT3DVERTEXBUFFER9	GetVertexBuffer() { return mVertexBuffer; }
	_bool		GetIndexBuffer(LPDIRECT3DINDEXBUFFER9& indexBuffer);
	LPDIRECT3DINDEXBUFFER9	GetIndexBuffer() { return mIndexBuffer; }
	_ulong		GetVertexSize() const;
	_ulong		GetVertexNum() const;
	_ulong		GetFacesNum() const;

public:
	LPDIRECT3DTEXTURE9	GetTexutre(const _ulong index);

private:
	_bool	Initialize(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix& pivotMatrix);

private:
	_bool	mIsClone = false;

private:
	LPD3DXMESH		mMesh = nullptr;
	LPD3DXBUFFER	mAdjacencyBuff = nullptr;
	LPD3DXBUFFER	mMaterialBuff = nullptr;
	_ulong			mNumMaterials = 0;
	D3DXMATERIAL*	mMaterials = nullptr;
	LPDIRECT3DTEXTURE9*	mTextures = nullptr;
	
private:
	LPDIRECT3DVERTEXBUFFER9	mVertexBuffer = nullptr;
	LPDIRECT3DINDEXBUFFER9	mIndexBuffer = nullptr;

private:
	_matrix			mPivotMatrix;

public:
	static StaticMeshRenderer_Object*	Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix& pivotMatrix);
	Component_Object*	Clone(void* arg) override;
	virtual void	Free() override;

};

