#pragma once

#include "Typedef.h"
#include <d3dx9.h>

typedef struct tagVertex_Cube_Texture
{
	D3DXVECTOR3		vPosition;
	D3DXVECTOR3		vTexUV;
}VTXCUBE;

typedef struct tagVertex_ViewPort
{
	D3DXVECTOR4		vPosition;
	D3DXVECTOR2		vTexUV;
}VTXVIEWPORT;

typedef struct tagVertex_Normal_Texture
{
	D3DXVECTOR3		vPosition;
	D3DXVECTOR3		vNormal;
	D3DXVECTOR2		vTexUV;
}VTXNORTEX;

typedef struct tagVertex_Texture
{
	D3DXVECTOR3		vPosition;
	D3DXVECTOR2		vTexUV;
}VTXTEX;

typedef	struct tagIndex16
{
	_ushort	_0, _1, _2;

}INDEX16;

typedef	struct tagIndex32
{
	_ulong	_0, _1, _2;

}INDEX32;

//struct MESHTEXTURE
//{
//	enum TYPE { TYPE_DIFFUSE, TYPE_NORMAL, TYPE_SPECULAR, TYPE_EMISSIVE, TYPE_END };
//
//	LPDIRECT3DTEXTURE9		pTextures[TYPE_END];
//};

struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER
{
	LPD3DXMESH			pOriginalMesh;
	//MESHTEXTURE*		pMeshTexture;
	LPDIRECT3DTEXTURE9*	pMeshTexture;
	D3DXMATRIX*			pOffsetMatrices;
	D3DXMATRIX**		ppCombinedTransformationMatrices;
	D3DXMATRIX*			pRenderMatrices;
	unsigned long		dwNumBones;

};

struct D3DXFRAME_DERIVED : public D3DXFRAME
{
	// 나의 TransformationMatrix * 부모뼈의 CombinedTransformationMatrix
	D3DXMATRIX		CombinedTransformationMatrix;
};