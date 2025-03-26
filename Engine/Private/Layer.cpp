#include "Layer.h"
#include "GameObject.h"
#include "GameInstance.h"

CLayer::CLayer() :
	m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

CComponent* CLayer::Get_Component(const _wstring& strComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	
	return (*iter)->Find_Component(strComponentTag);
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
		if (nullptr != pGameObject && pGameObject->IsActive())
			pGameObject->Priority_Update(fTimeDelta);

	}

}

void CLayer::Update(_float fTimeDelta)
{
  	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject&&pGameObject->IsActive())
			pGameObject->Update(fTimeDelta);

	}
}

void CLayer::Late_Update(_float fTimeDelta)
{
	//for (auto& pGameObject : m_GameObjects)
	//{
	//	if (nullptr != pGameObject)
	//	{
	//		if (!pGameObject->IsActive())
	//		{
	//			m_pGameInstance->Return_Object(m_iPrototypeLevelIndex, pGameObject->Get_Tag(), pGameObject);
	//		}
	//		pGameObject->Late_Update(fTimeDelta);
	//	}
	//}

	// 풀에 반환하기 위해서 iterator로 돌림
	auto iter = m_GameObjects.begin();
	while (iter != m_GameObjects.end())
	{
		if (nullptr != *iter)
		{
			if (!(*iter)->IsActive()&& (*iter)->Is_FromPool())
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

CGameObject* CLayer::Find_Object()
{
	if (m_GameObjects.empty())
		return nullptr;

	return m_GameObjects.front();
}

CGameObject* CLayer::Find_Last_Object()
{
	if (m_GameObjects.empty())
		return nullptr;

	// 리스트의 마지막 객체 반환
	return m_GameObjects.back();

}
HRESULT CLayer::Remove_GameObject(CGameObject* pGameObject)
{
	auto	iter = m_GameObjects.erase(remove_if(m_GameObjects.begin(), m_GameObjects.end(), [&](CGameObject* pTarget) {

		if (pTarget == pGameObject)
		{
			if (!pTarget->Is_FromPool())
			{
				Safe_Release(pTarget);
				pTarget = nullptr;
			}
			else
			{
				m_pGameInstance->Return_Object(m_iPrototypeLevelIndex, pGameObject->Get_Tag(), pGameObject);
			}
			return true;
		}
		return false;
		}));

	return S_OK;
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
