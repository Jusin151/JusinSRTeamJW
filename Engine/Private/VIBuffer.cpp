#include "VIBuffer.h"

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CComponent { pGraphic_Device }
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
    : CComponent{ Prototype }
    , m_pVB { Prototype.m_pVB }
    , m_iNumVertices { Prototype.m_iNumVertices }
    , m_iVertexStride { Prototype.m_iVertexStride }
    , m_iFVF { Prototype.m_iFVF }
{
    Safe_AddRef(m_pVB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CVIBuffer::Render()
{
    m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iVertexStride);
    m_pGraphic_Device->SetFVF(m_iFVF);

    m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

    return S_OK;
}


HRESULT CVIBuffer::Create_VertexBuffer()
{
    if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iNumVertices * m_iVertexStride,
        0/*D3DUSAGE_DYNAMIC*/, m_iFVF, D3DPOOL_MANAGED, &m_pVB, 0)))
        return E_FAIL;

    return S_OK;
}

void CVIBuffer::Free()
{
    __super::Free();

    Safe_Release(m_pIB);
    Safe_Release(m_pVB);
}
