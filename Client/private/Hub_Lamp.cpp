#include "GameInstance.h"
#include "Hub_Lamp.h"

CHub_Lamp::CHub_Lamp(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject{ pGraphic_Device }
{
}

CHub_Lamp::CHub_Lamp(const CHub_Lamp& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CHub_Lamp::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHub_Lamp::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHub_Lamp::Ready_Components()
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


void CHub_Lamp::Priority_Update(_float fTimeDelta)
{
}

void CHub_Lamp::Update(_float fTimeDelta)
{
}

void CHub_Lamp::Late_Update(_float fTimeDelta)
{
}

HRESULT CHub_Lamp::Pre_Render()
{
    return S_OK;
}

HRESULT CHub_Lamp::Render()
{
    return S_OK;
}

HRESULT CHub_Lamp::Post_Render()
{
    return S_OK;
}


CHub_Lamp* CHub_Lamp::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CHub_Lamp* pInstance = new CHub_Lamp(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Cloned : CHub_Lamp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CHub_Lamp* CHub_Lamp::Clone(void* pArg)
{
    CHub_Lamp* pInstance = new CHub_Lamp(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CHub_Lamp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHub_Lamp::Free()
{
    __super::Free();
    Safe_Release(m_pLightCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pMaterialCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTransformCom);
}

json CHub_Lamp::Serialize()
{
    return json();
}

void CHub_Lamp::Deserialize(const json& j)
{
    SET_TRANSFORM(j, m_pTransformCom);
}
