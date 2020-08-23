#include "stdafx.h"
#include "NaviMeshData.h"

#include "Shader.h"
#include "PipeLine.h"

NaviMeshData::NaviMeshData()
{
}

void NaviMeshData::SetInfo(const Info & info)
{
	mInfo = info;
}

void NaviMeshData::Clear()
{
	mInfo.CellInfoList.clear();
	mInfo.PointList.clear();
}

void NaviMeshData::SetCreateMode(const _bool value)
{
	mIsCreateMode = value;
	if (false == mIsCreateMode)
		ResetCreateMode();
	else
		ResetEditMode();
}

_bool NaviMeshData::PushNaviPoint(const PointStack& point, const _bool isIndex)
{
	if (false == mIsCreateMode)
		return true;
	
	mPointStack.emplace_back(point, isIndex);
	
	if (3 > mPointStack.size())
		return true;

	_vec3 v[3];
	::ZeroMemory(v, sizeof(_vec3) * 3);

	for (_int i = 0; i < 3; ++i)
	{
		if (true == mPointStack[i].second)
		{
			// index인 경우, Position 값을 가져옴
			const _int index = mPointStack[i].first.Index;
			v[i] = mInfo.PointList[index].Position;
		}
		else
		{
			//	Position일 겨웅 그대로 대입
			v[i] = mPointStack[i].first.Position;
		}
	}

	const _vec3 right = v[1] - v[0];
	const _vec3 forward = v[2] - v[0];
	_vec3 up = *D3DXVec3Cross(&up, &right, &forward);

	if (up.y < 0.f)
	{
		//	생성 예정인 셀의 법선 벡터의 y가 음수라면
		//	잘못 생성된 셀이라고 판정
		mPointStack.clear();
		return false;
	}

	for (_int i = 0; i < 3; ++i)
	{
		if (false == mPointStack[i].second)
		{
			//	index가 false라는 것은 PointList에 없음으로 PointList에 추가
			Point newPoint;
			::ZeroMemory(&newPoint, sizeof(Point));
			newPoint.Position = mPointStack[i].first.Position;
			D3DXMatrixTranslation(&newPoint.WorldMatrix, newPoint.Position.x, newPoint.Position.y, newPoint.Position.z);
			D3DXMatrixInverse(&newPoint.WorldInverseMatrix, nullptr, &newPoint.WorldMatrix);
			
			const _int index = (_int)mInfo.PointList.size();
			mInfo.PointList.emplace_back(newPoint);

			//	PointStack에 저장된 위치 값을 index로 변환
			mPointStack[i].first.Index = index;
		}
	}

	CellInfo cellInfo = {mPointStack[0].first.Index, mPointStack[1].first.Index,mPointStack[2].first.Index };
	mInfo.CellInfoList.emplace_back(cellInfo);
	mPointStack.clear();

	for (_int i = 0; i < 3; ++i)
		++mInfo.PointList[cellInfo.PointIdx[i]].RefCnt;

	return true;
}

void NaviMeshData::PopNaviPoint()
{
	mPointStack.pop_back();
}

void NaviMeshData::SetSelectedNavi(const _bool isCell, const _int index)
{
	mIsSelectedNaviCell = isCell;
	if (isCell)
	{
		mSelectedNaviCellIndex = index;
		mSelectedNaviPointIndex = -1;
	}
	else
	{
		mSelectedNaviCellIndex = -1;
		mSelectedNaviPointIndex = index;
	}
}

_bool NaviMeshData::CheckHitPoint(const _vec3 & worldRayPos, const _vec3 & worldRayDir, _int & hitIndex)
{
	_int nearestHitIndex = -1;
	_float nearestHitDis = MAX_FLOAT;
	for (_int i = 0; i < mInfo.PointList.size(); ++i)
	{
		_vec3 localRayPos = *D3DXVec3TransformCoord(&localRayPos, &worldRayPos, &mInfo.PointList[i].WorldInverseMatrix);
		_vec3 localRayDir = *D3DXVec3TransformNormal(&localRayDir, &worldRayDir, &mInfo.PointList[i].WorldInverseMatrix);

		BOOL isHit = FALSE;
		_float dis = 0.f;
		D3DXIntersect(mMesh, &localRayPos, &localRayDir, &isHit, nullptr, nullptr, nullptr, &dis, nullptr, nullptr);

		if (isHit)
		{
			if (nearestHitDis > dis)
			{
				nearestHitDis = dis;
				nearestHitIndex = i;
			}
		}

	}

	if (-1 == nearestHitIndex)
		return false;

	hitIndex = nearestHitIndex;

	return true;
}

