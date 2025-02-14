#include "MainApp.h"

CMainApp::CMainApp()
{
}

HRESULT CMainApp::Initialize()
{
	return S_OK;
}

void CMainApp::Update()
{
}

HRESULT CMainApp::Render()
{
	return E_NOTIMPL;
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


}
