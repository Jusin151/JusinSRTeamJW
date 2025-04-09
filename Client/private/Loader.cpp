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
#include "HellBoss_Headers.h" // 헬보스 관련 생성에 필요한 헤더 



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

	if (FAILED(D3DXCreateFontW(
		m_pGraphic_Device,
		24, 0, FW_BOLD, 1, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		L"맑은 고딕", &m_pFont)))
		return E_FAIL;

	m_iTotalTaskCount = 0;
	// 레벨별 전체 작업 수 설정
	switch (m_eNextLevelID) {
	case LEVEL_LOGO:      m_iTotalTaskCount = 5;  break; // 텍스쳐, 모델, 셰이더, 사운드, 원형객체, 최종
	case LEVEL_EDITOR:    m_iTotalTaskCount = 4;  break; // JSON1, JSON2, JSON3, 최종
	case LEVEL_HONG:      m_iTotalTaskCount = 6;  break; // 6 등록+텍스쳐
	}
	m_iCompletedTaskCount = 0;
	m_fProgress = 0.f;

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
	case LEVEL_HONG:
		hr = Loading_For_Hong();
		break;
	case LEVEL_BOSS:
 		hr = Loading_For_Boss();
		break;
	}
 	m_pGameInstance->Set_LevelState(CGameInstance::LEVEL_STATE::NORMAL); 
	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	
	return S_OK;
}
void CLoader::Output_LoadingText()
{	// 윈도우 타이틀에도 진행률 표시
	SetWindowText(g_hWnd, m_szLoadingText);

	RECT barRect = { 80, 640, 1200, 690 };
	DrawLoadingBar(m_pGraphic_Device, m_fProgress, barRect);

	RECT textRect = { 80, 600, 1200, 630 };
	m_pFont->DrawTextW(
		nullptr,
		m_szLoadingText,
		-1,
		&textRect,
		DT_LEFT | DT_NOCLIP,
		D3DCOLOR_ARGB(255, 255, 255, 255)
	);
}
HRESULT CLoader::Loading_For_Logo()
{
 	lstrcpy(m_szLoadingText, TEXT("텍스쳐을(를) 로딩중입니다."));

	Add_To_Logo_Textures();
	CompleteOneTask(); // 1/6
	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	CompleteOneTask(); // 1/6
	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));
	CompleteOneTask(); // 1/6
	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));
	CompleteOneTask(); // 1/6
 	m_pGameInstance->Load_Bank(L"Background");

 	lstrcpy(m_szLoadingText, TEXT("원형객체을(를) 로딩중입니다."));

	Add_To_Logo_Prototype();
	CompleteOneTask(); // 1/6
	m_isFinished = true;
 	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_pGameInstance->Set_LevelState(CGameInstance::LEVEL_STATE::NORMAL);
	return S_OK;
}
HRESULT CLoader::Loading_For_GamePlay()
{
	
   	lstrcpy(m_szLoadingText, TEXT("JSON에서 프로토타입을 로딩중입니다."));
	CJsonLoader jsonLoader;


	m_iTotalTaskCount += jsonLoader.CountPrototypes(L"../Save/Prototypes.json");
	m_iTotalTaskCount += jsonLoader.CountPrototypes(L"../Save/Prototypes_For_Test.json");

 //	if (FAILED(jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device,L"../Save/Prototypes.json")))
	//return E_FAIL;

	//// JSON 로더를 사용하여 모든 프로토타입 로드
	//if (FAILED(jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Test.json")))
	//	return E_FAIL;

	jsonLoader.Load_Prototypes(
		m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Test.json",
		[this]() { CompleteOneTask(); }
	);

	jsonLoader.Load_Prototypes(
		m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes.json",
		[this]() { CompleteOneTask(); }
	);


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Hub()
{	// JSON 로더를 사용하여 모든 프로토타입 로드
	CJsonLoader jsonLoader;


	m_iTotalTaskCount += jsonLoader.CountPrototypes(L"../Save/Prototypes_For_Hub.json");
	m_iTotalTaskCount += jsonLoader.CountPrototypes(L"../Save/Prototypes_For_Test.json");


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


	//if (FAILED(jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Hub.json"))) // 건물관련
	//	return E_FAIL;
	//// JSON 로더를 사용하여 모든 프로토타입 로드
	//if (FAILED(jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Test.json"))) // 명훈이형꺼 관련
	//	return E_FAIL;

	jsonLoader.Load_Prototypes(
		m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Hub.json",
		[this]() { CompleteOneTask(); }
	);
	jsonLoader.Load_Prototypes(
		m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Test.json",
		[this]() { CompleteOneTask(); }
	);

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	m_pGameInstance->Set_LevelState(CGameInstance::LEVEL_STATE::NORMAL);

	return S_OK;
}

HRESULT CLoader::Loading_For_Hong()
{
	CJsonLoader jsonLoader;


	m_iTotalTaskCount += jsonLoader.CountPrototypes(L"../Save/Prototypes_For_Hong.json");

	//헬보스 객체 등록
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss"),
		CHellBoss::Create(m_pGraphic_Device))))
		return E_FAIL;
	CompleteOneTask();

	//헬보스 텍스쳐
 	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HONG, TEXT("Prototype_Component_Texture_HellBoss"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Boss/HellBoss/HellBoss_%d.png"), 337))))
		return E_FAIL;
	CompleteOneTask();

	//헬보스 스킬 다크홀
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss_Skill_DarkHole"),
		CHellBoss_Skill_DarkHole::Create(m_pGraphic_Device))))
		return E_FAIL;
	CompleteOneTask();

	//헬보스 스킬 다크홀 텍스쳐
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HONG, TEXT("Prototype_Component_Texture_HellBoss_Skill_DarkHole"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Boss/HellBoss/Skill/Warp/Warp%d.png"), 16))))
		return E_FAIL;
	CompleteOneTask();

	//헬보스 스킬 다크홀의 총알
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss_DarkBullet"),
		CHellBoss_DarkBullet::Create(m_pGraphic_Device))))
		return E_FAIL;
	CompleteOneTask();


	//헬보스 스킬 랜딩
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss_Skill_Landing"),
		CHellBoss_Skill_Landing::Create(m_pGraphic_Device))))
		return E_FAIL;
	CompleteOneTask();

	//헬보스 스킬 랜딩 텍스쳐
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HONG, TEXT("Prototype_Component_Texture_HellBoss_Skill_Landing"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Boss/HellBoss/Skill/Landing/Landing%d.png"),19))))
		return E_FAIL;
	CompleteOneTask();


	//헬보스 총알객체 생성!!
 	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss_Bullet"),
		CHellBoss_Bullet::Create(m_pGraphic_Device))))
		return E_FAIL;
	CompleteOneTask();

	//헬보스 총알 텍스쳐 (눈)
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HONG, TEXT("Prototype_Component_Texture_HellBoss_Bullet"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Weapon/Staff/Bullet/wand_projectile_%d.png"),7))))
		return E_FAIL;
	CompleteOneTask();

	//헬보스 총알 텍스쳐 (손)
 	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HONG, TEXT("Prototype_Component_Texture_HellBoss_Hand_Bullet"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Boss/HellBoss/Bullet/slow_orb000%d.png"),10))))
		return E_FAIL;
	CompleteOneTask();

	//헬보스 총알 텍스쳐 (파워블라스트)
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HONG, TEXT("Prototype_Component_Texture_HellBoss_Blast"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Boss/HellBoss/Bullet/Flame/DemonFlame%d.png"), 28))))
		return E_FAIL;
	CompleteOneTask();

	//헬보스 두번째 페이즈 총알 텍스쳐 (손)
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HONG, TEXT("Prototype_Component_Texture_HellBoss_Phase2_Hand_Bullet"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Boss/HellBoss/Bullet/2Phase_Hand/Bullet%d.png"),6))))
		return E_FAIL;
	CompleteOneTask();


	////패링용 왼손 생성
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss_B"),
	//	CHellBoss_Bullet::Create(m_pGraphic_Device))))
	//	return E_FAIL;
	//CompleteOneTask();

	////패링용 왼손 텍스쳐
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_HONG, TEXT("Prototype_Component_Texture_HellBoss_Bullet"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, TEXT("../../Resources/Textures/Weapon/Staff/Bullet/wand_projectile_%d.png"), 7))))
	//	return E_FAIL;
	//CompleteOneTask();


	jsonLoader.Load_Prototypes(
		m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Hong.json",
		[this]() { CompleteOneTask(); }
	);

 	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_For_Boss()
{
	CJsonLoader jsonLoader;


	m_iTotalTaskCount += jsonLoader.CountPrototypes(L"../Save/Prototypes_For_Boss1.json");

	lstrcpy(m_szLoadingText, TEXT("JSON에서 프로토타입을 로딩중입니다."));
	jsonLoader.Load_Prototypes(
		m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Boss1.json",
		[this]() { CompleteOneTask(); }
	);

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

void CLoader::DrawLoadingBar(IDirect3DDevice9* device, _float progress, const RECT& rc)
{
	device->SetTexture(0, nullptr);  // 0번 텍스처 해제
	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	_float fullW = _float(rc.right - rc.left);
	_float fillW = fullW * progress;
	const DWORD kRed = 0xFFFF0000; 

	CUSTOMVERTEX v[6] = {
		{ float(rc.left),        float(rc.top),    0.0f, 1.0f, kRed },
		{ float(rc.left + fillW),  float(rc.top),    0.0f, 1.0f, kRed },
		{ float(rc.left),        float(rc.bottom), 0.0f, 1.0f, kRed },
		{ float(rc.left + fillW),  float(rc.top),    0.0f, 1.0f, kRed },
		{ float(rc.left + fillW),  float(rc.bottom), 0.0f, 1.0f, kRed },
		{ float(rc.left),        float(rc.bottom), 0.0f, 1.0f, kRed },
	};


	device->SetFVF(D3DFVF_CUSTOMVERTEX);
	device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, v, sizeof(CUSTOMVERTEX));

	// 테두리
	ID3DXLine* pLine = nullptr;
	if (SUCCEEDED(D3DXCreateLine(device, &pLine))) {
		D3DXVECTOR2 pts[5] = {
			{ float(rc.left),  float(rc.top) },
			{ float(rc.right), float(rc.top) },
			{ float(rc.right), float(rc.bottom) },
			{ float(rc.left),  float(rc.bottom) },
			{ float(rc.left),  float(rc.top) }
		};
		pLine->Begin();
		pLine->Draw(pts, 5, 0xFFFFFFFF);
		pLine->End();
		pLine->Release();
	}
	
	device->SetRenderState(D3DRS_ZENABLE, TRUE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
}

HRESULT CLoader::Loading_For_Editor()
{
	lstrcpy(m_szLoadingText, TEXT("JSON에서 프로토타입을 로딩중입니다."));
	CJsonLoader jsonLoader;
	// JSON 로더를 사용하여 모든 프로토타입 로드
	if (FAILED(jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Editor.json")))
		return E_FAIL;


	// JSON 로더를 사용하여 모든 프로토타입 로드
	if (FAILED(jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Test.json")))
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

void CLoader::CompleteOneTask()
{
	m_iCompletedTaskCount++;

	m_fProgress = float(m_iCompletedTaskCount) / m_iTotalTaskCount;


	int percent = static_cast<int>(ceil(m_fProgress * 100.0f));
	if (percent > 100) percent = 100;

	swprintf_s(m_szLoadingText, L"로딩 중... %d%%", percent);
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
	Safe_Release(m_pFont);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pGraphic_Device);


	DeleteCriticalSection(&m_CriticalSection);
}


