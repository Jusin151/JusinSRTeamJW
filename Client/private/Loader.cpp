#include "Loader.h"

#include "GameInstance.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "Player.h"
#include "TestMonster.h"
#include "UI_Headers.h" // UI 헤더들 

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

	Add_To_Logo_Buffer();

	Add_To_Logo_Transform();

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("원형객체을(를) 로딩중입니다."));

	Add_To_Logo_Prototype();

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}
HRESULT CLoader::Loading_For_GamePlay()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐을(를) 로딩중입니다."));

	Add_To_GamePlay_Textures();

	/* For.Prototype_Component_Texture_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player"),
		CTexture::Create(m_pGraphic_Device, TEXT("../../Resources/Textures/Player/Player.png"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	Add_To_GamePlay_Buffer();

	Add_To_GamePlay_Transform();

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("원형객체을(를) 로딩중입니다."));

	Add_To_GamePlay_Prototype();

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pGraphic_Device))))
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
		CTexture::Create(m_pGraphic_Device, TEXT("../../Resources/Textures/UI/Logo/SR_Real.png"), 1))))
		return E_FAIL;

	// 메뉴바 안의 시작버튼
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Menu_StartButton"),
		CTexture::Create(m_pGraphic_Device, TEXT("../../Resources/Textures/UI/Logo/SR_Start.png"), 1))))
		return E_FAIL;

	// 메뉴바 안의 시작버튼 선택시
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Menu_StartButton_Select"),
		CTexture::Create(m_pGraphic_Device, TEXT("../../Resources/Textures/UI/Logo/SR_Start_Select.png"), 1))))
		return E_FAIL;

	// 메뉴바 안의 종료버튼
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Menu_ExitButton"),
		CTexture::Create(m_pGraphic_Device, TEXT("../../Resources/Textures/UI/Logo/SR_Exit.png"), 1))))
		return E_FAIL;

	// 메뉴바 안의 종료버튼 선택시
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Menu_ExitButton_Select"),
		CTexture::Create(m_pGraphic_Device, TEXT("../../Resources/Textures/UI/Logo/SR_Exit_Select.png"), 1))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Add_To_Logo_Menu_Buffer()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_VIBuffer_Rect_Menu"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_VIBuffer_Menu_StartButton"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_VIBuffer_Menu_ExitButton"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Add_To_Logo_Menu_Transform()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Transform_Menu"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Transform_Menu_StartButton"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Transform_Menu_ExitButton"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Add_To_Logo_Menu_Prototype()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_GameObject_Menu"),
		CDefault_Menu::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_GameObject_Menu_StartButton"),
		CStart_Button::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_GameObject_Menu_ExitButton"),
		CExit_Button::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Add_To_Logo_Textures()
{
	//로고의 사람 // 3번째 렌더
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pGraphic_Device, TEXT("../../Resources/Textures/UI/Logo/menu-warlock_v02_winter.png"), 1))))
		return E_FAIL;
	//로고의 검은 몬스터 // 4번째 렌더
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround_2"),
		CTexture::Create(m_pGraphic_Device, TEXT("../../Resources/Textures/UI/Logo/SR_Long_menu_potwory.png"), 1))))
		return E_FAIL;
	// 로고의 맨뒤 배경 // 1번째 렌더
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround_3"),
		CTexture::Create(m_pGraphic_Device, TEXT("../../Resources/Textures/UI/Logo/menu_blyskawica_v01_winter.png"), 1))))
		return E_FAIL;
	// 로고의 맨뒤에서 2번째 몬스터 // 2번째 렌더
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround_4"),
		CTexture::Create(m_pGraphic_Device, TEXT("../../Resources/Textures/UI/Logo/SR_menu_potwory_v01_winter.png"), 1))))
		return E_FAIL;

	Add_To_Logo_Menu_Textures();

	return S_OK;
}

HRESULT CLoader::Add_To_Logo_Buffer()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_VIBuffer_Rect_BackGround"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	Add_To_Logo_Menu_Buffer();

	return S_OK;
}

HRESULT CLoader::Add_To_Logo_Transform()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Transform_BackGround"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	Add_To_Logo_Menu_Transform();
	return S_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CLoader::Add_To_GamePlay_Textures()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 터레인 텍스쳐
		TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pGraphic_Device,
			TEXT("../../Resources/Textures/Terrain/Tile0.jpg"), 1))))
		return E_FAIL;

	Add_To_UI_Textures();

	return S_OK;
}

HRESULT CLoader::Add_To_GamePlay_Buffer()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,     // 터레인 버퍼
		TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, 256, 256))))
		return E_FAIL;

	Add_To_UI_Buffer();

	return S_OK;
}

HRESULT CLoader::Add_To_GamePlay_Transform()
{
	Add_To_UI_Transform();

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
		TEXT("Prototype_GameObject_Monster"),
		CTestMonster::Create(m_pGraphic_Device))))
		return E_FAIL;

	Add_To_UI_Prototype();
	return S_OK;
}
HRESULT CLoader::Add_To_UI_Textures()// 텍스쳐 컴포넌트
{
	// 게임플레이 레벨 전체 UI의 캔버스 판넬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Default_PlayerUI"),
		CTexture::Create(m_pGraphic_Device,
			TEXT("../../Resources/Textures/UI/Default_UI.png"),
			1))))
		return E_FAIL;


	Add_To_UI_Left_DisPlay_Textures(); // 좌하단 디스플레이 텍스쳐

	Add_To_UI_Mid_DisPlay_Textures(); // 중단 디스플레이 텍스쳐

	Add_To_UI_Right_DisPlay_Textures(); // 우하단 디스플레이 텍스쳐

	Add_To_UI_Menu_DisPlay_Textures();

	Add_To_Shop_Textures();



	return S_OK;
}
HRESULT CLoader::Add_To_UI_Buffer()//버퍼 컴포넌트
{
	// 게임플레이 레벨 전체 UI의 캔버스 판넬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_VIBuffer_PlayerUI"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;


	Add_To_UI_Left_DisPlay_Buffer(); // 좌하단 디스플레이 버퍼

	Add_To_UI_Mid_DisPlay_Buffer(); // 중단 디스플레이 버퍼

	Add_To_UI_Right_DisPlay_Buffer(); // 우하단 디스플레이 버퍼

	Add_To_UI_Menu_DisPlay_Buffer();

	Add_To_UI_Menu_DisPlay_Transform();

	Add_To_Shop_Buffer();
	

	return S_OK;
}
HRESULT CLoader::Add_To_UI_Transform() // 트랜스폼 컴포넌트
{
	// 게임플레이 레벨 전체 UI의 캔버스 판넬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Transform_Default"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	Add_To_UI_Left_DisPlay_Transform();  // 좌하단 디스플레이 트랜스폼

	Add_To_UI_Mid_DisPlay_Transform();   // 중단 디스플레이 트랜스폼

	Add_To_UI_Right_DisPlay_Transform(); // 우하단디스플레이 트랜스폼

	Add_To_Shop_Transform();


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

	Add_To_Shop_Prototype();

	return S_OK;
}
HRESULT CLoader::Add_To_UI_Left_DisPlay_Textures() 
{
	// 게임플레이 레벨 전체 UI의 캔버스 판넬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Left_Panel"),
		CTexture::Create(m_pGraphic_Device,
			TEXT("../../Resources/Textures/UI/Left/SR_HUD_bottom_left.png"),
			1))))
		return E_FAIL;

	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Hp_Bar"),
		CTexture::Create(m_pGraphic_Device,
			TEXT("../../Resources/Textures/UI/Left/SR_HUD_bar_HP.png"),
			1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, // 기본 플레이어 이미지 4장
		TEXT("Prototype_Component_Texture_Player_Icon"),
		CTexture::Create(m_pGraphic_Device,
			TEXT("../../Resources/Textures/UI/Left/Player_Icon/All/1_SR_face_unhurt_%d.png"),
			20))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_MP_Bar"),
		CTexture::Create(m_pGraphic_Device,
			TEXT("../../Resources/Textures/UI/Left/SR_HUD_bar_mana.png"),
			1))))
		return E_FAIL;

	return S_OK;
}
HRESULT CLoader::Add_To_UI_Left_DisPlay_Buffer()
{
	// 좌하단 판넬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_VIBuffer_Left_Panel"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 체력바
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_VIBuffer_Hp_Bar"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 플레이어 아이콘
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_VIBuffer_Player_Icon"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 마나바
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_VIBuffer_MP_Bar"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}
HRESULT CLoader::Add_To_UI_Left_DisPlay_Transform()
{
	// 좌하단 판넬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Transform_Left_Panel"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 체력바
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Transform_Hp_Bar"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 플레이어 아이콘
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Transform_Player_Icon"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 마나바
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Transform_MP_Bar"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;


	return S_OK;
}
HRESULT CLoader::Add_To_UI_Left_DisPlay_Prototype()
{
	// 왼쪽 레프트 하단 판넬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Left_Panel"),
		CUI_HPDisplay::Create(m_pGraphic_Device))))
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
		CTexture::Create(m_pGraphic_Device,
			TEXT("../../Resources/Textures/UI/Middle/SR_EXP_Bar.png"),
			1))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Add_To_UI_Mid_DisPlay_Buffer()
{

	// 중앙 판넬 버퍼
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_VIBuffer_Mid_Panel"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;


	return S_OK;
}
HRESULT CLoader::Add_To_UI_Mid_DisPlay_Transform()
{
	// 오른쪽 판넬 트랜스폼
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Transform_Mid_Panel"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}
HRESULT CLoader::Add_To_UI_Mid_DisPlay_Prototype()
{
	// 중앙 판넬 원본 객체 생성
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Mid_Panel"),
		CUI_ExpDisplay::Create(m_pGraphic_Device))))
		return E_FAIL;


	return S_OK;
}
HRESULT CLoader::Add_To_UI_Right_DisPlay_Textures()
{

	// 오른쪽 판넬 텍스쳐
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Right_Panel"),
		CTexture::Create(m_pGraphic_Device,
			TEXT("../../Resources/Textures/UI/Right/SR_HUD_bottom_right.png"),
			1))))
		return E_FAIL;

	// 오른쪽 판넬의 총알 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Bullet_Bar"),
		CTexture::Create(m_pGraphic_Device,
			TEXT("../../Resources/Textures/UI/Right/SR_HUD_bar_ammo.png"),
			1))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Add_To_UI_Right_DisPlay_Buffer()
{
	// 오른쪽 판넬 버퍼
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_VIBuffer_RIght_Panel"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 오른쪽 판넬의 총알 UI 버퍼
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_VIBuffer_Bullet_Bar"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}
HRESULT CLoader::Add_To_UI_Right_DisPlay_Transform()
{
	// 오른쪽 판넬 트랜스폼
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Transform_Right_Panel"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 오른쪽 판넬의 총알 UI 트랜스폼 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Transform_Bullet_Bar"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}
HRESULT CLoader::Add_To_UI_Right_DisPlay_Prototype()
{
	
	// 중앙 판넬 원본 객체 생성
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Right_Bar"),
		CUI_Right_Panel::Create(m_pGraphic_Device))))
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
		CTexture::Create(m_pGraphic_Device,
			TEXT("../../Resources/Textures/UI/Game_Menu.png"),
			1))))
		return E_FAIL;

	return S_OK;
}
HRESULT CLoader::Add_To_UI_Menu_DisPlay_Buffer()
{
	// 기본 메뉴 판넬 버퍼
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_VIBuffer_Menu_Panel"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}
HRESULT CLoader::Add_To_UI_Menu_DisPlay_Transform()
{
	// 기본 메뉴 판넬 트랜스폼
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Transform_Menu_Panel"),
		CTransform::Create(m_pGraphic_Device))))
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
HRESULT CLoader::Add_To_Shop_Textures()
{
	// 포인트 스탯 상점
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Point_Shop_UI"),
		CTexture::Create(m_pGraphic_Device,
			TEXT("../../Resources/Textures/UI/Point_Shop/Point_Shop_UI.png"),
			1))))
		return E_FAIL;

	// 에피소드 선택창
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Episode_UI"),
		CTexture::Create(m_pGraphic_Device,
			TEXT("../../Resources/Textures/UI/Episode_UI/level_selection.png"),
			1))))
		return E_FAIL;

	// 스펠 샵 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Spell_Shop_UI"),
		CTexture::Create(m_pGraphic_Device,
			TEXT("../../Resources/Textures/UI/Spell_UI/Spell_UI.png"),
			1))))
		return E_FAIL;

	// 업그레이드 웨폰 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Texture_Upgrade_Weapon_UI"),
		CTexture::Create(m_pGraphic_Device,
			TEXT("../../Resources/Textures/UI/Upgrade_Weapon_UI/lweaponshop.png"),
			1))))
		return E_FAIL;

	return S_OK;
}
HRESULT CLoader::Add_To_Shop_Buffer()
{
	// 포인트 스탯 상점
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_VIBuffer_Point_Shop_UI"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;


	// 에피소드 선택창
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_VIBuffer_Episode_UI"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 스펠 샵 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_VIBuffer_Spell_Shop_UI"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;
	// 업그레이드 웨폰 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_VIBuffer_Upgrade_Weapon_UI"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}
HRESULT CLoader::Add_To_Shop_Transform()
{
	// 포인트 스탯 상점
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Transform_Point_Shop_UI"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 에피소드 선택창
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Transform_Episode_UI"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;
	// 스펠 샵 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Transform_Spell_Shop_UI"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 업그레이드 웨폰 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_Component_Transform_Upgrade_Weapon_UI"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}
HRESULT CLoader::Add_To_Shop_Prototype()
{
	// 포인트 스탯 상점
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Point_Shop_UI"),
		CUI_Point_Shop::Create(m_pGraphic_Device))))
		return E_FAIL;


	// 에피소드 선택창
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Episode_UI"),
		CUI_Episode::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 스펠 샵 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Spell_Shop_UI"),
		CUI_Spell_Shop::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 업그레이드 웨폰 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Upgrade_Weapon_UI"),
		CUI_Upgrade_Weapon::Create(m_pGraphic_Device))))
		return E_FAIL;
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
