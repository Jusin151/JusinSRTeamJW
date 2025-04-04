#include "Loader.h"

#include "GameInstance.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "Camera_FirstPerson.h"
#include "Player.h"
#include "UI_Headers.h" // UI 헤더들 
#include "Hub_Headers.h" // 허브 헤더들
#include "Weapon_Headers.h" // 무기 헤더들 
#include "GamePlay_Button.h"
#include "JsonLoader.h"
#include "Sky.h"
#include "Weapon_Headers.h"
#include "Weapon_Effect.h"
#include "Staff_Bullet.h"
#include "Image.h"
#include "Inven_UI.h"
#include "Level_Hub.h"



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
      	m_pGameInstance->Set_LevelState(CGameInstance::LEVEL_STATE::CHANGING);
	HRESULT		hr = {};
	m_pGameInstance->Set_LevelState(CGameInstance::LEVEL_STATE::CHANGING);
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
	case LEVEL_HUB:
		hr = Loading_For_Hub();
		break;
	}
 	m_pGameInstance->Set_LevelState(CGameInstance::LEVEL_STATE::NORMAL); 
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

 	//m_pGameInstance->Load_Bank(L"Background");

 	lstrcpy(m_szLoadingText, TEXT("원형객체을(를) 로딩중입니다."));

	Add_To_Logo_Prototype();

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	m_pGameInstance->Set_LevelState(CGameInstance::LEVEL_STATE::NORMAL);
	return S_OK;
}
HRESULT CLoader::Loading_For_GamePlay()
{
	

   	lstrcpy(m_szLoadingText, TEXT("JSON에서 프로토타입을 로딩중입니다."));

	// JSON 로더를 사용하여 모든 프로토타입 로드
	CJsonLoader jsonLoader;
 	if (FAILED(jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device,L"../Save/Prototypes.json")))
	return E_FAIL;

	// JSON 로더를 사용하여 모든 프로토타입 로드
	if (FAILED(jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Test.json")))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Hub()
{

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, // 게임플레이버튼 UI 
		TEXT("Prototype_GameObject_GamePlayer_Button"),
		CGamePlay_Button::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, // 포인트샵 월드객체 
		TEXT("Prototype_GameObject_Point_Shop"),
		CHub_PointShop::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, // 포인트샵 UI객체 
		TEXT("Prototype_GameObject_UI_Point_Shop"),
		CUI_Point_Shop::Create(m_pGraphic_Device))))
		return E_FAIL;

	//// 포인트샵UI객체  사진
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, TEXT("Prototype_Component_Texture_UI_Point_Shop"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Point_Shop/Point_Shop_UI_0.png"), 1))))
		return E_FAIL;


	// 웨폰샵 월드객체 삭제 X
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB,
		TEXT("Prototype_GameObject_Weapon_Shop"),
		CHub_WeaponShop::Create(m_pGraphic_Device))))
		return E_FAIL;

	//// 웨폰상점  월드 객체 사진
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, TEXT("Prototype_Component_Texture_Weapon_Shop"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Hub/Gunsmith_station.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, // 웨폰샵 UI 삭제 X
		TEXT("Prototype_GameObject_UI_Weapon_Shop"),
		CUI_WeaponShop_UI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, // 웨폰샵 UI 삭제 X
		TEXT("Prototype_GameObject_UI_Spell_Shop"),
		CUI_Spell_Shop::Create(m_pGraphic_Device))))
		return E_FAIL;


	//// 웨폰샵  UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, TEXT("Prototype_Component_Texture_Weapon_Shop_Display"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Upgrade_Weapon_UI/lweaponshop.png"), 1))))
		return E_FAIL;

	//// 웨폰샵 선택 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, TEXT("Prototype_Component_Texture_Weapon_Shop_Selected"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Upgrade_Weapon_UI/Weapon_Selected.png"), 1))))
		return E_FAIL;

	// 스펠샵 월드객체 삭제 X
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB,
		TEXT("Prototype_GameObject_Spell_Shop"),
		CHub_SpellShop::Create(m_pGraphic_Device))))
		return E_FAIL;

	//// 스펠샵  월드객체 사진
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, TEXT("Prototype_Component_Texture_Spell_Shop"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Hub/Spellstation.png"), 1))))
		return E_FAIL;

	//// 스펠상점 선택 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, TEXT("Prototype_Component_Texture_Spell_Shop_DisPlay"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Spell_UI/Spell_UI.png"), 1))))
		return E_FAIL;


	////// 스펠상점 Selected UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, TEXT("Prototype_Component_Texture_Spell_Shop_Selected"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Spell_UI/Spell_UI_Selected.png"), 1))))
		return E_FAIL;

	////// 스펠상점 겟스펠 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, TEXT("Prototype_Component_Texture_Spell_Shop_Button"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Spell_UI/Spell_BuyButton_%d.png"), 3))))
		return E_FAIL;


	// 에피소드 월드객체 삭제 X
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB,
		TEXT("Prototype_GameObject_Episode_Hub"),
		CHub_Episode::Create(m_pGraphic_Device))))
		return E_FAIL;

	//// 에피소드  월드객체 사진
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, TEXT("Prototype_Component_Texture_Episode_Hub"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Hub/TEXTURE_HUB_desk_1.png"), 1))))
		return E_FAIL;

	//// 에피소드  UI 디스플레이 사진
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, TEXT("Prototype_Component_Texture_Episode_Hub_UI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Episode_UI/Episode_Display.png"), 1))))
		return E_FAIL;

	// 에피소드 UI 삭제 X
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB,
		TEXT("Prototype_GameObject_UI_Episode_Hub"),
		CUI_Episode_Hub::Create(m_pGraphic_Device))))
		return E_FAIL;

	//// 에피소드  UI 초록색 셀렉트 박스
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, TEXT("Prototype_Component_Texture_Episode_Hub_UI_Selected"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Button/Episode_Selected/level_selected_%d.png"), 2))))
		return E_FAIL;

	//// 에피소드  UI 초록색 셀렉트 박스
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, TEXT("Prototype_Component_Texture_Episode_Hub_UI_Level_1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Button/Level_1/Level_1_%d.png"), 14))))
		return E_FAIL;

	// 포탈 월드객체 삭제 X
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB,
		TEXT("Prototype_GameObject_Portal"),
		CHub_Portal::Create(m_pGraphic_Device))))
		return E_FAIL;

	//// 포탈  월드객체 사진
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HUB, TEXT("Prototype_Component_Texture_Portal"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Portal/Portal_%d.png"), 8))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("JSON에서 프로토타입을 로딩중입니다."));

	// JSON 로더를 사용하여 모든 프로토타입 로드
	CJsonLoader jsonLoader;



	jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Hub.json"); // 건물관련

	// JSON 로더를 사용하여 모든 프로토타입 로드
	if (FAILED(jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Test.json"))) // 명훈이형꺼 관련
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	m_pGameInstance->Set_LevelState(CGameInstance::LEVEL_STATE::NORMAL);

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

	m_pGameInstance->Set_LevelState(CGameInstance::LEVEL_STATE::NORMAL);
	return S_OK;
}

HRESULT CLoader::Loading_For_Test()
{

	lstrcpy(m_szLoadingText, TEXT("JSON에서 프로토타입을 로딩중입니다."));
	CJsonLoader jsonLoader;
	// JSON 로더를 사용하여 모든 프로토타입 로드
	if (FAILED(jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Test.json")))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;
	m_pGameInstance->Set_LevelState(CGameInstance::LEVEL_STATE::NORMAL);
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
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Logo/menu-warlock_v03.png"), 1))))
		return E_FAIL;
	//로고의 검은 몬스터 // 4번째 렌더
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround_2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Logo/SR_Long_menu_potwory.png"), 1))))
		return E_FAIL;
	// 로고의 맨뒤 배경 // 1번째 렌더
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround_3"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Logo/menu_blyskawica_v01.png"), 1))))
		return E_FAIL;
	// 로고의 맨뒤에서 2번째 몬스터 // 2번째 렌더
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround_4"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Logo/menu_potwory_v01.png"), 1))))
		return E_FAIL;

	Add_To_Logo_Menu_Textures();

	return S_OK;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



HRESULT CLoader::Add_To_GamePlay_Prototype()
{

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, // 카메라 원형객체
		TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, // 카메라 원형객체
		TEXT("Prototype_GameObject_Camera_FirstPerson"),
		CCamera_FirstPerson::Create(m_pGraphic_Device))))
		return E_FAIL;

	///* For.Prototype_GameObject_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	


	return S_OK;
}


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


