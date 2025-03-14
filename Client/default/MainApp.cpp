#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Logo.h"
#include "Level_Loading.h"
#include "Loading_UI.h"
#include "VIBuffer_Cube.h"
#include "Collider_Sphere.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	D3DLIGHT9			LightDesc{};

	LightDesc
	
	/*
	D3DLIGHT9			LightDesc{};
	m_pGraphic_Device->SetLight(0, &LightDesc);

	D3DMATERIAL9		MtrlDesc{};
	m_pGraphic_Device->SetMaterial(&MtrlDesc);

	m_pGraphic_Device->LightEnable(0, true);
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);*/


	ENGINE_DESC		Desc{};
	Desc.hWnd = g_hWnd;
	Desc.isWindowed = true;
	Desc.iWinSizeX = g_iWinSizeX; 
	Desc.iWinSizeY = g_iWinSizeY;
	Desc.iNumLevels = LEVEL_END;

	if (FAILED(m_pGameInstance->Initialize_Engine(Desc, &m_pGraphic_Device)))
		return E_FAIL;

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
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Draw();

	return S_OK;
}

HRESULT CMainApp::Ready_Default_Setting()
{
	/*m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);*/

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, false);

	return S_OK;
}
HRESULT CMainApp::Ready_Component_For_Static()
{
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC,  TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider_Sphere::Create(m_pGraphic_Device))))
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
