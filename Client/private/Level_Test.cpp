#include "Level_Test.h"
#include "GameInstance.h"
#include "PickingSys.h"
#include "MyImGui.h"
#include "JsonLoader.h"

CLevel_Test::CLevel_Test(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CLevel{pGraphic_Device},
    m_pPickingSys{ CPickingSys::Get_Instance() }
{
    m_pPickingSys->Initialize(g_hWnd, m_pGraphic_Device, m_pGameInstance);
    Safe_AddRef(m_pPickingSys);
}

HRESULT CLevel_Test::Initialize()
{
    CJsonLoader jsonLoader;
    jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL_TEST.json", LEVEL_TEST);
    return E_NOTIMPL;
}

void CLevel_Test::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Test::Render()
{
    return E_NOTIMPL;
}

HRESULT CLevel_Test::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CLevel_Test::Ready_Layer_Particle(const _wstring& strLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CLevel_Test::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    return E_NOTIMPL;
}

CLevel_Test* CLevel_Test::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    return nullptr;
}

void CLevel_Test::Free()
{
}
