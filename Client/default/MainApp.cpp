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
#include "GameObject_Cube.h"
#include "GameObject_Plane.h"
#include "GameObject_Light.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{

	// 3월 18일 랜드오브젝트와 평면의 방정식
	// 플레이어 지형타기까지 완료 
	// 스카이큐브 까지 완료
	// dds 감지기능 완료

	ENGINE_DESC		Desc{};
	Desc.hWnd = g_hWnd;
	Desc.isWindowed = true;
	Desc.iWinSizeX = g_iWinSizeX; 
	Desc.iWinSizeY = g_iWinSizeY;
	Desc.iNumLevels = LEVEL_END;

	if (FAILED(m_pGameInstance->Initialize_Engine(Desc, &m_pGraphic_Device)))
		return E_FAIL;

	D3DLIGHT9			LightDesc{};

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Direction = _float3(1.f, -1.f, 1.f);
	//LightDesc.Position = _float3(10.f, 3.f, 10.f);
	//LightDesc.Range = 10.f;
	//LightDesc.Attenuation1 = 1.3f;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);	
	m_pGraphic_Device->SetLight(0, &LightDesc);

	//LightDesc.Type = D3DLIGHT_POINT;
	///*LightDesc.Direction = _float3(1.f, -1.f, 1.f);*/
	//LightDesc.Position = _float3(20.f, 3.f, 10.f);
	//LightDesc.Range = 10.f;
	//LightDesc.Attenuation1 = 1.3f;
	//LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//LightDesc.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//m_pGraphic_Device->SetLight(1, &LightDesc);

	

	D3DMATERIAL9		MtrlDesc{};
	
	MtrlDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	MtrlDesc.Ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f);

	m_pGraphic_Device->SetMaterial(&MtrlDesc);
	m_pGraphic_Device->LightEnable(0, true);
	/*m_pGraphic_Device->LightEnable(1, true);*/

	if (FAILED(Ready_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;



	/* 최초 보여줄 레벨을 할당하자. */
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

  

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
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

	/* For.Prototype_Component_Texture_Base*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_Component_Texture_Base"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/Base0.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Bullet"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,  TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider_Sphere::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		CCollider_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Material */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		CMaterial::Create(m_pGraphic_Device, L"../../Resources/Materials/TestMaterial.json"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(L"MainFont", L"../../Resources/Textures/Font/StandardFont.ttf")))
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
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Plane"),
		CGameObject_Plane::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Cube"),
		CGameObject_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,
		TEXT("Prototype_GameObject_Light"),
		CGameObject_Light::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
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
	Safe_Release(m_pGraphic_Device);

	m_pGameInstance->Release_Engine();

	/* 내멤버를 정리한다.*/	
	Safe_Release(m_pGameInstance);
	
	

	


}
