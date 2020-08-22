#pragma once
#include "Base.h"

#include "Defines.h"

class Shader;

class NaviMeshData :
	public Base
{
public:
	struct Point
	{
		_vec3	Position;
		_matrix	WorldMatrix;
		_matrix	WorldInverseMatrix;
	};

	struct CellInfo
	{
		_int PointIdx[3];
	};

	struct Info
	{
		vector<Point>		PointList;
		vector<CellInfo>	CellInfoList;
	};

	union PointStack
	{
		_int	Index;
		_vec3	Position;
		PointStack(const _int index = 0)
			: Index(index)
		{
		}
	};

private:
	explicit NaviMeshData();
	virtual ~NaviMeshData() = default;

public:
	const Info&	GetInfo()	const { return mInfo; }
	void		SetInfo(const Info& info);
	void		Clear();

public:
	void	SetCreateMode(const _bool value);
	_bool	IsCreateMode() const { return mIsCreateMode; }
	_bool	PushNaviPoint(const PointStack& point, const _bool isIndex);
	void	PopNaviPoint();
	_bool	CheckHitPoint(const _vec3& worldRayPos, const _vec3& worldRayDir, _int& hitIndex);
	void	Render(LPDIRECT3DDEVICE9 graphicDevice);

private:
	_bool	Initialize(LPDIRECT3DDEVICE9 graphicDevice);

private:
	LPD3DXMESH	mMesh = nullptr;
	LPD3DXLINE	mLine = nullptr;
	Shader*		mShader = nullptr;

private:
	Info	mInfo;

private:
	_bool							mIsCreateMode;
	vector<pair<PointStack, _bool>>	mPointStack;

public:
	static NaviMeshData*	Create(LPDIRECT3DDEVICE9 graphicDevice, const Info& info = Info());
	virtual void	Free()	override;
};