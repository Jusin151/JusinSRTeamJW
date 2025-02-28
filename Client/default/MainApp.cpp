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
	ENGINE_DESC		Desc{};
	Desc.hWnd = g_hWnd;
	Desc.isWindowed = true;
	Desc.iWinSizeX = g_iWinSizeX;
	Desc.iWinSizeY = g_iWinSizeY;
	Desc.iNumLevels = LEVEL_END;

	if (FAILED(m_pGameInstance->Initialize_Engine(Desc, &m_pGraphic_Device)))
		return E_FAIL;

	/* 최초 보여줄 레벨을 할당하자. */
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;
	
	return S_OK;
}

void CMainApp::Update()
{
	m_pGameInstance->Update_Engine(0.f);
}

HRESULT CMainApp::Render()
{

	m_pGameInstance->Draw();

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
	/* 내멤버를 정리한다.*/	
	Safe_Release(m_pGameInstance);
	m_pGameInstance->Release_Engine();
}
