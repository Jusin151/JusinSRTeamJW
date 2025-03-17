#include "MainApp.h"
#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_Loading.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	// 3월 17일 끊으신곳 : 머테리얼의 스펙큘러란? 에서 끊으심
	// 3월 18일 끊으신곳 : 반사벡터와 Pow로 코사인 그래프를 압축시키는이유 
	// Emissive 아이언맨 가슴까지 설명해주심

	/*결과색상 = (L.Diffuse.r * M.Diffuse.r,
		L.Diffuse.g * M.Diffuse.g,
		L.Diffuse.b * M.Diffuse.b,
		L.Diffuse.a * M.Diffuse.a);

	결과색상 = (L.Diffuse.r * DiffuseTexture.r * M.Diffuse.r,
		L.Diffuse.g * DiffuseTexture.g * M.Diffuse.g,
		L.Diffuse.b * DiffuseTexture.b * M.Diffuse.b,
		L.Diffuse.a * DiffuseTexture.a * M.Diffuse.a);

	결과색상 * (명암(0.f ~ 1.f) + L.Ambient * M.Ambient)*/

	//LightDesc
	
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

	D3DLIGHT9			LightDesc{};

	/* 난반사에 위한 빛의 색상. */
	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Direction = _float3(1.f, -1.f, 1.f);
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Specular = ;

	/* LightDesc.Specular = ;*/
	m_pGraphic_Device->SetLight(0, &LightDesc);

	/* 텍스쳐로 재질을 대체한다. */

	D3DMATERIAL9		MtrlDesc{};
	/* 난반사에 위한 픽셀의 색상. */
	MtrlDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	MtrlDesc.Ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f);
	MtrlDesc.Specular = ;

	m_pGraphic_Device->SetMaterial(&MtrlDesc);

	m_pGraphic_Device->LightEnable(0, true);



	if (FAILED(Ready_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Default_Setting()))
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
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

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

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, eLevelID))))
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

	Safe_Release(m_pGraphic_Device);

	m_pGameInstance->Release_Engine();

	/* 내멤버를 정리한다.*/	
	Safe_Release(m_pGameInstance);
	


	


}
