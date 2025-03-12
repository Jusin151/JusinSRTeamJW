#include "Camera.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject { pGraphic_Device }
{
}

CCamera::CCamera(const CCamera& Prototype)
    : CGameObject( Prototype )
{
}

HRESULT CCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);
    
    if (nullptr == m_pTransformCom)
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vEye);
    m_pTransformCom->LookAt(pDesc->vAt);

    m_fFov = pDesc->fFov;    
    m_fNear = pDesc->fNear;
    m_fFar = pDesc->fFar;

    D3DVIEWPORT9            ViewportDesc{};
    m_pGraphic_Device->GetViewport(&ViewportDesc);

    m_fAspect = static_cast<_float>(ViewportDesc.Width) / ViewportDesc.Height;    

    return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::Late_Update(_float fTimeDelta)
{
    
}

HRESULT CCamera::Render()
{
    return S_OK;
}


HRESULT CCamera::Update_VP_Matrices()
{
    _float4x4       ViewMatrix{}, ProjMatrix{};

    m_pGraphic_Device->SetTransform(D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());

    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, D3DXMatrixPerspectiveFovLH(&ProjMatrix, m_fFov, m_fAspect, m_fNear, m_fFar));


    return S_OK;
}

void CCamera::Free()
{
    __super::Free();

    Safe_Release(m_pTransformCom);
}
