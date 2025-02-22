#include "Level.h"

CLevel::CLevel(LPDIRECT3DDEVICE9 pGraphic_Device)
    : m_pGraphic_Device { pGraphic_Device }
{
    Safe_AddRef(m_pGraphic_Device);
}

HRESULT CLevel::Initialize()
{
    return S_OK;
}

void CLevel::Priority_Update(_float fTimeDelta)
{
}

void CLevel::Update(_float fTimeDelta)
{
}

void CLevel::Late_Update(_float fTimeDelta)
{
}

HRESULT CLevel::Render()
{
    return S_OK;
}

void CLevel::Free()
{
    __super::Free();

    Safe_Release(m_pGraphic_Device);
}
