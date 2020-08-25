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
		_int	RefCnt;
	};

	struct CellInfo
	{
		array<_int, 3> PointIdx;
		CellInfo(const _int idx0 = -1 , const _int idx1 = -1, const _int idx2 = -1)
			: PointIdx{ idx0, idx1, idx2 }
		{
		}
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

public:
	_int			GetSelectedNaviPointIndex() const { return mSelectedNaviPointIndex; }
	_int			GetSelectedNaviCellIndex() const { return mSelectedNaviCellIndex; }
	const _vec3&	GetSelectedNaviPointPosition() const;
	void			SetSelectedNaviPointIndex(const _int index);
	void			SetSelectedNaviCellIndex(const _int index);
	void			SetSelectedNaviPointPosition(const _vec3& position);

public:
	void	DeleteCell(const _int cellIndex);

public:
	_bool	CheckHitPoint(const _vec3& worldRayPos, const _vec3& worldRayDir, _int& hitIndex);
	_bool	CheckHitCell(const _vec3& worldRayPos, const _vec3& worldRayDir, _int& hitIndex);
	void	Render(LPDIRECT3DDEVICE9 graphicDevice);

public:
	void	ResetCreateMode();
	void	ResetEditMode();

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
	//	Create
	vector<pair<PointStack, _bool>>	mPointStack;

	//	Edit
	_int	mSelectedNaviPointIndex;
	_int	mSelectedNaviCellIndex;

public:
	static NaviMeshData*	Create(LPDIRECT3DDEVICE9 graphicDevice, const Info& info = Info());
	virtual void	Free()	override;
};
