#include "GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device { pGraphic_Device }
	, m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pGameInstance);
}

CGameObject::CGameObject(const CGameObject& Prototype) 
	: m_pGraphic_Device{ Prototype.m_pGraphic_Device }
	, m_pGameInstance{ Prototype.m_pGameInstance }
{
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	if (pArg)
	{

	if (!static_cast<OBJECT_DESC*>(pArg)->stProtTag.empty())
	{
		m_tObjDesc = *static_cast<OBJECT_DESC*>(pArg);
	}
	}
	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{
}

void CGameObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::On_Collision()
{
	return S_OK;
}

void CGameObject::SetActive(_bool bIsActive)
{
	if (bIsActive == true && m_bIsActive == false)
	{
		Reset(); // 상태 초기화
	}

	m_bIsActive = bIsActive;
}

json CGameObject::Serialize()
{
	json j;
	j["IsActive"] = m_bIsActive;
	j["FromPool"] = m_bFromPool;
	j["LayerTag"] = ISerializable::WideToUtf8(m_strLayerTag);


	json objectDesc;
	objectDesc["ProtTag"] = ISerializable::WideToUtf8(m_tObjDesc.stProtTag);
	objectDesc["ProtTextureTag"] = ISerializable::WideToUtf8(m_tObjDesc.stProtTextureTag);
	objectDesc["BufferTag"] = ISerializable::WideToUtf8(m_tObjDesc.stBufferTag);
	objectDesc["Level"] = m_tObjDesc.iLevel;
	objectDesc["ProtoLevel"] = m_tObjDesc.iProtoLevel;

	j["OBJECT_DESC"] = objectDesc;

	return j;
}

void CGameObject::Deserialize(const json& j)
{
		if (j.contains("IsActive")) m_bIsActive = j["IsActive"];
		if (j.contains("FromPool")) m_bFromPool = j["FromPool"];
		if (j.contains("LayerTag"))
		{
			string layerTag = j["LayerTag"].get<string>();
			m_strLayerTag = ISerializable::Utf8ToWide(layerTag);
		}

		if (j.contains("OBJECT_DESC"))
		{
			const json& objectDesc = j["OBJECT_DESC"];

			if (objectDesc.contains("ProtTag"))
			{
				string protTag = objectDesc["ProtTag"].get<string>();
				m_tObjDesc.stProtTag = ISerializable::Utf8ToWide(protTag);
			}

			if (objectDesc.contains("ProtTextureTag"))
			{
				string protTextureTag = objectDesc["ProtTextureTag"].get<string>();
				m_tObjDesc.stProtTextureTag = ISerializable::Utf8ToWide(protTextureTag);
			}

			if (objectDesc.contains("BufferTag"))
			{
				string bufferTag = objectDesc["BufferTag"].get<string>();
				m_tObjDesc.stBufferTag = ISerializable::Utf8ToWide(bufferTag);
			}

			if (objectDesc.contains("Level")) m_tObjDesc.iLevel = objectDesc["Level"];
			if (objectDesc.contains("ProtoLevel")) m_tObjDesc.iProtoLevel = objectDesc["ProtoLevel"];
		}
}

CComponent* CGameObject::Find_Component(const _wstring& strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);
	if(iter == m_Components.end())
		return nullptr;

	return iter->second;
}

HRESULT CGameObject::Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
	if (m_Components.end() != m_Components.find(strComponentTag))
		return E_FAIL;

	CComponent*		pComponent = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_COMPONENT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

CComponent* CGameObject::Find(const _wstring strComponentTag)
{
	if (m_Components.end() == m_Components.find(strComponentTag))
		return nullptr;
	return m_Components[strComponentTag];
}

void CGameObject::Free()
{
	__super::Free();

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();


	Safe_Release(m_pGameInstance);
	Safe_Release(m_pGraphic_Device);
}
