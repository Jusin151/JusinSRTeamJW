#include "Loader.h"

#include "GameInstance.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "Camera_FirstPerson.h"
#include "Player.h"
#include "UI_Headers.h" // UI 헤더들 
#include "GamePlay_Button.h"
#include "JsonLoader.h"
#include "Sky.h"
#include "Weapon_Headers.h"
#include"Item_Icon.h"
#include "Weapon_Effect.h"
#include "Staff_Bullet.h"
#include "PointShop.h"
#include "UI_Point_Shop.h"
#include "UI_WeaponShop_UI.h"
#include "UI_Spell_Shop.h"
#include "SpellShop.h"
#include "WeaponShop.h"
#include "Episode_Hub.h"
#include "UI_Episode_Hub.h"
#include "Portal.h"


CLoader::CLoader(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device{ pGraphic_Device }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGraphic_Device);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	/* 자원로딩한다. */
	CLoader* pLoader = static_cast<CLoader*>(pArg);

 	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;

}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT		hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo();
		break;

	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlay();
		break;
	case LEVEL_EDITOR:
		hr = Loading_For_Editor();
		break;
	case LEVEL_TEST:
		hr = Loading_For_Test();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;


	return S_OK;
}
HRESULT CLoader::Loading_For_Logo()
{
 	lstrcpy(m_szLoadingText, TEXT("텍스쳐을(를) 로딩중입니다."));

	Add_To_Logo_Textures();

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("원형객체을(를) 로딩중입니다."));

	Add_To_Logo_Prototype();

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}
HRESULT CLoader::Loading_For_GamePlay()
{
	//lstrcpy(m_szLoadingText, TEXT("텍스쳐을(를) 로딩중입니다."));

	//Add_To_GamePlay_Textures();

	///* For.Prototype_Component_Texture_Player */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player"),
	//	CTexture::Create(m_pGraphic_Device, TEXT("../../Resources/Textures/Player/Player.png"), 1))))
	//	return E_FAIL;

	//lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	//Add_To_GamePlay_Buffer();


	//lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	//lstrcpy(m_szLoadingText, TEXT("원형객체을(를) 로딩중입니다."));

	//Add_To_GamePlay_Prototype();

	///* For.Prototype_GameObject_Player */
 //	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Player"),
	//	CPlayer::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Monster */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestMonster"),
	//	CTestMonster::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	//lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	//m_isFinished = true;

	/* For.Prototype_Component_Texture_Sky */

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBE, TEXT("../../Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
    return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 인벤 테스트 삭제 X
		TEXT("Prototype_GameObject_Inven"),
		CInventory::Create(m_pGraphic_Device))))
		return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 아이콘 테스트 삭제 X
    	TEXT("Prototype_GameObject_Icon"),
    	CItem_Icon::Create(m_pGraphic_Device)))) 
    	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 미니건 테스트 삭제 X
		TEXT("Prototype_GameObject_Minigun"),
		CMinigun::Create(m_pGraphic_Device))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 하베스터 테스트 삭제 X
		TEXT("Prototype_GameObject_Harvester"),
		CHarvester::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 소닉 테스트 삭제 X
		TEXT("Prototype_GameObject_Sonic"),
		CSonic::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 웨폰이펙트 테스트 삭제 X
		TEXT("Prototype_GameObject_Weapon_Effect"),
		CWeapon_Effect::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 스태프총알 테스트 삭제 X
		TEXT("Prototype_GameObject_Staff_Bullet"),
		CStaff_Bullet::Create(m_pGraphic_Device))))
		return E_FAIL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 게임플레이버튼 UI 삭제 X
		TEXT("Prototype_GameObject_GamePlayer_Button"),
		CGamePlay_Button::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 포인트샵 월드객체 삭제 X
		TEXT("Prototype_GameObject_Point_Shop"),
		CPoint_Shop::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 포인트샵 UI 삭제 X
		TEXT("Prototype_GameObject_UI_Point_Shop"),
		CUI_Point_Shop::Create(m_pGraphic_Device))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 웨폰샵 UI 삭제 X
		TEXT("Prototype_GameObject_UI_Weapon_Shop"),
		CUI_WeaponShop_UI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 웨폰샵 UI 삭제 X
		TEXT("Prototype_GameObject_UI_Spell_Shop"),
		CUI_Spell_Shop::Create(m_pGraphic_Device))))
		return E_FAIL;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//// 웨폰상점  UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Weapon_Shop_Display"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Upgrade_Weapon_UI/lweaponshop.png"), 1))))
		return E_FAIL;

	//// 웨폰상점 선택 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Weapon_Shop_Selected"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Upgrade_Weapon_UI/Weapon_Selected.png"), 1))))
		return E_FAIL;

	//// 스펠상점 선택 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Spell_Shop_DisPlay"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Spell_UI/Spell_UI.png"), 1))))
		return E_FAIL;


	////// 스펠상점 Selected UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Spell_Shop_Selected"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Spell_UI/Spell_UI_Selected.png"), 1))))
		return E_FAIL;

	////// 스펠상점 겟스펠 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Spell_Shop_Button"),
	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Spell_UI/Spell_BuyButton_%d.png"), 3))))
		return E_FAIL;



	// 웨폰샵 월드객체 삭제 X
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, 
		TEXT("Prototype_GameObject_Weapon_Shop"),
		CWeaponShop::Create(m_pGraphic_Device))))
		return E_FAIL;

	//// 웨폰상점  월드 객체 사진
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Weapon_Shop"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Hub/Gunsmith_station.png"), 1))))
		return E_FAIL;


	// 스펠샵 월드객체 삭제 X
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, 
		TEXT("Prototype_GameObject_Spell_Shop"),
		CSpellShop::Create(m_pGraphic_Device))))
		return E_FAIL;

	//// 스펠샵  월드객체 사진
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Spell_Shop"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Hub/Spellstation.png"), 1))))
		return E_FAIL;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 에피소드 UI 삭제 X
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_UI_Episode_Hub"),
		CUI_Episode_Hub::Create(m_pGraphic_Device))))
		return E_FAIL;


	//// 에피소드  UI 디스플레이 사진
  	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Episode_Hub_UI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Episode_UI/Episode_Display.png"), 1))))
		return E_FAIL;

	//// 에피소드  UI 초록색 셀렉트 박스
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Episode_Hub_UI_Selected"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Button/Episode_Selected/level_selected_%d.png"), 2))))
		return E_FAIL;

	//// 에피소드  UI 초록색 셀렉트 박스
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Episode_Hub_UI_Level_1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Button/Level_1/Level_1_%d.png"),14))))
		return E_FAIL;

	// 에피소드 월드객체 삭제 X
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Episode_Hub"),
		CEpisode_Hub::Create(m_pGraphic_Device))))
		return E_FAIL;

	//// 스펠샵  월드객체 사진
 	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Episode_Hub"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Hub/TEXTURE_HUB_desk_1.png"), 1))))
		return E_FAIL;

	// 포탈 월드객체 삭제 X
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Portal"),
		CPortal::Create(m_pGraphic_Device))))
		return E_FAIL;

	//// 포탈  월드객체 사진
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Portal"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Portal/Portal_%d.png"), 8))))
		return E_FAIL;




   	lstrcpy(m_szLoadingText, TEXT("JSON에서 프로토타입을 로딩중입니다."));

	// JSON 로더를 사용하여 모든 프로토타입 로드
	CJsonLoader jsonLoader;
  	if (FAILED(jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device,L"../Save/Prototypes.json")))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Editor()
{
	lstrcpy(m_szLoadingText, TEXT("JSON에서 프로토타입을 로딩중입니다."));
	CJsonLoader jsonLoader;
	// JSON 로더를 사용하여 모든 프로토타입 로드
	if (FAILED(jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Editor.json")))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Test()
{

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TEST, // 카메라 원형객체
	//	TEXT("Prototype_GameObject_Camera_Free"),
	//	CCamera_Free::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("JSON에서 프로토타입을 로딩중입니다."));
	CJsonLoader jsonLoader;
	// JSON 로더를 사용하여 모든 프로토타입 로드
	if (FAILED(jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Test.json")))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Add_To_Logo_Prototype()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pGraphic_Device))))
		return E_FAIL;

	Add_To_Logo_Menu_Prototype();

	return S_OK;
}

HRESULT CLoader::Add_To_Logo_Menu_Textures()
{
	// 검은색 메뉴바
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Menu"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Logo/SR_Real.png"), 1))))
		return E_FAIL;

	// 메뉴바 안의 시작버튼
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Menu_StartButton"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Logo/SR_Start.png"), 1))))
		return E_FAIL;

	// 메뉴바 안의 시작버튼 선택시
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Menu_StartButton_Select"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Logo/SR_Start_Select.png"), 1))))
		return E_FAIL;

	// 메뉴바 안의 종료버튼
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Menu_ExitButton"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Logo/SR_Exit.png"), 1))))
		return E_FAIL;

	// 메뉴바 안의 종료버튼 선택시
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Menu_ExitButton_Select"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Logo/SR_Exit_Select.png"), 1))))
		return E_FAIL;

	return S_OK;
}