_bool NaviMeshData::CheckHitCell(const _vec3 & worldRayPos, const _vec3 & worldRayDir, _int & hitIndex)
{
	_int nearestHitIndex = -1;
	_float nearestHitDis = MAX_FLOAT;
	for (_int i = 0; i < mInfo.CellInfoList.size(); ++i)
	{
		const CellInfo& cell = mInfo.CellInfoList[i];
		const _int index[3] = { cell.PointIdx[0], cell.PointIdx[1], cell.PointIdx[2] };

		BOOL isHit = FALSE;
		_float dis = 0.f;
		
		D3DXIntersectTri(&mInfo.PointList[index[0]].Position, &mInfo.PointList[index[1]].Position, &mInfo.PointList[index[2]].Position, &worldRayPos, &worldRayDir, nullptr, nullptr, &dis);
		
		if (isHit)
		{
			if (nearestHitDis > dis)
			{
				nearestHitDis = dis;
				nearestHitIndex = i;
			}
		}
	}

	if (MAX_FLOAT == nearestHitDis)
		return false;
	
	hitIndex = nearestHitIndex;

	return true;
}

void NaviMeshData::Render(LPDIRECT3DDEVICE9 graphicDevice)
{
	const PipeLine* pipeLine = PipeLine::GetInstance();

	graphicDevice->EndScene();
	graphicDevice->BeginScene();
	
	mLine->Begin();

	for (_int i = 0; i < mInfo.CellInfoList.size(); ++i)
	{
		const CellInfo& cell = mInfo.CellInfoList[i];

		_vec3	point[4] = { mInfo.PointList[cell.PointIdx[0]].Position, mInfo.PointList[cell.PointIdx[1]].Position , mInfo.PointList[cell.PointIdx[2]].Position , mInfo.PointList[cell.PointIdx[0]].Position };

		for (_int i = 0; i < 4; ++i)
		{
			D3DXVec3TransformCoord(&point[i], &point[i], &pipeLine->GetTransform(D3DTS_VIEW));

			if (point[i].z <= 0.1f)
				point[i].z = 0.1f;

			D3DXVec3TransformCoord(&point[i], &point[i], &pipeLine->GetTransform(D3DTS_PROJECTION));
		}

		mLine->DrawTransform(point, 4, &IDENTITY_MATRIX, D3DXCOLOR(1.f, 1.f, 0.f, 1.f));
	}

	mLine->End();

	const _matrix matVP = pipeLine->GetTransform(D3DTS_VIEW) * pipeLine->GetTransform(D3DTS_PROJECTION);
	mShader->SetValue("gMatVP", &matVP, sizeof(_matrix));

	mShader->BeginShader(nullptr);
	mShader->BeginPass(0);

	mShader->SetValue("gColor", &D3DXCOLOR(0.f, 1.f, 1.f, 1.f), sizeof(D3DXCOLOR));
	for (_int i = 0; i < mInfo.PointList.size(); ++i)
	{
		mShader->SetValue("gMatWorld", mInfo.PointList[i].WorldMatrix, sizeof(_matrix));
		mShader->CommitChanges();
		mMesh->DrawSubset(0);
	}

	mShader->SetValue("gColor", &D3DXCOLOR(0.f, 1.f, 0.f, 1.f), sizeof(D3DXCOLOR));
	for (_int i = 0; i < mPointStack.size(); ++i)
	{
		_matrix matWorld;
		
		if(false == mPointStack[i].second)
			D3DXMatrixTranslation(&matWorld, mPointStack[i].first.Position.x, mPointStack[i].first.Position.y, mPointStack[i].first.Position.z);
		else
		{
			const _int index = mPointStack[i].first.Index;
			matWorld = mInfo.PointList[index].WorldMatrix;
		}
		mShader->SetValue("gMatWorld", &matWorld, sizeof(_matrix));
		mShader->CommitChanges();
		mMesh->DrawSubset(0);
	}

	mShader->EndPass();
	mShader->EndShader();

}

_bool NaviMeshData::Initialize(LPDIRECT3DDEVICE9 graphicDevice)
{
	mIsCreateMode = false;
	HRESULT hr = D3DXCreateSphere(graphicDevice, 0.05f, 100, 100, &mMesh, nullptr);
	if (FAILED(hr))
		return false;

	hr = D3DXCreateLine(graphicDevice, &mLine);
	if (FAILED(hr))
		return false;
	
	mLine->SetWidth(3.f);

	mShader = Shader::Create(graphicDevice, L"..\\Shader\\NaviMesh.fx");
	if (nullptr == mShader)
		return false;

	mPointStack.reserve(3);

	return true;
}

void NaviMeshData::ResetCreateMode()
{
	mPointStack.clear();
}

void NaviMeshData::ResetEditMode()
{
	mIsSelectedNaviCell = false;
	mSelectedNaviPointIndex = -1;
	mSelectedNaviCellIndex = -1;
}

NaviMeshData * NaviMeshData::Create(LPDIRECT3DDEVICE9 graphicDevice, const Info & info)
{
	NaviMeshData*	pInstance = new NaviMeshData();
	if (false == pInstance->Initialize(graphicDevice))
	{
		MSG_BOX("NaviMeshData Created Failed!");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void NaviMeshData::Free()
{
	Clear();
	SafeRelease(mShader);
	SafeRelease(mLine);
	SafeRelease(mMesh);
}

