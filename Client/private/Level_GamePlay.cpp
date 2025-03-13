#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "UI_Default_Panel.h"
#include "CUI_Base.h"

CLevel_GamePlay::CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel { pGraphic_Device } 

{}
HRESULT CLevel_GamePlay::Initialize()
{

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Shop()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;
	
	

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	int a = 10;
}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{

	//if (FAILED(m_pGameInstance->Add_GameObject
	//(LEVEL_GAMEPLAY, 
	//	TEXT("Prototype_GameObject_Terrain"),
	//	LEVEL_GAMEPLAY, strLayerTag)))
	//	return E_FAIL;
	// 오브젝트 풀에 등록
	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Terrain"), strLayerTag, 1000)))
		return E_FAIL;

	// 오브젝트 풀링에서 가져와서 오브젝트 매니저에 추가
	if (FAILED(m_pGameInstance->Add_GameObject_FromPool(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, strLayerTag)))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Shop()
{
	CUI_Base::UI_Child_Desc Point_Shop{};  // 자식 UI는 3개만 소유 부모 상대적으로 위치 잡을꺼임
	Point_Shop.vSize = { 804.f,483.f };
	Point_Shop.fAlpha = 1.0f;
	Point_Shop.vPos = { 0.f,0.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Point_Shop_UI"),
		LEVEL_GAMEPLAY, TEXT("Layer_Point_Shop_1"), &Point_Shop)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc Episode_UI{};  // 자식 UI는 3개만 소유 부모 상대적으로 위치 잡을꺼임
	Episode_UI.vSize = { 846.f,508.f };
	Episode_UI.fAlpha = 1.0f;
	Episode_UI.vPos = { 0.f,0.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Episode_UI"),
		LEVEL_GAMEPLAY, TEXT("Layer_Spell_Shop_1"), &Episode_UI)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc Spell_Shop_UI{};  // 자식 UI는 3개만 소유 부모 상대적으로 위치 잡을꺼임
	Spell_Shop_UI.vSize = { 847.f,508.f };
	Spell_Shop_UI.fAlpha = 1.0f;
	Spell_Shop_UI.vPos = {0.f,0.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Spell_Shop_UI"),
		LEVEL_GAMEPLAY, TEXT("Layer_Upgrade_Weapon_1"), &Spell_Shop_UI)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc Upgrade_Weapon{};  // 자식 UI는 3개만 소유 부모 상대적으로 위치 잡을꺼임
	Upgrade_Weapon.vSize = {847.f,508.f };
	Upgrade_Weapon.fAlpha = 1.0f;
	Upgrade_Weapon.vPos = { 0.f,0.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Upgrade_Weapon_UI"),
		LEVEL_GAMEPLAY, TEXT("Layer_Upgrade_Weapon_1"), &Upgrade_Weapon)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Free"),
		LEVEL_GAMEPLAY, strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Player"),
		LEVEL_GAMEPLAY, strLayerTag)))
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
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Menu_Panel"),
		LEVEL_GAMEPLAY, TEXT("1"), &Menu_Panel)))
		return E_FAIL;



	CUI_Base::UI_Parent_Desc DefaultUI_Desc{}; // 부모 UI는 총 6개의 정보 소유

	DefaultUI_Desc.vSize = { 0,0 };
	DefaultUI_Desc.fAlpha = 1.0f;
	DefaultUI_Desc.vPos = { 0.f,0.f };
	DefaultUI_Desc.ProjMatrix = {};
	DefaultUI_Desc.ViewMatrix = {};
	DefaultUI_Desc.WorldMatrix = {};

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
	TEXT("Prototype_GameObject_Default_PlayerUI"),
	LEVEL_GAMEPLAY, TEXT("Layer_Default_PlayerUI"), &DefaultUI_Desc)))
	return E_FAIL;


	CUI_Base::UI_Child_Desc Left_Panel{};  // 왼쪽 하단 판넬
	Left_Panel.vSize = { 262,210 };
	Left_Panel.fAlpha = 1.0f;
	Left_Panel.vPos = { -510.f,-255.f }; // 부모위치 설정

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Left_Panel"),
		LEVEL_GAMEPLAY, TEXT("Layer_Left_Panel_UI_1"), &Left_Panel)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc Left_Panel_HP{};  // HP 바
	Left_Panel_HP.vSize = { 118.f,18.f };
	Left_Panel_HP.fAlpha = 1.0f;
	Left_Panel_HP.vPos = { 47.f,-80.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Hp_Bar"),
		LEVEL_GAMEPLAY, TEXT("Layer_Left_Panel_UI_2"), &Left_Panel_HP)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc Left_Panel_Player_Icon{};  // 플레이어 아이콘
	Left_Panel_Player_Icon.vSize = { 69.f,61.f };
	Left_Panel_Player_Icon.fAlpha = 1.0f;
	Left_Panel_Player_Icon.vPos = { -589.f,-314.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Player_Icon"),
		LEVEL_GAMEPLAY, TEXT("Layer_Left_Panel_UI_3"), &Left_Panel_Player_Icon)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc Left_Panel_Mana_Bar{};  // 플레이어 아이콘
	Left_Panel_Mana_Bar.vSize = { 120.f,21.f };
	Left_Panel_Mana_Bar.fAlpha = 1.0f;
	Left_Panel_Mana_Bar.vPos = { 22.f,-53.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_MP_Bar"),
		LEVEL_GAMEPLAY, TEXT("Layer_Left_Panel_UI_4"), &Left_Panel_Mana_Bar)))
		return E_FAIL;



	CUI_Base::UI_Child_Desc EXP_Desc{};  // 중앙 패널
	EXP_Desc.vSize = { 850,13.f };
	EXP_Desc.fAlpha = 1.0f;
	EXP_Desc.vPos = { 4.f,-353.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Mid_Panel"),
		LEVEL_GAMEPLAY, TEXT("Layer_Mid_Panel"), &EXP_Desc)))
		return E_FAIL;



	CUI_Base::UI_Child_Desc RIght_Panel{};  // 우하단 패널 
	Left_Panel.vSize = { 270,177 };
	Left_Panel.fAlpha = 1.0f;
	Left_Panel.vPos = { 505.f,-272.f }; // 부모위치 잡아주기

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Right_Bar"),
		LEVEL_GAMEPLAY, TEXT("Layer_Right_Panel_UI_1"), &Left_Panel)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc RIght_Panel_Bullet{};  // 자식 UI는 3개만 소유 부모 상대적으로 위치 잡을꺼임
	Left_Panel_HP.vSize = { 222.f,29.f };
	Left_Panel_HP.fAlpha = 1.0f;
	Left_Panel_HP.vPos = { 8.f,-59.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Bullet_Bar"),
		LEVEL_GAMEPLAY, TEXT("Layer_Right_Panel_UI_2"), &Left_Panel_HP)))
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
