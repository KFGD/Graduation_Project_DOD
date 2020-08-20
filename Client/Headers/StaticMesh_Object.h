#pragma once
#include "Component_Object.h"

class Shader;

class StaticMesh_Object :
	public Component_Object
{
private:
	explicit	StaticMesh_Object();
	explicit	StaticMesh_Object(const StaticMesh_Object& rhs);
	virtual		~StaticMesh_Object() = default;

public:
	void	Render(Shader* shader);

public:
	_bool	GetVertexBuffer(LPDIRECT3DVERTEXBUFFER9& vertexBuffer);
	_bool	GetIndexBuffer(LPDIRECT3DINDEXBUFFER9& indexBuffer);
	_ulong	GetVertexSize() const;
	_ulong	GetVertexNum() const;
	_ulong	GetFacesNum() const;

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
	_matrix			mPivotMatrix;

public:
	static StaticMesh_Object*	Create(LPDIRECT3DDEVICE9 graphicDevice, const _tchar * filePath, const _tchar * fileName, const _matrix& pivotMatrix);
	Component_Object*	Clone(void* arg) override;
	virtual void	Free() override;

};