HRESULT CLoader::Add_To_Logo_Menu_Prototype()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_GameObject_Menu"),
		CDefault_Menu::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_GameObject_Menu_StartButton"),
		CLogo_Button::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_GameObject_Menu_ExitButton"),
		CLogo_Button::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Add_To_Logo_Textures()
{
	//로고의 사람 // 3번째 렌더
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Logo/menu-warlock_v02_winter.png"), 1))))
		return E_FAIL;
	//로고의 검은 몬스터 // 4번째 렌더
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround_2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Logo/SR_Long_menu_potwory.png"), 1))))
		return E_FAIL;
	// 로고의 맨뒤 배경 // 1번째 렌더
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround_3"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Logo/menu_blyskawica_v01_winter.png"), 1))))
		return E_FAIL;
	// 로고의 맨뒤에서 2번째 몬스터 // 2번째 렌더
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround_4"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Logo/SR_menu_potwory_v01_winter.png"), 1))))
		return E_FAIL;

	Add_To_Logo_Menu_Textures();

	return S_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CLoader::Add_To_GamePlay_Textures()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 터레인 텍스쳐
		TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/Terrain/Tile0.jpg"), 1))))
		return E_FAIL;

	Add_To_UI_Textures(); // UI 텍스쳐

	return S_OK;
}

