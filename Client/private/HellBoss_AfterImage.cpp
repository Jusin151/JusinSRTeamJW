#include "HellBoss_AfterImage.h"
#include "GameInstance.h"

CHellBoss_AfterImage::CHellBoss_AfterImage(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject(pGraphic_Device)
{
}

HRESULT CHellBoss_AfterImage::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    DESC* pDesc = static_cast<DESC*>(pArg);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, pDesc->vRight);
    m_pTransformCom->Set_State(CTransform::STATE_UP, pDesc->vUp);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, pDesc->vLook);

    m_iFrame = pDesc->iAnimFrame;

    return S_OK;
}

void CHellBoss_AfterImage::Update(_float fDeltaTime)
{
    m_fLifeTime += fDeltaTime;
    m_fAlpha -= fDeltaTime * 2.f;

    if (m_fLifeTime >= 0.3f || m_fAlpha <= 0.f)
    {
        SetActive(false);
    }
}

void CHellBoss_AfterImage::Late_Update(_float fDeltaTime)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

HRESULT CHellBoss_AfterImage::Render()
{
    if (FAILED(SetUp_RenderState()))
        return E_FAIL;

    m_pTransformCom->Bind_Resource();
    m_pTextureCom->Bind_Resource(0);
   // m_pShaderCom->Bind_Transform();
    //m_pShaderCom->Bind_Texture(m_pTextureCom, m_iFrame);

  //  m_pShaderCom->Begin(1);
    m_pVIBufferCom->Render();
    //m_pShaderCom->End();

    if (FAILED(Release_RenderState()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHellBoss_AfterImage::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_HONG, TEXT("Prototype_Component_Texture_HellBoss_AfterImage"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CHellBoss_AfterImage::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    return S_OK;
}

HRESULT CHellBoss_AfterImage::Release_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    return S_OK;
}

CHellBoss_AfterImage* CHellBoss_AfterImage::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CHellBoss_AfterImage* pInstance = new CHellBoss_AfterImage(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CHellBoss_AfterImage");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHellBoss_AfterImage::Clone(void* pArg)
{
    CHellBoss_AfterImage* pInstance = new CHellBoss_AfterImage(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CHellBoss_AfterImage");
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CHellBoss_AfterImage::Free()
{
    __super::Free();
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);


}

