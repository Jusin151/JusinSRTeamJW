#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Logo.h"
#include "Level_Loading.h"
#include "Loading_UI.h"
#include "VIBuffer_Cube.h"
#include "Structure.h"
#include "Collider_Sphere.h"
#include "Collider_Cube.h"
#include "Material.h"
#include "GameObjects_Base.h"
#include "VIBuffer_TexturedCube.h"
#include "Light.h"
#include "PickingSys.h"
#include "Particles.h"
#include "Sky.h"
#include "MiniMap.h"
#include "StructureManager.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "Camera_FirstPerson.h"
#include "Player.h"
#include "UI_Headers.h" // UI 헤더들 
#include "Hub_Headers.h" // 허브 헤더들
#include <Weapon_Base.h>
#include "GamePlay_Button.h"
#include "JsonLoader.h"
#include "Weapon_Effect.h"
#include "Staff_Bullet.h"
#include "Image.h"
#include "Inven_UI.h"
#include "Level_Hub.h"


CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
#pragma region 랜덤생성
	srand((_uint)time(NULL));
#pragma endregion

#pragma region Engine 준비
	ENGINE_DESC		Desc{};
	Desc.hWnd = g_hWnd;
	Desc.isWindowed = true;
	Desc.iWinSizeX = g_iWinSizeX;
	Desc.iWinSizeY = g_iWinSizeY;
	Desc.iNumLevels = LEVEL_END;

	if (FAILED(m_pGameInstance->Initialize_Engine(Desc, &m_pGraphic_Device)))
		return E_FAIL;	
#pragma endregion

#pragma region 준비
	if (FAILED(Ready_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Player()))
		return E_FAIL;
	if (FAILED(Ready_Prototype_UI()))
		return E_FAIL;
	if (FAILED(Ready_Prototype_Weapon()))
		return E_FAIL;
	if (FAILED(Ready_Prototype_Inven()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, // 카메라 원형객체
		TEXT("Prototype_GameObject_Camera_FirstPerson"),
		CCamera_FirstPerson::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion

	



	/* 최초 보여줄 레벨을 할당하자. */
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

  

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		PostQuitMessage(0);
//	m_pMyImGui->Update(fTimeDelta);
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	
	//m_pMyImGui->Render();

	m_pGameInstance->Draw();

	return S_OK;
}

HRESULT CMainApp::Ready_Default_Setting()
{
	//m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	return S_OK;
}
HRESULT CMainApp::Ready_Component_For_Static()
{
#pragma region VIBuffer
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_HP"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_MP"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_EXP"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_TexturedCube"),
		CVIBuffer_TexturedCube::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Bullet"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;
	/* For.Prototype_Component_VIBuffer_Cube*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;
	/* For.Prototype_Component_VIBuffer_Sphere*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Sphere"),
		CVIBuffer_Sphere::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion
	
#pragma region Texture
	/* For.Prototype_Component_Texture_Base*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Base"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/Base0.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/Terrain/Tile0.jpg"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Light*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Light"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/Light.png"), 1))))
		return E_FAIL;



	/* For.Prototype_Component_Texture_Snow*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/Particle/snowflake.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Flare*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Flare"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/Particle/flare_alpha.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Cube_Base*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Cube_Base"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBE,
			TEXT("../../Resources/Textures/XYZ.dds"), 1))))
		return E_FAIL;
#pragma endregion
	
#pragma region Light
	/* For.Prototype_Component_Light_Point*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Light_Point"),
		CLight::Create(m_pGraphic_Device, CLight::LT_POINT))))
		return E_FAIL;
	/* For.Prototype_Component_Light_Direction*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Light_Direction"),
		CLight::Create(m_pGraphic_Device, CLight::LT_DIR))))
		return E_FAIL;
	/* For.Prototype_Component_Light_Spot*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Light_Spot"),
		CLight::Create(m_pGraphic_Device, CLight::LT_SPOT))))
		return E_FAIL;
#pragma endregion

#pragma region Shader
	/*For.Prototype_Component_Shader*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader"),
		CShader::Create(m_pGraphic_Device, L"../../Resources/Shaders/Shader_Rect.hlsl"))))
		return E_FAIL;
#pragma endregion
	
#pragma region Particle
	/* For.Prototype_Component_Particle_Snow*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Snow"),
		CSnow_Particle_System::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Particle_Firework*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Firework"),
		CFirework_Particle_System::Create(m_pGraphic_Device))))
		return E_FAIL;
	/* For.Prototype_Component_Particle_Projectile*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Projectile"),
		CProjectile_Particle_System::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Particle_Gold*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Gold"),
		CGold_Particle_System::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Particle_Blood*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Blood"),
		CBlood_Particle_System::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Particle_BulletShell*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_BulletShell"),
		CBulletShell_Particle_System::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion
	
#pragma region Transform
	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion

#pragma region Collider
	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider_Sphere::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		CCollider_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion

#pragma region Material
	/* For.Prototype_Component_Material */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		CMaterial::Create(m_pGraphic_Device, L"../../Resources/Materials/BaseMaterial.json"))))
		return E_FAIL;
