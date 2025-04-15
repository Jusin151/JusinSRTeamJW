#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Logo.h"
#include "Level_Loading.h"
#include "Loading_UI.h"
#include "Structure.h"
#include "GameObjects_Base.h"
#include "PickingSys.h"
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
#include "Cursor.h"
#include "GameObject_Snow.h"
#include "Sound_Manager.h"
#include "UI_Hit_Blood.h"
#include "UI_Restore_Effect.h"
#include "Camera_CutScene.h"


CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::Get_Instance() },
	m_pPickingSys{ CPickingSys::Get_Instance() }
{ 
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pPickingSys);
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

	if (FAILED(m_pGameInstance->Initialize_Engine(Desc, &m_pGraphic_Device, &m_pSound_Manager)))
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

	if (FAILED(Ready_Prototype_Weapon()))
		return E_FAIL;
	if (FAILED(Ready_Prototype_Inven()))
		return E_FAIL;
#pragma endregion
	m_pPickingSys->Initialize(g_hWnd, m_pGraphic_Device, m_pGameInstance);
	CJsonLoader jsonLoader;
	if (FAILED(jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_Static.json")))
		return E_FAIL;
	if (FAILED(Ready_Prototype_UI()))
		return E_FAIL;
	/* 최초 보여줄 레벨을 할당하자. */
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	//컷씬 카메라 등록
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_CutScene"),
		CCamera_CutScene::Create(m_pGraphic_Device))))
		return E_FAIL;

	ShowCursor(false);
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

	/* For.Prototype_Component_Texture_Skybox_Tundra*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Skybox_Tundra"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBE,
			TEXT("../../Resources/Textures/SkyBox/SkyBox_Tundra.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Skybox_Hell*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Skybox_Hell"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBE,
			TEXT("../../Resources/Textures/SkyBox/SkyBox_Hell.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Skybox_Cthulhu*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Skybox_Cthulhu"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBE,
			TEXT("../../Resources/Textures/SkyBox/SkyBox_Cthulhu.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_Hit*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Player_Hit"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/Effect/bloodscreen_2.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_Restore*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Player_Restore"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/Effect/Restore_Screen.png"), 1))))
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
	/*For.Prototype_Component_BaseShader*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
		CShader::Create(m_pGraphic_Device, L"../../Resources/Shaders/BaseShader.hlsl"))))
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
	/* For.Prototype_Component_Particle_Trail*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Trail"),
		CTrail_Particle_System::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Particle_Gold*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Gold"),
		CGold_Particle_System::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Particle_Magic*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Magic"),
		CMagic_Particle_System::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Particle_Blood*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Blood"),
		CBlood_Particle_System::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Particle_Gib*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Gib"),
		CGib_Particle_System::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Particle_BulletShell*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_BulletShell"),
		CBulletShell_Particle_System::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Particle_Projectile*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Projectile"),
		CProjectile_Particle_System::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Particle_Fire*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Fire"),
		CFire_Particle_System::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Particle_Smoke*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Smoke"),
		CSmoke_Particle_System::Create(m_pGraphic_Device))))
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

#pragma region Sound
	/* For.Prototype_Component_Sound_Source */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sound_Source"),
		CSound_Source::Create(m_pGraphic_Device, m_pSound_Manager))))
		return E_FAIL;
	/* For.Prototype_Component_Sound_Source */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sound_Listener"),
		CSound_Listener::Create(m_pGraphic_Device, m_pSound_Manager))))
		return E_FAIL;
#pragma endregion

#pragma region Font
	if (FAILED(m_pGameInstance->Add_Font(L"MainFont", L"../../Resources/Textures/Font/StandardFont.ttf")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(L"EventFont", L"../../Resources/Textures/Font/함초롱돋움R.ttf")))
		return E_FAIL;
#pragma endregion
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
#pragma region Camera
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, // 카메라 원형객체
		TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, // 카메라 원형객체
		TEXT("Prototype_GameObject_Camera_FirstPerson"),
		CCamera_FirstPerson::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion

#pragma region MiniMap
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_MiniMap"),
		CMiniMap::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion

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
		TEXT("Prototype_GameObject_Particle_System_Snow"),
		CGameObject_Snow::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Projectile_Test"),
		CGameObject_Projectile_Test::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Hit_Blood"),
		CUI_Hit_Blood::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Restore_Effect"),
		CUI_Restore_Effect::Create(m_pGraphic_Device))))
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
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_UI_Cursor"),
		CCursor::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC,
		TEXT("Prototype_GameObject_UI_Cursor"),LEVEL_STATIC,TEXT("Layer_Cursor"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Weapon()
{


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
	Safe_Release(m_pPickingSys);
	CPickingSys::Destroy_Instance();
	CStructureManager::Destroy_Instance();
	Safe_Release(m_pGraphic_Device);
	m_pGameInstance->Stop_All_Event();
	m_pGameInstance->Release_Engine();


	Safe_Release(m_pSound_Manager);
	/* 내멤버를 정리한다.*/	
	Safe_Release(m_pGameInstance);
	
}
