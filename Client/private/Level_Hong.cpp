#include "Level_Hong.h"
#include "GameInstance.h"
#include "PickingSys.h"
#include "Collider_Sphere.h"
#include "UI_Default_Panel.h"
#include "UI_Base.h"
#include "GamePlay_Button.h"
#include "JsonLoader.h"
#include "Weapon_Base.h"
#include "Effect_Base.h"
#include "UI_Shop_Base.h"
#include "UI_Point_Shop.h"
#include "Image.h"
#include "Sound_Event.h"
#include "Level_Loading.h"
#include "Transform.h"
#include <Camera_FirstPerson.h>
#include "HellBoss.h"

static const vector<CItem::ITEM_DESC> g_ItemInfos{
	{ CItem::ITEM_TYPE::HP, L"Prototype_GameObject_Item_HP_Small" },
	{ CItem::ITEM_TYPE::MP, L"Prototype_GameObject_Item_MP_Small"  },
	{ CItem::ITEM_TYPE::AMMO, L"Prototype_GameObject_Item_Magnum_Ammo_Small" },
	{ CItem::ITEM_TYPE::AMMO, L"Prototype_GameObject_Item_Magnum_Ammo_Big" },
	{ CItem::ITEM_TYPE::AMMO, L"Prototype_GameObject_Item_ShotGun_Ammo_Small"   },
	{ CItem::ITEM_TYPE::AMMO, L"Prototype_GameObject_Item_ShotGun_Ammo_Big"   },
	{ CItem::ITEM_TYPE::AMMO, L"Prototype_GameObject_Item_Minigun_Ammo"   },
};


CLevel_Hong::CLevel_Hong(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device }

{
}
HRESULT CLevel_Hong::Initialize()
{
	CJsonLoader jsonLoader;

	jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL_Hong.json", LEVEL_HONG);
	static _bool bIsLoading = false;
	m_pGameInstance->Stop_All_Event();

	CGameObject* pPlayer = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	if (pPlayer)
	{
		m_pPlayerTransCom = static_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));
		if (nullptr == m_pPlayerTransCom)
			return E_FAIL;
		m_pPlayerTransCom->Set_State(CTransform::STATE_POSITION, _float3(-4.6f, 0.f, -1.1f));

	}

	CCamera_FirstPerson* pCamera = dynamic_cast<CCamera_FirstPerson*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Camera")));
	if (pCamera)
	{
		pCamera->Set_Yaw(D3DXToRadian(180.f)); 
	}
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_MiniMap"),
		LEVEL_HONG, TEXT("Layer_MiniMap"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss"),
		LEVEL_HONG, TEXT("Layer_HellBoss"))))
		return E_FAIL;

	m_pHellboss = dynamic_cast<CHellBoss*>(m_pGameInstance->Find_Object(LEVEL_HONG, TEXT("Layer_HellBoss")));

	if (!m_pHellboss)
		return E_FAIL;



//event: / Backgrounds / 068 Hell - HongDongWan_[cut_194sec]
	m_pGameInstance->Play_Background(L"event:/Backgrounds/068 Hell - HongDongWan_[cut_194sec]").SetVolume(0.25f);



	return S_OK;
}

void CLevel_Hong::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState('F') & 0x8000)
	{
		if (FAILED(m_pGameInstance->Process_LevelChange(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_HUB))))
			return;
	}

	m_fItemSpawnTime += fTimeDelta;
	m_fMonsterSpawnTime += fTimeDelta;
	Spawn_Item();
	Spawn_Monsters((m_pHellboss->Get_Phase()!=PHASE4));
}

HRESULT CLevel_Hong::Render()
{
	SetWindowText(g_hWnd, TEXT("홍동완 레벨입니다"));

	return S_OK;
}

