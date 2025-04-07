#include "GameInstance.h"
#include "Goblet.h"

CGoblet::CGoblet(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject{ pGraphic_Device }
{
}

CGoblet::CGoblet(const CGoblet& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CGoblet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGoblet::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGoblet::Ready_Components()
{
    /* For.Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Base"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* For.Com_Light */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
        TEXT("Com_Light"), reinterpret_cast<CComponent**>(&m_pLightCom))))
        return E_FAIL;

    CMaterial::MATERIAL_DESC materialDesc = {};

    /* For.Com_Material */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
        TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}


void CGoblet::Priority_Update(_float fTimeDelta)
{
}

void CGoblet::Update(_float fTimeDelta)
{
}

void CGoblet::Late_Update(_float fTimeDelta)
{
}

HRESULT CGoblet::Pre_Render()
{
    return S_OK;
}

HRESULT CGoblet::Render()
{
    return S_OK;
}

HRESULT CGoblet::Post_Render()
{
    return S_OK;
}


CGoblet* CGoblet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CGoblet* pInstance = new CGoblet(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Cloned : CGoblet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGoblet* CGoblet::Clone(void* pArg)
{
    CGoblet* pInstance = new CGoblet(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CGoblet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGoblet::Free()
{
    __super::Free();
    Safe_Release(m_pLightCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pMaterialCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTransformCom);
}

json CGoblet::Serialize()
{
    return json();
}

void CGoblet::Deserialize(const json& j)
{
    SET_TRANSFORM(j, m_pTransformCom);
}
