#include "Layer.h"
#include "GameObject.h"
#include "GameInstance.h"
CLayer::CLayer() :
	m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Priority_Update(fTimeDelta);

	}

}

void CLayer::Update(_float fTimeDelta)
{
 	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Update(fTimeDelta);

	}
}

void CLayer::Late_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
		{
			if (!pGameObject->IsActive())
			{
				m_pGameInstance->Return_Object(m_iPrototypeLevelIndex, pGameObject->Get_Tag(), pGameObject);
			}
			pGameObject->Late_Update(fTimeDelta);
		}
	}

	// 풀에 반환하기 위해서 iterator로 돌림
	auto iter = m_GameObjects.begin();
	while (iter != m_GameObjects.end())
	{
		if (nullptr != *iter)
		{
			if (!(*iter)->IsActive())
			{
				m_pGameInstance->Return_Object(m_iPrototypeLevelIndex, (*iter)->Get_Tag(), *iter);
				iter = m_GameObjects.erase(iter); // 요소 제거 후 반복자 업데이트
				continue;
			}
			(*iter)->Late_Update(fTimeDelta);
		}
		++iter;
	}
}

CLayer* CLayer::Create(_uint iPrototypeLevelIndex)
{
	CLayer* pLayer = new CLayer();
	if (pLayer)
	{
		pLayer->m_iPrototypeLevelIndex = iPrototypeLevelIndex;
	}
	return pLayer;
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
	{
		if (!pGameObject->Is_FromPool())
		{
			Safe_Release(pGameObject);
		}
		else
		{
			m_pGameInstance->Return_Object(m_iPrototypeLevelIndex, pGameObject->Get_Tag(), pGameObject);
		}
	}
	m_GameObjects.clear();


	Safe_Release(m_pGameInstance);
}
