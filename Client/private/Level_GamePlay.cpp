#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "PickingSys.h"
#include "Collider_Sphere.h"
#include "UI_Default_Panel.h"
#include "CUI_Base.h"
#include "GamePlay_Button.h"


CLevel_GamePlay::CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel { pGraphic_Device } ,
	m_pPickingSys{CPickingSys::Get_Instance()}

{
	m_pPickingSys->Initialize(g_hWnd, m_pGraphic_Device,m_pGameInstance);
	Safe_AddRef(m_pPickingSys);
}
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
	
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	m_pPickingSys->Update();

	auto colliderVec = m_pGameInstance->Get_Colliders();

	for (auto& colliderList : colliderVec)
	{
		for (auto& collider : colliderList)
		{
			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{
				if (m_pPickingSys->Ray_Intersection(collider))
				{
					int a = 10;
				}
			}
		}
	}
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
	
	CUI_Base::UI_Child_Desc Spell_Shop_UI{};  // 자식 UI는 3개만 소유 부모 상대적으로 위치 잡을꺼임
	Spell_Shop_UI.vSize = { 847.f,508.f };
	Spell_Shop_UI.fAlpha = 1.0f;
	Spell_Shop_UI.vPos = {-200.f,100.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Spell_Shop_UI"),
		LEVEL_GAMEPLAY, TEXT("Layer_Spell_Shop_1"), &Spell_Shop_UI)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc Upgrade_Weapon{};  // 자식 UI는 3개만 소유 부모 상대적으로 위치 잡을꺼임
	Upgrade_Weapon.vSize = {847.f,508.f };
	Upgrade_Weapon.fAlpha = 1.0f;
	Upgrade_Weapon.vPos = { -200.f,-100.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Upgrade_Weapon_UI"),
		LEVEL_GAMEPLAY, TEXT("Layer_Upgrade_Weapon_1"), &Upgrade_Weapon)))
		return E_FAIL;

	Ready_Layer_Episode_Button();
	Ready_Layer_Point_Shop_Button();

	return S_OK;
}

#include "GamePlay_Button.h"

// ...

