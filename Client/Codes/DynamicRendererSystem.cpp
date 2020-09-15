#include "stdafx.h"
#include "DynamicRendererSystem.h"

#include "PipeLine.h"
#include "Shader.h"
#include "AnimationCtrl.h"
#include "DynamicMesh.h"

IMPLEMENT_SINGLETON(DynamicRendererSystem)

DynamicRendererSystem::DynamicRendererSystem()
	: mPipeLine(PipeLine::GetInstance())
{
	SafeAddRef(mPipeLine);
}

_bool DynamicRendererSystem::ReadyResources(LPDIRECT3DDEVICE9 graphicDevice)
{
	mShader = Shader::Create(graphicDevice, L"..\\Shader\\HardwareSkinning.fx");
	if (nullptr == mShader)
		return false;

	return true;
}

_bool DynamicRendererSystem::ReadySystem(const _size_t entitySize, const _size_t componentSize)
{
	if (0 != mEntitySize || 0 != mComponentSize)
		return false;

	mEntitySize = entitySize;
	mComponentSize = componentSize;

	mIndexTable.resize(entitySize, componentSize);

	for (_uniqueId i = 0; i < componentSize; ++i)
		mIndexQueue.emplace(i);

	mComponentList.resize(componentSize + 1);		// 마지막은 별도의 용도

	return true;
}

void DynamicRendererSystem::ClearResources()
{
	SafeRelease(mShader);

	for (auto& meshPair : mDynamicMeshMap)
		SafeRelease(meshPair.second);
	mDynamicMeshMap.clear();
}

void DynamicRendererSystem::ClearSystem()
{
	mRenderingInfoMap.clear();

	for (Component& component : mComponentList)
		SafeRelease(component.Controller);
	mComponentList.clear();
	mComponentList.shrink_to_fit();

	mIndexQueue.swap(queue<_uniqueId>());

	mIndexTable.clear();
	mIndexTable.shrink_to_fit();

	mEntitySize = 0;
	mComponentSize = 0;
}

void DynamicRendererSystem::Update(const _double timeDelta)
{
}

void DynamicRendererSystem::LateUpdate(const _double timeDelta)
{
	mTimeDelta = timeDelta;
}

void DynamicRendererSystem::Render(LPDIRECT3DDEVICE9 graphicDevice)
{
	const _matrix matVP = mPipeLine->GetTransform(D3DTS_VIEW) * mPipeLine->GetTransform(D3DTS_PROJECTION);

	mShader->SetValue("gMatVP", &matVP, sizeof(_matrix));
	mShader->BeginShader(nullptr);
	mShader->BeginPass(0);

	for (auto RenderingInfo : mRenderingInfoMap)
	{
		DynamicMesh* const mesh = mDynamicMeshMap.find(RenderingInfo.first)->second;
		const vector<D3DXMESHCONTAINER_DERIVED*>& meshContainerList = mesh->GetMeshContainerList();
		
		for (auto entityId : RenderingInfo.second)
		{
			const _uniqueId index = mIndexTable[entityId];

			mShader->SetValue("gMatWorld", &mComponentList[index].WorldMatrix, sizeof(_matrix));
			mComponentList[index].Controller->SetUpAnimation(0);
			mComponentList[index].Controller->PlayAnimation(mTimeDelta);
			mesh->UpdateCombinedTransformationMatrices();

			//	Rendering
			for (auto meshContainer : meshContainerList)
			{
				LPD3DXBONECOMBINATION boneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(meshContainer->pBoneCombinationBuf->GetBufferPointer());
				for (_ulong i = 0; i < meshContainer->dwNumAttributeGroups; ++i)
				{
					for (_ulong paletteEntry = 0; paletteEntry < meshContainer->dwNumPaletteEntries; ++paletteEntry)
					{
						const _int matrixIndex = boneComb[i].BoneId[paletteEntry];

						if (UINT_MAX != matrixIndex)
							D3DXMatrixMultiply(&meshContainer->pRenderMatrices[paletteEntry], &meshContainer->pOffsetMatrices[matrixIndex], meshContainer->ppCombinedTransformationMatrices[matrixIndex]);
					}
					_ulong numInflu = meshContainer->dwNumInfl - 1;

					mShader->Get_EffectHandle()->SetMatrixArray("gMatrixPalette", meshContainer->pRenderMatrices, meshContainer->dwNumPaletteEntries);

					mShader->Get_EffectHandle()->SetValue("gNumBoneInfluences", &numInflu, sizeof(_ulong));
					mShader->SetTexture("gDiffuseTexture", meshContainer->pMeshTexture[boneComb[i].AttribId]);
					mShader->CommitChanges();
					meshContainer->MeshData.pMesh->DrawSubset(i);
				}
			}
		}
	}

	mShader->EndPass();
	mShader->EndShader();
}

_bool DynamicRendererSystem::AttachComponent(const _uniqueId entityId, const char* meshName)
{
	if (0 == mIndexQueue.size())
		return false;

	const _uint hashCode = HashCode(meshName);

	auto resIter = mDynamicMeshMap.find(hashCode);

	if (resIter == mDynamicMeshMap.end())
		return false;

	//	Attach Component
	const _uniqueId index = mIndexQueue.front();
	mIndexQueue.pop();

	mIndexTable[entityId] = index;

	mComponentList[index].Controller = resIter->second->CloneAnimationCtrl();
	

	//	Add RenderingInfo
	auto renderIter = mRenderingInfoMap.find(hashCode);
	if (renderIter == mRenderingInfoMap.end())
	{
		vector<_uniqueId>	infoList;
		infoList.emplace_back(entityId);

		mRenderingInfoMap.emplace(hashCode, infoList);
	}
	else
		renderIter->second.emplace_back(entityId);

	return true;
}

_bool DynamicRendererSystem::AddDynamicMesh(const char* meshName, DynamicMesh* dynamicMesh)
{
	const _uint hashCode = HashCode(meshName);

	//	Resource 추가
	auto& iter = mDynamicMeshMap.find(hashCode);
	if (iter != mDynamicMeshMap.end())
		return false;

	mDynamicMeshMap.emplace(hashCode, dynamicMesh);
	return true;
}

void DynamicRendererSystem::SetWorldMatrix(const _uniqueId entityId, const _matrix& worldMatrix)
{
	const _uniqueId index = mIndexTable[entityId];
	mComponentList[index].WorldMatrix = worldMatrix;
}

void DynamicRendererSystem::Free()
{
	ClearSystem();
	ClearResources();

	SafeRelease(mPipeLine);

	ComponentSystem::Free();
}

