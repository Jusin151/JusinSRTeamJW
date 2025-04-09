#include "Level_Hub.h"
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
#include "Level_Loading.h"
#include "Sound_Event.h"
#include <Camera_FirstPerson.h>

CLevel_Hub::CLevel_Hub(LPDIRECT3DDEVICE9 pGraphic_Device)
 	: CLevel{ pGraphic_Device }
{

}
HRESULT CLevel_Hub::Initialize()

{
	CJsonLoader jsonLoader;
	
	//jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL_GAMEPLAY.json", LEVEL_GAMEPLAY);
	jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL_Hub.json", LEVEL_HUB);
	static _bool bIsLoading = false;

	if (!bIsLoading)
	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Inven_UI"),
			LEVEL_STATIC, TEXT("Layer_Inven_UI")))) 
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Inven"),
			LEVEL_STATIC, TEXT("Layer_Inven"))))
			return E_FAIL;


		if (FAILED(Ready_Layer_Weapon()))
			return E_FAIL;

		if (FAILED(Ready_Layer_Weapon_Icon()))
			return E_FAIL;

		if (FAILED(Ready_Layer_UI()))
			return E_FAIL;

		if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
			return E_FAIL;

		if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
			return E_FAIL;

		bIsLoading = true;

	}

	if (FAILED(Ready_Layer_Shop_UI()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_HUB, TEXT("Prototype_GameObject_Point_Shop"),
		LEVEL_HUB, TEXT("Layer_Point_Shop"))))
		return E_FAIL;


	//CUI_Base::UI_Parent_Desc = {};

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_HUB, TEXT("Prototype_GameObject_Weapon_Shop"),
		LEVEL_HUB, TEXT("Layer_Weapon_Shop"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_HUB, TEXT("Prototype_GameObject_Spell_Shop"),
		LEVEL_HUB, TEXT("Layer_Spell_Shop"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_HUB, TEXT("Prototype_GameObject_Episode_Hub"),
		LEVEL_HUB, TEXT("Layer_Shop"))))
		return E_FAIL;


	CGameObject* pPlayer = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	if (pPlayer)
	{
		CTransform* pTransform = static_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));
		if (nullptr == pTransform)
			return E_FAIL;
		pTransform->Set_State(CTransform::STATE_POSITION, _float3(-11.f, 1.f, 8.f));
	}

	CCamera_FirstPerson * pCamera = dynamic_cast<CCamera_FirstPerson*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Camera")));
	if (pCamera)
	{
		pCamera->Set_Yaw(D3DXToRadian(90.f));
	}

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_MiniMap"),
		LEVEL_HUB, TEXT("Layer_MiniMap"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Sky"),
		LEVEL_STATIC, TEXT("Layer_SkyBox"))))
		return E_FAIL;

	m_pGameInstance->Stop_All_Event();
	m_pGameInstance->Play_Background(L"event:/Backgrounds/003 All That Glitters Is Gold (Hub)").SetVolume(0.5f);
	CUI_Manager::GetInstance()->GetUI(L"Cursor")->SetActive(false);
	return S_OK;
}

void CLevel_Hub::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState('F') & 0x8000)
	{
		if (FAILED(m_pGameInstance->Process_LevelChange(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_GAMEPLAY))))
			return;
		return;
	}
	if (GetAsyncKeyState('H') & 0x8000)
	{
  		if (FAILED(m_pGameInstance->Process_LevelChange(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_HONG))))
			return;
		return;
	}


	if (GetKeyState('P') & 0x8000)
	{
		if (FAILED(m_pGameInstance->Process_LevelChange(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_BOSS))))
			return;
	}
}

