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
	if (0 == mPointStack.size())
		return;

	mPointStack.pop_back();
}

const _vec3 & NaviMeshData::GetSelectedNaviPointPosition() const
{
	static _vec3 originVec;
	
	if (-1 == mSelectedNaviPointIndex)
		return originVec;
	
	return mInfo.PointList[mSelectedNaviPointIndex].Position;
}

void NaviMeshData::SetSelectedNaviPointIndex(const _int index)
{
	mSelectedNaviPointIndex = index;
	//mSelectedNaviCellIndex = -1;
}

void NaviMeshData::SetSelectedNaviCellIndex(const _int index)
{
	mSelectedNaviCellIndex = index;
}

void NaviMeshData::SetSelectedNaviPointPosition(const _vec3 & position)
{
	if (-1 == mSelectedNaviPointIndex)
		return;

	if (0.f == position.y)
		int a = 0;
	
	Point& selectedPoint = mInfo.PointList[mSelectedNaviPointIndex];
	selectedPoint.Position = position;

	D3DXMatrixTranslation(&selectedPoint.WorldMatrix, selectedPoint.Position.x, selectedPoint.Position.y, selectedPoint.Position.z);
	D3DXMatrixInverse(&selectedPoint.WorldInverseMatrix, nullptr, &selectedPoint.WorldMatrix);
}

void NaviMeshData::DeleteCell(const _int cellIndex)
{
	if (cellIndex >= mInfo.CellInfoList.size())
		return;

	const _int refIndex[3] = { mInfo.CellInfoList[cellIndex].PointIdx[0], mInfo.CellInfoList[cellIndex].PointIdx[1], mInfo.CellInfoList[cellIndex].PointIdx[2] };

	//	Cell 삭제
	auto cellIter = mInfo.CellInfoList.begin();
	for (_int i = 0; i < cellIndex; ++i)
		++cellIter;
	mInfo.CellInfoList.erase(cellIter);

	//	Cell과 관련된 Point의 레퍼런스 카운트 감소
	for (_int i = 0; i < 3; ++i)
		--mInfo.PointList[refIndex[i]].RefCnt;

	//	레퍼런스 카운트가 0이어서 삭제 예정인 Point의 index조사
	list<_int> listDeleteIndex;
	for (_int i = 0; i < mInfo.PointList.size(); ++i)
	{
		if (0 == mInfo.PointList[i].RefCnt)
			listDeleteIndex.emplace_front(i);
		
	}

	//	Cell들의 pointIndex 값들과 삭제 예정인 Point의 index들을 비교하여 값 조정
	for (_int i = 0; i < mInfo.CellInfoList.size(); ++i)
	{
		for (_int& pointIdx : mInfo.CellInfoList[i].PointIdx)
		{
			for (auto& deleteIndex : listDeleteIndex)
			{
				if (pointIdx > deleteIndex)
					--pointIdx;
			}
		}
	}

	//	레퍼런스 카운트가 0인 Point들 제거
	for (auto pointIter = mInfo.PointList.begin(); pointIter < mInfo.PointList.end(); )
	{
		if (0 == pointIter->RefCnt)
			pointIter = mInfo.PointList.erase(pointIter);
		else
			++pointIter;
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

		_float dis = 0.f;
		
		const BOOL isHit = D3DXIntersectTri(&mInfo.PointList[index[0]].Position, &mInfo.PointList[index[1]].Position, &mInfo.PointList[index[2]].Position, &worldRayPos, &worldRayDir, nullptr, nullptr, &dis);
		
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

		if(i == mSelectedNaviCellIndex)
			mLine->DrawTransform(point, 4, &IDENTITY_MATRIX, D3DXCOLOR(0.f, 0.f, 1.f, 1.f));
		else
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

	if (-1 != mSelectedNaviPointIndex)
	{
		mShader->SetValue("gMatWorld", &mInfo.PointList[mSelectedNaviPointIndex].WorldMatrix, sizeof(_matrix));
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

	mSelectedNaviPointIndex = -1;
	mSelectedNaviCellIndex = -1;

	return true;
}

void NaviMeshData::ResetCreateMode()
{
	mPointStack.clear();
}

void NaviMeshData::ResetEditMode()
{
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

