#include "Level_Boss.h"
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
#include "Trigger.h"
#include <Camera_FirstPerson.h>

CLevel_Boss::CLevel_Boss(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device }

{
}
HRESULT CLevel_Boss::Initialize()
{

	CJsonLoader jsonLoader;
 	jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL_AntarcticBoss.json", LEVEL_GAMEPLAY);
	m_pGameInstance->Stop_All_Event();


	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	if (m_pPlayer)
	{
		CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
		if (nullptr == pTransform)
			return E_FAIL;
		pTransform->Set_State(CTransform::STATE_POSITION, _float3(-14.5f, 0.f, -2.f));
	}

	CCamera_FirstPerson* pCamera = dynamic_cast<CCamera_FirstPerson*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Camera")));
	if (pCamera)
	{
		pCamera->Set_Yaw(D3DXToRadian(90.f));
	}
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_MiniMap"),
		LEVEL_BOSS, TEXT("Layer_MiniMap"))))
		return E_FAIL;

	m_pGameInstance->Play_Background(L"event:/Backgrounds/030 Antarctic - The End").SetVolume(0.25f);

	CTrigger::TRIGGER_DESC vTriggerDesc;
	vTriggerDesc.bStartsActive = true;
	vTriggerDesc.eLevel = LEVEL_HUB;
	vTriggerDesc.vPos = _float3(4.f,1.f, -2.4f);
	vTriggerDesc.eType = CTrigger::TRIGGER_TYPE::LEVEL_CHANGE;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger_Button"),
		LEVEL_BOSS, TEXT("Layer_Trigger_Level"), &vTriggerDesc)))
		return E_FAIL;
	m_pLevelTrigger = static_cast<CTrigger*>(m_pGameInstance->Find_Last_Object(LEVEL_BOSS, TEXT("Layer_Trigger_Level")));
	m_pLevelTrigger->SetActive(false);

	auto pWormhole = m_pGameInstance->Find_Object(LEVEL_BOSS, TEXT("Layer_Wormhole"));
	if (pWormhole)
	{
		pWormhole->SetActive(false);
	}
	return S_OK;
}

void CLevel_Boss::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState('F') & 0x8000)
	{
		if (FAILED(m_pGameInstance->Process_LevelChange(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_HUB))))
			return;
		return;
	}


	if (m_pPlayer&& m_pPlayer->Get_ClearLevel()==LEVEL_BOSS)
	{
		if (m_pLevelTrigger && !m_pLevelTrigger->IsActive())
		{
			m_pLevelTrigger->SetActive(true);
			auto pWormhole = m_pGameInstance->Find_Object(LEVEL_BOSS, TEXT("Layer_Wormhole"));
			if (pWormhole)
			{
				pWormhole->SetActive(true);
			}
		}
		if (m_pLevelTrigger&&m_pLevelTrigger->WasTriggered())
		{
			// 나중에 웜홀 만들기
		  if (FAILED(m_pGameInstance->Process_LevelChange(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_HUB))))
			return;
		}
	}
}

HRESULT CLevel_Boss::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Monster(const _wstring& strLayerTag)
{


	return S_OK;
}

CLevel_Boss* CLevel_Boss::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Boss* pInstance = new CLevel_Boss(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Boss::Free()
{
	__super::Free();
}