HRESULT CLevel_Hub::Render()
{
	SetWindowText(g_hWnd, TEXT("허브 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Hub::Ready_Layer_BackGround(const _wstring& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_Hub::Ready_Layer_Weapon()
{
	CWeapon_Base::Weapon_DESC Weapon_Claymore_Desc{}; // 클레이 모어
	Weapon_Claymore_Desc.WeaponID = CWeapon_Base::WEAPON_ID::Claymore;
	Weapon_Claymore_Desc.vPos = { 330.f,-40.f };
	Weapon_Claymore_Desc.vSize = { 2048.,682.f };
	Weapon_Claymore_Desc.AttackSpeed = { 1.f };
	//Weapon_Claymore_Desc.TextureKey = L"Prototype_Component_Texture_Claymore";
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Weapon_Claymore"),
		LEVEL_STATIC, TEXT("Layer_Weapon_Claymore"),
		&Weapon_Claymore_Desc)))
		return E_FAIL;

	CWeapon_Base::Weapon_DESC Weapon_Axe_Desc{}; // 도끼
	Weapon_Axe_Desc.WeaponID = CWeapon_Base::WEAPON_ID::Axe;
	Weapon_Axe_Desc.vPos = { 0.f,-160.f };
	Weapon_Axe_Desc.vSize = { 1500,423.f };
	Weapon_Axe_Desc.AttackSpeed = { 1.f };
	//Weapon_Axe_Desc.TextureKey = L"Prototype_Component_Texture_Axe";
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Weapon_Axe"),
		LEVEL_STATIC, TEXT("Layer_Weapon_Axe"),
		&Weapon_Axe_Desc)))
		return E_FAIL;


	CWeapon_Base::Weapon_DESC Weapon_ShotGun_Desc{}; // 샷건
	Weapon_ShotGun_Desc.WeaponID = CWeapon_Base::WEAPON_ID::ShotGun;
	Weapon_ShotGun_Desc.vPos =  { 0.f,-170.f };
	Weapon_ShotGun_Desc.vSize = { 749,420.f };
	Weapon_ShotGun_Desc.AttackSpeed = { 1.f };
	Weapon_ShotGun_Desc.Damage = { 1 };
	Weapon_ShotGun_Desc.TextureKey =L"Prototype_Component_Texture_ShotGun";
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Weapon_ShotGun"),
		LEVEL_STATIC, TEXT("Layer_Weapon_ShotGun"),
		&Weapon_ShotGun_Desc)))
		return E_FAIL;

	CWeapon_Base::Weapon_DESC Weapon_Magnum_Desc{}; // 매그넘
	Weapon_Magnum_Desc.WeaponID = CWeapon_Base::WEAPON_ID::Magnum;
	Weapon_Magnum_Desc.vPos = { 70.f,-250.f };
	Weapon_Magnum_Desc.vSize = { 390.f,520.f };
	Weapon_Magnum_Desc.AttackSpeed = { 1.f };
	//Weapon_Magnum_Desc.TextureKey = L"Prototype_Component_Texture_Magnum";
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Weapon_Magnum"),
		LEVEL_STATIC, TEXT("Layer_Weapon_Magnum"),
		&Weapon_Magnum_Desc)))
		return E_FAIL;

	CWeapon_Base::Weapon_DESC Weapon_Staff_Desc{}; // 스태프 원형객체
	Weapon_Staff_Desc.WeaponID = CWeapon_Base::WEAPON_ID::Staff;
	Weapon_Staff_Desc.vPos = { -10.f,-220.f };
	Weapon_Staff_Desc.vSize = { 936,525.f };
	Weapon_Staff_Desc.Damage = { 100 };
	Weapon_Staff_Desc.AttackSpeed = { 1.f };
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Weapon_Staff"),
		LEVEL_STATIC, TEXT("Layer_Weapon_Staff"),
		&Weapon_Staff_Desc)))
		return E_FAIL;

	CWeapon_Base::Weapon_DESC Weapon_Minigun_Desc{}; // 미니건 원형객체
	Weapon_Minigun_Desc.WeaponID = CWeapon_Base::WEAPON_ID::Minigun;
	Weapon_Minigun_Desc.vPos = { 0.f,-200.f };
	Weapon_Minigun_Desc.vSize = { 959,347.f };
	Weapon_Minigun_Desc.Damage = { 100 };
	Weapon_Minigun_Desc.AttackSpeed = { 1.f };
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Minigun"),
		LEVEL_STATIC, TEXT("Layer_Weapon_Minigun"),
		&Weapon_Minigun_Desc)))
		return E_FAIL;


	CWeapon_Base::Weapon_DESC Weapon_Harvester_Desc{}; // 하베스터 원형객체
	Weapon_Harvester_Desc.WeaponID = CWeapon_Base::WEAPON_ID::Harvester;
	Weapon_Harvester_Desc.vPos = { 80.f,-213.f };
	Weapon_Harvester_Desc.vSize = { 356.f,375.f };
	Weapon_Harvester_Desc.Damage = { 100 };
	Weapon_Harvester_Desc.AttackSpeed = { 1.f };
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Harvester"),
		LEVEL_STATIC, TEXT("Layer_Weapon_Harvester"),
		&Weapon_Harvester_Desc)))
		return E_FAIL;

	CWeapon_Base::Weapon_DESC Weapon_Sonic_Desc{}; // 소닉 원형객체
	Weapon_Sonic_Desc.WeaponID = CWeapon_Base::WEAPON_ID::Sonic;
	Weapon_Sonic_Desc.vPos = { 270.f,-200.f };
	Weapon_Sonic_Desc.vSize = { 436.f,316.f };
	Weapon_Sonic_Desc.Damage = { 100 };
	Weapon_Sonic_Desc.AttackSpeed = { 1.f };
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Sonic"),
		LEVEL_STATIC, TEXT("Layer_Weapon_Sonic"),
		&Weapon_Sonic_Desc)))
		return E_FAIL;


	return S_OK;
}
HRESULT CLevel_Hub::Ready_Layer_Weapon_Icon()
{
	CImage::Image_DESC ClaymoreIcon_INFO = {};
	ClaymoreIcon_INFO.vPos = { -400.f,150.f };
	ClaymoreIcon_INFO.vSize = { 90.f,34.f };
	ClaymoreIcon_INFO.IMAGE_TYPE = CImage::IMAGE_TYPE::WEAPON_ICON;
	ClaymoreIcon_INFO.TextureKey = L"Prototype_Component_Texture_Weapon_Icon";
	ClaymoreIcon_INFO.WeaponTag = L"Claymore";
	ClaymoreIcon_INFO.TextureImageNum = CWeapon_Base::Claymore;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Image"),
		LEVEL_STATIC, TEXT("Layer_Image"), &ClaymoreIcon_INFO)))
		return E_FAIL;

	CImage::Image_DESC AxeIcon_INFO = {};
	AxeIcon_INFO.vPos = { -300.f,150.f };
	AxeIcon_INFO.vSize = { 100.f,50.f };
	AxeIcon_INFO.IMAGE_TYPE = CImage::IMAGE_TYPE::WEAPON_ICON;
	AxeIcon_INFO.TextureKey = L"Prototype_Component_Texture_Weapon_Icon";
	AxeIcon_INFO.WeaponTag = L"Axe";
	AxeIcon_INFO.TextureImageNum = CWeapon_Base::Axe;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Image"),
		LEVEL_STATIC, TEXT("Layer_Image"), &AxeIcon_INFO)))
		return E_FAIL;

	CImage::Image_DESC ShotGunIcon_INFO = {};
	ShotGunIcon_INFO.vPos = { -200.f,150.f };
	ShotGunIcon_INFO.vSize = { 100.f,50.f };
	ShotGunIcon_INFO.IMAGE_TYPE = CImage::IMAGE_TYPE::WEAPON_ICON;
	ShotGunIcon_INFO.TextureKey = L"Prototype_Component_Texture_Weapon_Icon";
	ShotGunIcon_INFO.WeaponTag = L"ShotGun";
	ShotGunIcon_INFO.TextureImageNum = CWeapon_Base::ShotGun;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Image"),
		LEVEL_STATIC, TEXT("Layer_Image"), &ShotGunIcon_INFO)))
		return E_FAIL;

	CImage::Image_DESC MagnumIcon_INFO = {};
	MagnumIcon_INFO.vPos = { -100.f,150.f };
	MagnumIcon_INFO.vSize = { 74.f,31.f };
	MagnumIcon_INFO.IMAGE_TYPE = CImage::IMAGE_TYPE::WEAPON_ICON;
	MagnumIcon_INFO.TextureKey = L"Prototype_Component_Texture_Weapon_Icon";
	MagnumIcon_INFO.WeaponTag = L"Magnum";
	MagnumIcon_INFO.TextureImageNum = CWeapon_Base::Magnum;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Image"),
		LEVEL_STATIC, TEXT("Layer_Image"), &MagnumIcon_INFO)))
		return E_FAIL;

	CImage::Image_DESC StaffIcon_INFO = {};
	StaffIcon_INFO.vPos = { 0.f,150.f };
	StaffIcon_INFO.vSize = { 80.f,40.f };
	StaffIcon_INFO.IMAGE_TYPE = CImage::IMAGE_TYPE::WEAPON_ICON;
	StaffIcon_INFO.TextureKey = L"Prototype_Component_Texture_Weapon_Icon";
	StaffIcon_INFO.WeaponTag = L"Staff";
	StaffIcon_INFO.TextureImageNum = CWeapon_Base::Staff;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Image"),
		LEVEL_STATIC, TEXT("Layer_Image"), &StaffIcon_INFO)))
		return E_FAIL;

	CImage::Image_DESC MinigunIcon_INFO = {};
	MinigunIcon_INFO.vPos = { 100.f,150.f };
	MinigunIcon_INFO.vSize = { 80.f,26.f };
	MinigunIcon_INFO.IMAGE_TYPE = CImage::IMAGE_TYPE::WEAPON_ICON;
	MinigunIcon_INFO.TextureKey = L"Prototype_Component_Texture_Weapon_Icon";
	MinigunIcon_INFO.WeaponTag = L"Minigun";
	MinigunIcon_INFO.TextureImageNum = CWeapon_Base::Minigun;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Image"),
		LEVEL_STATIC, TEXT("Layer_Image"), &MinigunIcon_INFO)))
		return E_FAIL;

	CImage::Image_DESC HarvesterIcon_INFO = {};
	HarvesterIcon_INFO.vPos = { 200.f,150.f };
	HarvesterIcon_INFO.vSize = { 100.f,41.f };
	HarvesterIcon_INFO.IMAGE_TYPE = CImage::IMAGE_TYPE::WEAPON_ICON;
	HarvesterIcon_INFO.TextureKey = L"Prototype_Component_Texture_Weapon_Icon";
	HarvesterIcon_INFO.WeaponTag = L"Harvester";
	HarvesterIcon_INFO.TextureImageNum = CWeapon_Base::Harvester;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Image"),
		LEVEL_STATIC, TEXT("Layer_Image"), &HarvesterIcon_INFO)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Hub::Ready_Layer_Shop_UI()
{
	CUI_Base::UI_Desc Point_Shop_UI{};
	Point_Shop_UI.vPos = { 0.f, 0.f };
	Point_Shop_UI.vSize = { 804.f, 482.f };

	if (FAILED(m_pGameInstance->Add_GameObject(
		LEVEL_HUB,
		TEXT("Prototype_GameObject_UI_Point_Shop"), // 포인트샵_UI 클론
		LEVEL_HUB,
		TEXT("Layer_UI_Point_Shop"),
		&Point_Shop_UI)))
		return E_FAIL;

	CUI_Base::UI_Desc Weapon_Shop_UI{};
	Weapon_Shop_UI.vPos = { 0.f, 0.f };
	Weapon_Shop_UI.vSize = { 804.f, 482.f };

	if (FAILED(m_pGameInstance->Add_GameObject(
		LEVEL_HUB,
		TEXT("Prototype_GameObject_UI_Weapon_Shop"),
		LEVEL_HUB,
		TEXT("Layer_UI_Weapon_Shop"),
		&Weapon_Shop_UI)))
		return E_FAIL;

	CUI_Base::UI_Desc Spell_Shop_UI{};
	Spell_Shop_UI.vPos = { 0.f, 0.f };
	Spell_Shop_UI.vSize = { 804.f, 482.f };

	if (FAILED(m_pGameInstance->Add_GameObject(
		LEVEL_HUB,
		TEXT("Prototype_GameObject_UI_Spell_Shop"),
		LEVEL_HUB,
		TEXT("Layer_UI_Spell_Shop"),
		&Spell_Shop_UI)))
		return E_FAIL;

	CUI_Base::UI_Desc Episode_Hub_UI{};
	Episode_Hub_UI.vPos = { 0.f, 0.f };
	Episode_Hub_UI.vSize = { 804.f, 482.f };

 	if (FAILED(m_pGameInstance->Add_GameObject(
		LEVEL_HUB,
		TEXT("Prototype_GameObject_UI_Episode_Hub"),
		LEVEL_HUB,
		TEXT("Layer_UI_Episode_Hub"),
		&Episode_Hub_UI)))
		return E_FAIL;

	return S_OK;
}
HRESULT CLevel_Hub::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_FirstPerson"),
		LEVEL_STATIC, strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Hub::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Player"),
		LEVEL_STATIC, strLayerTag)))
 		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Hub::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	

	return S_OK;
}

HRESULT CLevel_Hub::Ready_Layer_UI()
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

	CUI_Base::UI_Child_Desc RIght_Panel_Bullet{};  // 자식 UI는 3개만 소유 부모 상대적으로 위치 잡을꺼임
	Left_Panel_HP.vSize = { 222.f,29.f };
	Left_Panel_HP.fAlpha = 1.0f;
	Left_Panel_HP.vPos = { 8.f,-59.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Bullet_Bar"),
		LEVEL_STATIC, TEXT("Layer_Right_Panel_UI_2"), &Left_Panel_HP)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Hit_Blood"),
		LEVEL_STATIC, TEXT("Layer_Blood"))))
		return E_FAIL;

	return S_OK;
}

CLevel_Hub* CLevel_Hub::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Hub* pInstance = new CLevel_Hub(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Hub");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Hub::Free()
{
	__super::Free();
	
	Safe_Release(m_pPickingSys);
}