HRESULT CLoader::Add_To_GamePlay_Buffer()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,     // 터레인 버퍼
		TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, 256, 256))))
		return E_FAIL;
	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;



	return S_OK;
}

HRESULT CLoader::Add_To_GamePlay_Prototype()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 터레인 원형객체
		TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 카메라 원형객체
		TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 카메라 원형객체
		TEXT("Prototype_GameObject_Camera_FirstPerson"),
		CCamera_FirstPerson::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pGraphic_Device))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));




	


	Add_To_UI_Prototype(); //UI 원형 객체


	return S_OK;
}
HRESULT CLoader::Add_To_UI_Textures()// 텍스쳐 컴포넌트
{
	// 게임플레이 레벨 전체 UI의 캔버스 판넬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Default_PlayerUI"), 
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/UI/Default_UI.png"),
			1))))
		return E_FAIL;


	Add_To_UI_Left_DisPlay_Textures(); // 좌하단 디스플레이 텍스쳐

	Add_To_UI_Mid_DisPlay_Textures(); // 중단 디스플레이 텍스쳐

	Add_To_UI_Right_DisPlay_Textures(); // 우하단 디스플레이 텍스쳐

	Add_To_UI_Menu_DisPlay_Textures();


	return S_OK;
}


HRESULT CLoader::Add_To_UI_Prototype() //원형객체들
{
	// 게임플레이 레벨 전체 UI의 캔버스 판넬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Default_PlayerUI"),
		CUI_Default_Panel::Create(m_pGraphic_Device))))
		return E_FAIL;

	Add_To_UI_Left_DisPlay_Prototype();  //좌하단 디스플레이 원본생성

	Add_To_UI_Mid_DisPlay_Prototype();   // 중단 디스플레이 원본생성

	Add_To_UI_Right_DisPlay_Prototype(); // 우하단 디스플레이 원본생성

	Add_To_UI_Right_Menu_Prototype();


	return S_OK;
}
HRESULT CLoader::Add_To_UI_Left_DisPlay_Textures() 
{
	// 게임플레이 레벨 전체 UI의 캔버스 판넬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Left_Panel"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/UI/Left/SR_HUD_bottom_left.png"),
			1))))
		return E_FAIL;

	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Hp_Bar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/UI/Left/SR_HUD_bar_HP.png"),
			1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 기본 플레이어 이미지 4장
		TEXT("Prototype_Component_Texture_Player_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/UI/Left/Player_Icon/All/1_SR_face_unhurt_%d.png"),
			20))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_MP_Bar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/UI/Left/SR_HUD_bar_mana.png"),
			1))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Add_To_UI_Left_DisPlay_Prototype()
{
	// 왼쪽 레프트 하단 판넬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Left_Panel"),
		CUI_Left_Display::Create(m_pGraphic_Device))))
		return E_FAIL;


	// 플레이어아이콘
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Player_Icon"),
		CUI_Player_Icon::Create(m_pGraphic_Device))))
		return E_FAIL;


	// 체력바
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Hp_Bar"),
		CUI_HP_Bar::Create(m_pGraphic_Device))))
		return E_FAIL;


	// 플레이어아이콘
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_MP_Bar"),
		CUI_MP_Bar::Create(m_pGraphic_Device)))) 
		return E_FAIL;


	return S_OK;
}
HRESULT CLoader::Add_To_UI_Mid_DisPlay_Textures()
{
	// 중앙 판넬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Mid_Panel"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/UI/Middle/SR_EXP_Bar.png"),
			1))))
		return E_FAIL;


	return S_OK;
}


