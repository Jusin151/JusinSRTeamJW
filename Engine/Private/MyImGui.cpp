#include "MyImGui.h"
#include "GameInstance.h"

CMyImGui::CMyImGui(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device{pGraphic_Device}
	, m_pGameInstance{CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CMyImGui::Initialize(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	return S_OK;
}

void CMyImGui::Update(_float fTimeDelta)
{
}

HRESULT CMyImGui::Render()
{
	return S_OK;
}

CMyImGui* CMyImGui::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	return nullptr;
}

void CMyImGui::Free()
{
	CBase::Free();
}
