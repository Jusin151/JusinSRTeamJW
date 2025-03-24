#include "BlendObject.h"

CBlendObject::CBlendObject(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject { pGraphic_Device }
{
}

CBlendObject::CBlendObject(const CBlendObject& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CBlendObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBlendObject::Initialize(void* pArg)
{
    return S_OK;
}

void CBlendObject::Priority_Update(_float fTimeDelta)
{
}

void CBlendObject::Update(_float fTimeDelta)
{
}

void CBlendObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CBlendObject::Render()
{
    return S_OK;
}

void CBlendObject::Compute_CameraDepth(const _float3& vPosition)
{
    _float4x4       ViewMatrix{};

    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
    D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

    _float3         vCamPosition = *reinterpret_cast<_float3*>(&ViewMatrix.m[3][0]);

    _float3         vDistance = vCamPosition - vPosition;

    m_fDepth = D3DXVec3Length(&vDistance);
}

void CBlendObject::Free()
{
    __super::Free();

}