HRESULT CLoader::Add_To_UI_Mid_DisPlay_Prototype()
{
	// 중앙 판넬 원본 객체 생성
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Mid_Panel"),
		CUI_Mid_Display::Create(m_pGraphic_Device))))
		return E_FAIL;


	return S_OK;
}
HRESULT CLoader::Add_To_UI_Right_DisPlay_Textures()
{

	// 오른쪽 판넬 텍스쳐
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Right_Panel"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/UI/Right/SR_HUD_bottom_right.png"),
			1))))
		return E_FAIL;

	// 오른쪽 판넬의 총알 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Bullet_Bar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/UI/Right/SR_HUD_bar_ammo.png"),
			1))))
		return E_FAIL;


	return S_OK;
}


HRESULT CLoader::Add_To_UI_Right_DisPlay_Prototype()
{
	
	// 중앙 판넬 원본 객체 생성
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Right_Bar"),
		CUI_Right_Display::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 중앙 판넬의 총알 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Bullet_Bar"),
		CUI_Bullet_Bar::Create(m_pGraphic_Device))))
		return E_FAIL;


	return S_OK;
}
HRESULT CLoader::Add_To_UI_Menu_DisPlay_Textures()
{
	// 기본 메뉴 판넬 텍스쳐
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Menu_Panel"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/UI/Game_Menu.png"),
			1))))
		return E_FAIL;

	return S_OK;
}
HRESULT CLoader::Add_To_UI_Right_Menu_Prototype()
{
	// 기본 메뉴 판넬 프로토타입
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Menu_Panel"),
		CUI_Menu::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}



	//// 인벤 원형객체
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
	//	TEXT("Prototype_GameObject_Weapon_Claymore_Sh"),
	//	CStaff::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	//// 아이템 아이콘 원형객체
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
	//	TEXT("Prototype_GameObject_Weapon_Claymore_ShotGun"),
	//	CStaff::Create(m_pGraphic_Device))))
	//	return E_FAIL;
	



CLoader* CLoader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pGraphic_Device);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pGraphic_Device);


	DeleteCriticalSection(&m_CriticalSection);
}


