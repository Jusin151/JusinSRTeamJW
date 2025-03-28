#include "Sound_Source.h"
#include "Sound_Event.h"

CSound_Source::CSound_Source(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CComponent{}
{
}

CSound_Source::CSound_Source(const CSound_Source& Prototype)
    : CComponent{ Prototype };
{
}

HRESULT CSound_Source::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSound_Source::Initialize(void* pArg)
{
    return S_OK;
}

void CSound_Source::Priority_Update(_float fTimeDelta)
{
}

void CSound_Source::Update(_float fTimeDelta)
{

}

void CSound_Source::Late_Update(_float fTimeDelta)
{
}

CSound_Event CSound_Source::Play_Event()
{
    return CSound_Event();
}

void CSound_Source::Stop_All_Event()
{
}

CSound_Source* CSound_Source::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CSound_Source* pInstance = new CSound_Source(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype())
    {
        MSG_BOX("Failed to Created : CSound_Source");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CSound_Source* CSound_Source::Clone(void* pArg)
{
    CSound_Source* pInstance = new CSound_Source(*this);

    if (FAILED(pInstance->Initialize(pArg))
    {
        MSG_BOX("Failed to Created : CSound_Source");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSound_Source::Free()
{
    __super::Free();
}
