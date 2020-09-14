#include "stdafx.h"
#include "StaticRendererSystem.h"

#include "PipeLine.h"
#include "Shader.h"
#include "StaticMesh.h"

IMPLEMENT_SINGLETON(StaticRendererSystem)

StaticRendererSystem::StaticRendererSystem()
	: mPipeLine(PipeLine::GetInstance())
{
	SafeAddRef(mPipeLine);
}

_bool StaticRendererSystem::ReadyResources(LPDIRECT3DDEVICE9 graphicDevice)
{
	D3DVERTEXELEMENT9 vertexElem[] =
	{
		{ 0, 0,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,   0 },
		{ 0, 24,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD, 0 },
		{ 1, 0,		D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD, 1 },
		{ 1, 16,	D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD, 2 },
		{ 1, 32,	D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD, 3 },
		{ 1, 48,	D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD, 4 },
		D3DDECL_END()
	};

	//	For. Hardware Instancing
	HRESULT hr = 0;

	hr = graphicDevice->CreateVertexBuffer(mBlockRenderBatchSize * sizeof(_matrix), 0, 0, D3DPOOL_MANAGED, &mVertexBuffer, nullptr);
	if (FAILED(hr))
		return false;

	hr = graphicDevice->CreateVertexDeclaration(vertexElem, &mVertexDeclaration);
	if (FAILED(hr))
		return false;

	mShader = Shader::Create(graphicDevice, L"..\\Shader\\HardwareSkinning.fx");
	if (nullptr == mShader)
		return false;

	return true;
}

_bool StaticRendererSystem::ReadySystem(const _size_t entitySize, const _size_t componentSize)
{
	if (0 != mEntitySize || 0 != mComponentSize)
		return false;

	mEntitySize = entitySize;
	mComponentSize = componentSize;

	mIndexTable.resize(entitySize, componentSize);

	for (_uniqueId i = 0; i < componentSize; ++i)
		mIndexQueue.emplace(i);

	mComponentList.resize(componentSize + 1);	// 마지막은 별도의 용도

	return true;
}

void StaticRendererSystem::ClearResources()
{
	SafeRelease(mShader);
	SafeRelease(mVertexBuffer);
	SafeRelease(mVertexDeclaration);

	for (auto& meshPair : mStaticMeshMap)
		SafeRelease(meshPair.second);
	mStaticMeshMap.clear();
}

void StaticRendererSystem::ClearSystem()
{
	mRenderingInfoMap.clear();

	mComponentList.clear();
	mComponentList.shrink_to_fit();

	mIndexQueue.swap(queue<_uniqueId>());

	mIndexTable.clear();
	mIndexTable.shrink_to_fit();

	mEntitySize = 0;
	mComponentSize = 0;
}

void StaticRendererSystem::Update(const _double timeDelta)
{

}

void StaticRendererSystem::LateUpdate(const _double timeDelta)
{
	
}

void StaticRendererSystem::Render(LPDIRECT3DDEVICE9 graphicDevice)
{
	const _matrix matVP = mPipeLine->GetTransform(D3DTS_VIEW) * mPipeLine->GetTransform(D3DTS_PROJECTION);

	mShader->SetValue("gMatVP", &matVP, sizeof(_matrix));

	mShader->BeginShader(nullptr);
	mShader->BeginPass(0);

	for (auto RenderingInfo : mRenderingInfoMap)
	{
		StaticMesh* mesh = mStaticMeshMap.find(RenderingInfo.first)->second;

		mShader->SetTexture("gDiffuseTexture", mesh->GetTexutre(0));
		mShader->CommitChanges();

		_int objectCount = 0;

		_matrix* vmMatrix = nullptr;
		mVertexBuffer->Lock(0, 0, (void**)&vmMatrix, 0);

		for (auto entityId : RenderingInfo.second)
		{
			const _uniqueId index = mIndexTable[entityId];

			const _int vmIndex = objectCount % mBlockRenderBatchSize;
			vmMatrix[vmIndex] = mComponentList[index].WorldMatrix;

			objectCount += 1;

			if (mBlockRenderBatchSize == objectCount)
			{
				mVertexBuffer->Unlock();

				RenderHardwareInstancing(graphicDevice, mesh, objectCount);
				objectCount = 0;

				mVertexBuffer->Lock(0, 0, (void**)&vmMatrix, 0);
			}
		}

		mVertexBuffer->Unlock();
		vmMatrix = nullptr;

		if (0 < objectCount)
			RenderHardwareInstancing(graphicDevice, mesh, objectCount);
	}

	mShader->EndPass();
	mShader->EndShader();
}

_bool StaticRendererSystem::AttachComponent(const _uniqueId entityId, const char* meshName)
{
	if (0 == mIndexQueue.size())
		return false;

	const _uint hashCode = HashCode(meshName);

	auto resIter = mStaticMeshMap.find(hashCode);

	if (resIter == mStaticMeshMap.end())
		return false;

	//	Attach Component
	const _uniqueId index = mIndexQueue.front();
	mIndexQueue.pop();

	mIndexTable[entityId] = index;

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

_bool StaticRendererSystem::AddStaticMesh(const char * meshName, StaticMesh * staticMesh)
{
	const _uint hashCode = HashCode(meshName);

	//	Resource 추가
	auto& iter = mStaticMeshMap.find(hashCode);
	if (iter != mStaticMeshMap.end())
		return false;

	mStaticMeshMap.emplace(hashCode, staticMesh);
	return true;
}

void StaticRendererSystem::SetWorldMatrix(const _uniqueId entityId, const _matrix & worldMatrix)
{
	const _uniqueId index = mIndexTable[entityId];
	mComponentList[index].WorldMatrix = worldMatrix;
}

void StaticRendererSystem::RenderHardwareInstancing(LPDIRECT3DDEVICE9 graphicDevice, StaticMesh* staticMesh, const _int objectNum)
{
	LPDIRECT3DVERTEXBUFFER9 blockVB = nullptr;
	staticMesh->GetVertexBuffer(blockVB);

	LPDIRECT3DINDEXBUFFER9	blockIB = nullptr;
	staticMesh->GetIndexBuffer(blockIB);

	const _ulong blockVertexSize = staticMesh->GetVertexSize();

	graphicDevice->SetVertexDeclaration(mVertexDeclaration);
	graphicDevice->SetStreamSource(0, blockVB, 0, blockVertexSize);
	graphicDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | objectNum);

	graphicDevice->SetStreamSource(1, mVertexBuffer, 0, sizeof(_matrix));
	graphicDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul);
	graphicDevice->SetIndices(blockIB);

	graphicDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, staticMesh->GetVertexNum(), 0, staticMesh->GetFacesNum());

	SafeRelease(blockIB);
	SafeRelease(blockVB);

	graphicDevice->SetStreamSourceFreq(0, 1);
	graphicDevice->SetStreamSourceFreq(1, 1);
}

void StaticRendererSystem::Free()
{
	ClearSystem();
	ClearResources();
	
	SafeRelease(mPipeLine);

	ComponentSystem::Free();
}
