#pragma once
#include "Base.h"

#include "Defines.h"
class StaticMesh :
	public Base
{
private:
	explicit StaticMesh();
	explicit StaticMesh(const StaticMesh& rhs) = delete;
	virtual ~StaticMesh() = default;

public:
	LPD3DXMESH				GetMesh() { return mMesh; }
	_bool					GetVertexBuffer(LPDIRECT3DVERTEXBUFFER9& vertexBuffer);
	LPDIRECT3DVERTEXBUFFER9	GetVertexBuffer() { return mVertexBuffer; }
	_bool					GetIndexBuffer(LPDIRECT3DINDEXBUFFER9& indexBuffer);
	LPDIRECT3DINDEXBUFFER9	GetIndexBuffer() { return mIndexBuffer; }
	_ulong					GetVertexSize() const;
	_ulong					GetVertexNum() const;
	_ulong					GetFacesNum() const;

private:
	_bool	Initialize(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix& pivotMatrix);

private:
	LPD3DXMESH			mMesh = nullptr;
	LPD3DXBUFFER		mAdjacencyBuff = nullptr;
	LPD3DXBUFFER		mMaterialBuff = nullptr;
	_ulong				mNumMaterials = 0;
	D3DXMATERIAL*		mMaterials = nullptr;
	LPDIRECT3DTEXTURE9*	mTextures = nullptr;

private:
	LPDIRECT3DVERTEXBUFFER9	mVertexBuffer = nullptr;
	LPDIRECT3DINDEXBUFFER9	mIndexBuffer = nullptr;

public:
	static	StaticMesh*	Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix& pivotMatrix);
	virtual	void	Free()	override;

};