HRESULT CLevel_GamePlay::Ready_Layer_Episode_Button() // 레벨은 큰 라운드 스테이지는 레벨안의 스테이지
{
	CGamePlay_Button::GamePlayer_Button_Desc Level_1_Display{}; // 레벨 5 이미지의 첫번째 스테이지
	Level_1_Display.Button_Desc.vSize = { 846.f,508.f };
	Level_1_Display.Button_Desc.vPos = { 0.f,0.f };
	Level_1_Display.strTexture_Default_Tag = { L"Prototype_Component_Texture_Episode_Level_1_UI" };
	Level_1_Display.strUIName = { L"Level_1_Display" };
	Level_1_Display.Button_type = CGamePlay_Button::Episode;
	Level_1_Display.bDisplay_On = true;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GamePlayer_Button"),
		LEVEL_LOGO, TEXT("Layer_Episode_0"),
		&Level_1_Display)))
		return E_FAIL;



    CGamePlay_Button::GamePlayer_Button_Desc Level_1_Icon_Desc{}; // 왼쪽 레벨 1 이미지 ( ex 후드쓴 개구리 
    Level_1_Icon_Desc.Button_Desc.vSize = { 147.f,72.f };
    Level_1_Icon_Desc.Button_Desc.vPos = { -320.f,184.f };
    Level_1_Icon_Desc.strTexture_Default_Tag = { L"Prototype_Component_Texture_Episode_Level_1_UI" };
    Level_1_Icon_Desc.strUIName = { L"Level_1_Icon" };
    Level_1_Icon_Desc.Episode_Button_Type.bLevel_Icon_Button_Flag = true;
	Level_1_Icon_Desc.Button_type = CGamePlay_Button::Button_type::Episode;
    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY,TEXT("Prototype_GameObject_GamePlayer_Button"),
        LEVEL_LOGO, TEXT("Layer_Episode_2"), 
        &Level_1_Icon_Desc)))
        return E_FAIL;

	CGamePlay_Button::GamePlayer_Button_Desc Level_1_Level_Stage_Desc{}; // 레벨 1 이미지의 첫번째 스테이지
	Level_1_Level_Stage_Desc.Button_Desc.vSize = { 192.f,192.f };
	Level_1_Level_Stage_Desc.Button_Desc.vPos = { -112.f,121.f };
	Level_1_Level_Stage_Desc.strTexture_Default_Tag = { L"Prototype_Component_Texture_Episode_Level_1_UI" };
	Level_1_Level_Stage_Desc.strUIName = { L"Level_1_01_Stage" };
	Level_1_Level_Stage_Desc.Episode_Button_Type.bLevel_01_Stage_Button_Flag = true;
	Level_1_Level_Stage_Desc.Button_type = CGamePlay_Button::Button_type::Episode;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GamePlayer_Button"),
		LEVEL_LOGO, TEXT("Layer_Episode_3"),
		&Level_1_Level_Stage_Desc)))
		return E_FAIL;

	CGamePlay_Button::GamePlayer_Button_Desc Level_2_Level_Stage_Desc{}; // 레벨 2 이미지의 첫번째 스테이지
	Level_2_Level_Stage_Desc.Button_Desc.vSize = { 192.f,192.f };
	Level_2_Level_Stage_Desc.Button_Desc.vPos = { 87.f,121.f };
	Level_2_Level_Stage_Desc.strTexture_Default_Tag = { L"Prototype_Component_Texture_Episode_Level_1_UI" };
	Level_2_Level_Stage_Desc.strUIName = { L"Level_1_02_Stage" };
	Level_2_Level_Stage_Desc.Episode_Button_Type.bLevel_02_Stage_Button_Flag = true;
	Level_2_Level_Stage_Desc.Button_type = CGamePlay_Button::Button_type::Episode;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GamePlayer_Button"),
		LEVEL_LOGO, TEXT("Layer_Episode_4"),
		&Level_2_Level_Stage_Desc)))
		return E_FAIL;

	CGamePlay_Button::GamePlayer_Button_Desc Level_3_Level_Stage_Desc{}; // 레벨 3 이미지의 첫번째 스테이지
	Level_3_Level_Stage_Desc.Button_Desc.vSize = { 192.f,192.f };
	Level_3_Level_Stage_Desc.Button_Desc.vPos = { 288.f,121.f };
	Level_3_Level_Stage_Desc.strTexture_Default_Tag = { L"Prototype_Component_Texture_Episode_Level_1_UI" };
	Level_3_Level_Stage_Desc.strUIName = { L"Level_1_03_Stage" };
	Level_3_Level_Stage_Desc.Episode_Button_Type.bLevel_03_Stage_Button_Flag = true;
	Level_3_Level_Stage_Desc.Button_type = CGamePlay_Button::Button_type::Episode;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GamePlayer_Button"),
		LEVEL_LOGO, TEXT("Layer_Episode_5"),
		&Level_3_Level_Stage_Desc)))
		return E_FAIL;

	CGamePlay_Button::GamePlayer_Button_Desc Level_4_Level_Stage_Desc{}; // 레벨 4 이미지의 첫번째 스테이지
	Level_4_Level_Stage_Desc.Button_Desc.vSize = { 192.f,192.f };
	Level_4_Level_Stage_Desc.Button_Desc.vPos = {-112.f,-76.f };
	Level_4_Level_Stage_Desc.strTexture_Default_Tag = { L"Prototype_Component_Texture_Episode_Level_1_UI" };
	Level_4_Level_Stage_Desc.strUIName = { L"Level_1_04_Stage" };
	Level_4_Level_Stage_Desc.Episode_Button_Type.bLevel_04_Stage_Button_Flag = true;
	Level_4_Level_Stage_Desc.Button_type = CGamePlay_Button::Button_type::Episode;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GamePlayer_Button"),
		LEVEL_LOGO, TEXT("Layer_Episode_6"),
		&Level_4_Level_Stage_Desc)))
		return E_FAIL;

	CGamePlay_Button::GamePlayer_Button_Desc Level_5_Level_Stage_Desc{}; // 레벨 5 이미지의 첫번째 스테이지
	Level_5_Level_Stage_Desc.Button_Desc.vSize = { 192.f,192.f };
	Level_5_Level_Stage_Desc.Button_Desc.vPos = { 87.f,-76.f };
	Level_5_Level_Stage_Desc.strTexture_Default_Tag = { L"Prototype_Component_Texture_Episode_Level_1_UI" };
	Level_5_Level_Stage_Desc.strUIName = { L"Level_1_05_Stage" };
	Level_5_Level_Stage_Desc.Episode_Button_Type.bLevel_05_Stage_Button_Flag = true;
	Level_5_Level_Stage_Desc.Button_type = CGamePlay_Button::Button_type::Episode;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GamePlayer_Button"),
		LEVEL_LOGO, TEXT("Layer_Episode_7"),
		&Level_5_Level_Stage_Desc)))
		return E_FAIL;


    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Point_Shop_Button()
{
	CGamePlay_Button::GamePlayer_Button_Desc Level_Poinst_Shop_Display{}; // 포인트 상점
	Level_Poinst_Shop_Display.Button_Desc.vSize = { 804.f,482.f };
	Level_Poinst_Shop_Display.Button_Desc.vPos = { 0.f,0.f };
	Level_Poinst_Shop_Display.strTexture_Default_Tag = { L"Prototype_Component_Texture_Point_Shop_UI" };
	Level_Poinst_Shop_Display.strUIName = { L"Level_Point_Shop_Display" };
	Level_Poinst_Shop_Display.Button_type = CGamePlay_Button::Button_type::Point_Shop;

		
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GamePlayer_Button"),
		LEVEL_LOGO, TEXT("Layer_Point_Shop_Display"),
		&Level_Poinst_Shop_Display)))
		return E_FAIL;
	 
	vector<CGamePlay_Button::GamePlayer_Button_Desc> vecButtonDescs(12);

	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			int index = i * 2 + j;
			vecButtonDescs[index].Button_Desc.vSize = { 211.f, 32.f };
			vecButtonDescs[index].Button_Desc.vPos = { 10.f*j,10.f*i }; // 위치 조정
			vecButtonDescs[index].strTexture_Default_Tag = L"Prototype_Component_Texture_Point_Shop_UI";
			vecButtonDescs[index].strUIName = L"Level_Point_Shop_Selected_" + to_wstring(index);
			vecButtonDescs[index].Button_type = CGamePlay_Button::Button_type::Point_Shop;
			vecButtonDescs[index].Point_Shop_Seleted[0] = true;

			if (FAILED(m_pGameInstance->Add_GameObject(
				LEVEL_GAMEPLAY,
				TEXT("Prototype_GameObject_GamePlayer_Button"),
				LEVEL_LOGO,
				TEXT("Layer_Point_Shop_Selected"),
				&vecButtonDescs[index])))
			{
				return E_FAIL;
			}
		}
	}



	return S_OK;
}



HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_FirstPerson"),
		LEVEL_GAMEPLAY, strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Player"),
	//	LEVEL_GAMEPLAY, strLayerTag)))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Player"), strLayerTag, 1)))
		return E_FAIL;
	


	CTransform::TRANSFORM_DESC randTransDesc{};


	

	randTransDesc.fRotationPerSec = D3DXToRadian(90.f);
	randTransDesc.fSpeedPerSec = 10.f;
	randTransDesc.vPos = { _float(rand() % 50),5.f,_float(rand() % 50) };
	if (FAILED(m_pGameInstance->Add_GameObject_FromPool(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, strLayerTag,&randTransDesc)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{

	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestMonster"), strLayerTag, 10)))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_GameObject_FromPool(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, strLayerTag)))
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

	Safe_Release(m_pPickingSys);
	CPickingSys::Destroy_Instance();

}
