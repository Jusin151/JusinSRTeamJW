#include "Sound_Source.h"
#include "Sound_Manager.h"

CSound_Source::CSound_Source(LPDIRECT3DDEVICE9 pGraphic_Device, CSound_Manager* pSound_Manager)
    : CComponent{ pGraphic_Device }
    , m_pSound_Manager { pSound_Manager }
{
    Safe_AddRef(pSound_Manager);
}

CSound_Source::CSound_Source(const CSound_Source& Prototype)
    : CComponent{ Prototype }
    , m_pSound_Manager { Prototype.m_pSound_Manager }
{
    Safe_AddRef(m_pSound_Manager);
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
    // 유효하지 않은 2D event 삭제
    auto iter = m_Events2D.begin();
    while (iter != m_Events2D.end())
    {
        (*iter)->IsValid() ? iter = m_Events2D.erase(iter) : iter;
    }

    // 유효하지 않는 3D event 삭제
    iter = m_Events3D.begin();
    while (iter != m_Events3D.end())
    {
        (*iter)->IsValid() ? iter = m_Events3D.erase(iter) : iter;
    }
}

void CSound_Source::Late_Update(_float fTimeDelta)
{
}

CSound_Event* CSound_Source::Play_Event(_wstring strEvent)
{
    CSound_Event* e = m_pSound_Manager->Play_Event(strEvent);
    if (e->Is3D())
    {
        m_Events3D.emplace_back(e);
        //e.Set3DAttributes();
    }
    else
    {
        m_Events2D.emplace_back(e);
    }
    return e;
}

void CSound_Source::Stop_All_Event()
{
    for (auto& e : m_Events2D)
    {
        e->Stop();
    }
    for (auto& e : m_Events3D)
    {
        e->Stop();
    }

    // 모든 이벤트를 컨테이너에서 삭제한다.
    m_Events2D.clear();
    m_Events3D.clear();
}

CSound_Source* CSound_Source::Create(LPDIRECT3DDEVICE9 pGraphic_Device, CSound_Manager* pSound_Manager)
{
    CSound_Source* pInstance = new CSound_Source(pGraphic_Device, pSound_Manager);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSound_Source");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CSound_Source* CSound_Source::Clone(void* pArg)
{
    CSound_Source* pInstance = new CSound_Source(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CSound_Source");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSound_Source::Free()
{
    __super::Free();
    for (auto& iter : m_Events2D)
    {
        Safe_Release(iter);
    }
    for (auto& iter : m_Events3D)
    {
        Safe_Release(iter);
    }
    Safe_Release(m_pSound_Manager);
}