#pragma endregion

#pragma region Font
	if (FAILED(m_pGameInstance->Add_Font(L"MainFont", L"../../Resources/Textures/Font/StandardFont.ttf")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(L"EventFont", L"../../Resources/Textures/Font/함초롱돋움R.ttf")))
		return E_FAIL;
#pragma endregion
	

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, // 웨폰이펙트 테스트 삭제 X
		TEXT("Prototype_GameObject_MiniMap"),
		CMiniMap::Create(m_pGraphic_Device))))
		return E_FAIL;
	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
#pragma region BaseGameObjects
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Plane"),
		CGameObject_Plane::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Cube"),
		CGameObject_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Sphere"),
		CGameObject_Sphere::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Light"),
		CGameObject_Light::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Particle_System_Test"),
		CGameObject_Particle_Test::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Particle_System_Firework"),
		CGameObject_Particle_Firework::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Projectile_Test"),
		CGameObject_Projectile_Test::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pGraphic_Device))))
		return E_FAIL;


	

#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_UI()
{

	// 게임플레이 레벨 전체 UI의 캔버스 판넬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Default_PlayerUI"),
		CUI_Default_Panel::Create(m_pGraphic_Device))))
		return E_FAIL;


	// 게임플레이 레벨 전체 UI의 캔버스 판넬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Default_PlayerUI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/UI/Default_UI.png"),
			1))))
		return E_FAIL;

	// 레프트 디스플레이 UI 클래스 생성 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Left_Panel"),
		CUI_Left_Display::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 레프트 디스플레이 UI 텍스쳐 생성							
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Left_Panel"),
		CTexture::Create(m_pGraphic_Device,
			CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Left/SR_HUD_bottom_left.png"), 1))))
		return E_FAIL;

	// 총알바 UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Bullet_Bar"),
		CUI_Bullet_Bar::Create(m_pGraphic_Device))))
		return E_FAIL;


	// 총알바 UI 텍스쳐						
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Bullet_Bar"),
		CTexture::Create(m_pGraphic_Device,
			CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Right/SR_HUD_bar_ammo.png"), 1))))
		return E_FAIL;


	// ExpBar UI 이미지 생성 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_ExpBar_UI"),
		CTexture::Create(m_pGraphic_Device,
			CTexture::TYPE_2D, TEXT("../../Resources/Textures/UI/Middle/SR_EXP_BlackBar0.png"), 1))))
		return E_FAIL;

	// ExpBar UI 클래스 생성 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_ExpBar_UI"),
		CUI_Exp_Bar::Create(m_pGraphic_Device))))
		return E_FAIL;

	// ExpBar UI 클래스 생성 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Event_UI"),
		CUI_Event::Create(m_pGraphic_Device))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Weapon()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, // 미니건 테스트 삭제 X
		TEXT("Prototype_GameObject_Minigun"),
		CMinigun::Create(m_pGraphic_Device))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, // 하베스터 테스트 삭제 X
		TEXT("Prototype_GameObject_Harvester"),
		CHarvester::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, // 소닉 테스트 삭제 X
		TEXT("Prototype_GameObject_Sonic"),
		CSonic::Create(m_pGraphic_Device))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, // 스태프총알 테스트 삭제 X
		TEXT("Prototype_GameObject_Staff_Bullet"),
		CStaff_Bullet::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, // 웨폰이펙트 테스트 삭제 X
		TEXT("Prototype_GameObject_Weapon_Effect"),
		CWeapon_Effect::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 이미지 클래스 (웨폰 아이콘 뽑아내려고)
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Image"),
		CImage::Create(m_pGraphic_Device))))
		return E_FAIL;

	////// 웨폰들의  아이콘 사진
 	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Weapon_Icon"),
		CTexture::Create(m_pGraphic_Device,
			CTexture::TYPE_2D, TEXT("../../Resources/Textures/Weapon/Icon/Weapon_Icon_%d.png"), 8))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Inven()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, // 인벤 테스트 삭제 X
		TEXT("Prototype_GameObject_Inven"),
		CInventory::Create(m_pGraphic_Device))))
		return E_FAIL;


	// 인벤토리UI 클래스 생성 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Inven_UI"),
		CInven_UI::Create(m_pGraphic_Device))))
		return E_FAIL;

	//// 인벤토리UI 예시 사진
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, 
		TEXT("Prototype_Component_Texture_Inven_UI"),
		CTexture::Create(m_pGraphic_Device,
			CTexture::TYPE_2D, TEXT("../../Resources/Textures/Inven/Inven.png"), 1))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Player()
{
	// 플레이어	클래스 생성
   	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();
	CStructureManager::Destroy_Instance();
	Safe_Release(m_pGraphic_Device);
	m_pGameInstance->Stop_All_Event();
	m_pGameInstance->Release_Engine();


	/* 내멤버를 정리한다.*/	
	Safe_Release(m_pGameInstance);
	
}
