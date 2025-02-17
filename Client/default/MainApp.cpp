#include "MainApp.h"
#include "Graphic_Device.h"



CMainApp::CMainApp()
	: m_pGraphic_Device { CGraphic_Device::Get_Instance() }
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CMainApp::Initialize()
{
	if (FAILED(m_pGraphic_Device->Initialize(g_hWnd, true, g_iWinSizeX, g_iWinSizeY)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Update()
{
}

HRESULT CMainApp::Render()
{
	m_pGraphic_Device->Render_Begin();

	m_pGraphic_Device->Render_End();

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

	/* 내멤버를 정리한다.*/
	Safe_Release(m_pGraphic_Device);


}
