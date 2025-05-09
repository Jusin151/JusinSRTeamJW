﻿#include "PoolManager.h"
#include "GameObject.h"
#include "GameInstance.h"

CPool_Manager::CPool_Manager() : m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

CGameObject* CPool_Manager::Acquire_Object(_uint iPrototypeLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	auto& objQueue = m_Pools[iPrototypeLevelIndex][strLayerTag];

	if (objQueue.empty()) // 비어있다면 생성해서 반환
	{
		if (pArg == nullptr)
		{

			if (FAILED(Reserve_Pool(iPrototypeLevelIndex, m_InitArgMap[strLayerTag].first, strLayerTag, 1, m_InitArgMap[strLayerTag].second)))
			{
				return nullptr;
			}
		}
		else
		{
			if (FAILED(Reserve_Pool(iPrototypeLevelIndex, m_InitArgMap[strLayerTag].first, strLayerTag, 1, pArg)))
			{
				return nullptr;
			}
		}
	}

	if (objQueue.empty()) return nullptr; // 여전히 비어있다면 nullptr 반환
	CGameObject* pGameObject = objQueue.front();
 	Safe_AddRef(pGameObject); // 빌려줄 때 레퍼런스 올리기
	if (pArg&& !m_InitArgMap[strLayerTag].second)
	{
		pGameObject->Initialize(pArg);
	}
	objQueue.pop();
	return pGameObject;
}

HRESULT CPool_Manager::Return_Object(_uint iPrototypeLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	if (pGameObject == nullptr) return E_FAIL;
	auto& objQueue = m_Pools[iPrototypeLevelIndex][strLayerTag];
	objQueue.push(pGameObject);
	Safe_Release(pGameObject);

	return S_OK;
}

HRESULT CPool_Manager::Reserve_Pool(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strLayerTag, _uint iCount, void* pArg)
{
	if (iPrototypeLevelIndex < 0 || iCount <= 0) return E_FAIL;

	CGameObject* pGameObject = nullptr;

	for (_uint i = 0; i < iCount; ++i)
	{
		pGameObject = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
		if (pGameObject)
		{
			pGameObject->Set_Tag(strLayerTag);
			pGameObject->Set_FromPool(true);
			pGameObject->SetActive(false);

			m_Pools[iPrototypeLevelIndex][strLayerTag].push(pGameObject);

			if (pArg)
			{
				m_InitArgMap[strLayerTag] = { strPrototypeTag, pArg };
			}
			else
			{
				m_InitArgMap[strLayerTag] = { strPrototypeTag, nullptr };
			}
		}
	}

	return S_OK;
}

void CPool_Manager::Clear(_uint iLevelIndex)
{
	for (auto& pool : m_Pools[iLevelIndex])
	{
		while (!pool.second.empty())
		{
			Safe_Release(pool.second.front());
			pool.second.pop();
		}
	}
	m_Pools[iLevelIndex].clear();
}

CPool_Manager* CPool_Manager::Create()
{
	CPool_Manager* pInstance = new CPool_Manager();

	if (pInstance == nullptr)
	{
		MSG_BOX("Failed to Created : CPool_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPool_Manager::Free()
{
	__super::Free();

	for (auto& levelPair : m_Pools)
	{
		for (auto& poolPair : levelPair.second)
		{
			while (!poolPair.second.empty())
			{
				auto& pObj = poolPair.second.front();
				Safe_Release(pObj);
				poolPair.second.pop();
			}
		}
		levelPair.second.clear();
	}

	m_Pools.clear();

	Safe_Release(m_pGameInstance);
}