void CLevel_Hong::Spawn_Item()
{
	if (m_fItemSpawnTime >= m_fItemSpawnCooldownTime)
	{
		_float3 playerPos = m_pPlayerTransCom->Get_State(CTransform::STATE_POSITION);
		// 아이템 스폰 위치
		for (_int i = 0; i < 15; i++)
		{

			_float3 spawnPos;
			GetRandomVector(&spawnPos, &playerPos, 30.0f);
			spawnPos.y = 0.5f;
			_int iItemCount = static_cast<_int>(g_ItemInfos.size());
			_int iRandomIndex = rand() % iItemCount;
			const CItem::ITEM_DESC& vSelectedItem = g_ItemInfos[iRandomIndex];

			CItem::ITEM_DESC itemDesc;
			itemDesc.eType = vSelectedItem.eType;
			itemDesc.strItemName = vSelectedItem.strItemName.substr(wcslen(L"Prototype_GameObject_Item_"));

			_wstring stProtTag = vSelectedItem.strItemName;
			HRESULT hr = m_pGameInstance->Add_GameObject(LEVEL_HONG, stProtTag, LEVEL_HONG, TEXT("Layer_Item"), &itemDesc);

			if (SUCCEEDED(hr))
			{
				CItem* pItem = dynamic_cast<CItem*>(m_pGameInstance->Find_Last_Object(LEVEL_HONG, TEXT("Layer_Item")));
				if (pItem)
				{
					CTransform* pTransform = dynamic_cast<CTransform*>(pItem->Get_Component(TEXT("Com_Transform")));
					if (pTransform)
					{
						pTransform->Set_State(CTransform::STATE_POSITION, spawnPos);
					}
				}
			}
		}
		m_fItemSpawnTime = 0.f;
	}
}

void CLevel_Hong::Spawn_Monsters(_bool bInterval)
{
     _float3 playerPos = m_pPlayerTransCom->Get_State(CTransform::STATE_POSITION);

	if (bInterval)
	{
		if (m_fMonsterSpawnTime >= 5.f)
		{
			_float3 spawnPos;

			GetRandomVector(&spawnPos, &playerPos, 55.f);
			spawnPos.y = 0.5f;
			_int iRand = rand() % static_cast<int>(MonsterType::TYPE_END);
			Spawn_Monster(static_cast<MonsterType>(iRand), spawnPos);
			m_fMonsterSpawnTime = 0.f;
		}
	}
	else
	{
		if (m_fMonsterSpawnTime >= m_fMonsterSpawnCooldownTime)
		{
			for (_int i = 0; i < 20; ++i)
			{
				_float3 spawnPos;
				GetRandomVector(&spawnPos, &playerPos, 50.f);
				spawnPos.y = 0.5f;

				MonsterType type;
				if (i % 2 == 0)
					type = MonsterType::THINGY;
				else if (i % 5 == 0)
					type = MonsterType::LOOKER;
				else
					type = MonsterType::HELLHOUND;

				Spawn_Monster(type, spawnPos);
			}
			m_fMonsterSpawnTime = 0.f;
		}
	}
}

void CLevel_Hong::Spawn_Monster(MonsterType type, const _float3& spawnPos)
{
	_wstring stProtoTag;
	_wstring stLayerTag;

	switch (type)
	{
	case MonsterType::THINGY:
		stProtoTag = TEXT("Prototype_GameObject_Thingy");
		stLayerTag = TEXT("Layer_Monster_Thingy");
		break;
	case MonsterType::LOOKER:
		stProtoTag = TEXT("Prototype_GameObject_Looker");
		stLayerTag = TEXT("Layer_Monster_Looker");
		break;
	case MonsterType::HELLHOUND:
		stProtoTag = TEXT("Prototype_GameObject_Hellhound");
		stLayerTag = TEXT("Layer_Monster_Hellhound");
		break;
	default:
		return;
	}

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, stProtoTag, LEVEL_HONG, stLayerTag)))
		return;

	auto pMonster = m_pGameInstance->Find_Last_Object(LEVEL_HONG, stLayerTag);
	if (pMonster)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(pMonster->Get_Component(TEXT("Com_Transform")));
		if (pTransform)
			pTransform->Set_State(CTransform::STATE_POSITION, spawnPos);
	}
}

CLevel_Hong* CLevel_Hong::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Hong* pInstance = new CLevel_Hong(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Hong");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Hong::Free()
{
	__super::Free();
}
