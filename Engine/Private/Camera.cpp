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

   /* m_pTransformCom = CTransform::Create(m_pGraphic_Device);
    if (nullptr == m_pTransformCom)
        return E_FAIL;*/


   

    //if (FAILED(m_pTransformCom->Initialize(pArg)))
    //    return E_FAIL;

    //CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);



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


void CCamera::Free()
{
    __super::Free();


}
