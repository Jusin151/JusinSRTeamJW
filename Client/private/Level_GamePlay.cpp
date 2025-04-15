#include "Level_GamePlay.h"
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
#include <Camera_FirstPerson.h>
#include "Trigger.h"

CLevel_GamePlay::CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device }

{
}
HRESULT CLevel_GamePlay::Initialize()
{
	
	CJsonLoader jsonLoader;
 	//jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL_GAMEPLAY.json", LEVEL_GAMEPLAY);
 	//jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL_GamePlay_Test1.json", LEVEL_GAMEPLAY);
 	jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL_GamePlay_Test.json", LEVEL_GAMEPLAY);
	
	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_MiniMap"),
		LEVEL_GAMEPLAY, TEXT("Layer_MiniMap"))))
		return E_FAIL;*/

	CGameObject* pPlayer = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	if (pPlayer)
	{
		CTransform* pTransform = static_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));
		if (nullptr == pTransform)
			return E_FAIL;
		pTransform->Set_State(CTransform::STATE_POSITION, _float3(-4.6f, 1.f, -1.1f));
	}

	CTrigger::TRIGGER_DESC vTriggerDesc;
	vTriggerDesc.bStartsActive = true;
	vTriggerDesc.eLevel = LEVEL_HUB;
	vTriggerDesc.vPos = _float3(-31.f, 0.3f, -30.f);
	vTriggerDesc.eType = CTrigger::TRIGGER_TYPE::LEVEL_CHANGE;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Trigger_Button"),
		LEVEL_GAMEPLAY, TEXT("Layer_Trigger_Level"), &vTriggerDesc)))
		return E_FAIL;
	m_pLevelTrigger = static_cast<CTrigger*>(m_pGameInstance->Find_Last_Object(LEVEL_GAMEPLAY, TEXT("Layer_Trigger_Level")));
	CCamera_FirstPerson* pCamera = dynamic_cast<CCamera_FirstPerson*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Camera")));
	if (pCamera)
	{
		pCamera->Set_Yaw(D3DXToRadian(180.f));
	}

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_MiniMap"),
		LEVEL_GAMEPLAY, TEXT("Layer_MiniMap"))))
		return E_FAIL;
	// json으로 변경 필요
	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Statue"),
	//	LEVEL_GAMEPLAY, TEXT("Layer_Deco"))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Box"),
	//	LEVEL_GAMEPLAY, TEXT("Layer_Deco"))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Tree"),
	//	LEVEL_GAMEPLAY, TEXT("Layer_Deco"))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Thingy"),
		LEVEL_GAMEPLAY, TEXT("Layer_Monster_Thingy"))))
		return E_FAIL;

	m_pGameInstance->Play_Background(L"event:/Backgrounds/019 Antarctic - Calm Before The Storm").SetVolume(0.5f);

	

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState('F') & 0x8000)
	{

		if (FAILED(m_pGameInstance->Process_LevelChange(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_HUB))))
			return;
		return;
	}

	if (m_pLevelTrigger && m_pLevelTrigger->WasTriggered())
	{
		if (CPlayer* player = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC,TEXT("Layer_Player"))))
		{
			player->Set_ClearLevel(LEVEL_GAMEPLAY);
		}
		if (FAILED(m_pGameInstance->Process_LevelChange(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_HUB))))
			return;
	}
}

HRESULT CLevel_GamePlay::Render()		
{
	SetWindowText(g_hWnd, TEXT("게임플레이 레벨입니다."));

	return S_OK;
}



HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Crocman"), strLayerTag, 10)))
		return E_FAIL;

	if(nullptr ==(m_pGameInstance->Add_GameObject_FromPool(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI()
{
	CUI_Base::UI_Child_Desc Menu_Panel{};  // 자식 UI는 3개만 소유 부모 상대적으로 위치 잡을꺼임
	Menu_Panel.vSize = { 453.f,720.f };
	Menu_Panel.fAlpha = 1.0f;
	Menu_Panel.vPos = { 0.f,0.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지
	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Menu_Panel"),
		LEVEL_STATIC, TEXT("1"), &Menu_Panel)))
		return E_FAIL;

	CUI_Base::UI_Parent_Desc DefaultUI_Desc{}; // 부모 UI는 총 6개의 정보 소유

	DefaultUI_Desc.vSize = { 0,0 };
	DefaultUI_Desc.fAlpha = 1.0f;
	DefaultUI_Desc.vPos = { 0.f,0.f };
	DefaultUI_Desc.ProjMatrix = {};
	DefaultUI_Desc.ViewMatrix = {};
	DefaultUI_Desc.WorldMatrix = {};

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Default_PlayerUI"),
		LEVEL_STATIC, TEXT("Layer_Default_PlayerUI"), &DefaultUI_Desc)))
		return E_FAIL;


	CUI_Base::UI_Child_Desc Left_Panel{};  // 왼쪽 하단 판넬
	Left_Panel.vSize = { 262,210 };
	Left_Panel.fAlpha = 1.0f;
	Left_Panel.vPos = { -510.f,-255.f }; // 부모위치 설정

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Left_Panel"),
		LEVEL_STATIC, TEXT("Layer_Left_Panel_UI_1"), &Left_Panel)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc Left_Panel_HP{};  // HP 바
	Left_Panel_HP.vSize = { 118.f,18.f };
	Left_Panel_HP.fAlpha = 1.0f;
	Left_Panel_HP.vPos = { 47.f,-80.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Hp_Bar"),
		LEVEL_STATIC, TEXT("Layer_Left_Panel_UI_2"), &Left_Panel_HP)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc Left_Panel_Player_Icon{};  // 플레이어 아이콘
	Left_Panel_Player_Icon.vSize = { 69.f,61.f };
	Left_Panel_Player_Icon.fAlpha = 1.0f;
	Left_Panel_Player_Icon.vPos = { -589.f,-314.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Player_Icon"),
		LEVEL_STATIC, TEXT("Layer_Left_Panel_UI_3"), &Left_Panel_Player_Icon)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc Left_Panel_Mana_Bar{};  // MP 아이콘
	Left_Panel_Mana_Bar.vSize = { 120.f,21.f };
	Left_Panel_Mana_Bar.fAlpha = 1.0f;
	Left_Panel_Mana_Bar.vPos = { 22.f,-53.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_STATIC,
		TEXT("Prototype_GameObject_MP_Bar"),
		LEVEL_STATIC, TEXT("Layer_Left_Panel_UI_4"), &Left_Panel_Mana_Bar)))
		return E_FAIL;


	CUI_Base::UI_Child_Desc EXP_Desc{};  // 중앙 패널
	EXP_Desc.vSize = { 850,13.f };
	EXP_Desc.fAlpha = 1.0f;
	EXP_Desc.vPos = { 4.f,-353.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지
	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Mid_Panel"),
		LEVEL_STATIC, TEXT("Layer_Mid_Panel"), &EXP_Desc)))
		return E_FAIL;

 	CUI_Base::UI_Child_Desc EXPBar_Desc{};  // Exp 검은바
	EXPBar_Desc.vSize = { 850,4.f };
	EXPBar_Desc.fAlpha = 1.0f;
	EXPBar_Desc.vPos = { 4.f,-1.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지
	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_STATIC,
		TEXT("Prototype_GameObject_ExpBar_UI"),
		LEVEL_STATIC, TEXT("Layer_ExpBar_UI"), &EXPBar_Desc)))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Event_UI"),
		LEVEL_STATIC, TEXT("Layer_Event_UI"))))
		return E_FAIL;


	CUI_Base::UI_Child_Desc RIght_Panel{};  // 우하단 패널 
	Left_Panel.vSize = { 270,177 };
	Left_Panel.fAlpha = 1.0f;
	Left_Panel.vPos = { 505.f,-272.f }; // 부모위치 잡아주기

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Right_Bar"),
		LEVEL_STATIC, TEXT("Layer_Right_Panel_UI_1"), &Left_Panel)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc RIght_Panel_Bullet{};  // 
	RIght_Panel_Bullet.vSize = { 222.f,29.f };
	RIght_Panel_Bullet.fAlpha = 1.0f;
	RIght_Panel_Bullet.vPos = { 8.f,-59.f };

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Bullet_Bar"),
		LEVEL_STATIC, TEXT("Layer_Right_Panel_UI_2"), &RIght_Panel_Bullet)))
		return E_FAIL;

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_GamePlay::Free()
{
	__super::Free();
}
