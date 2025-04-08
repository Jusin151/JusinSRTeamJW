#include "GameInstance.h"
#include "Long_Torch.h"

CLong_Torch::CLong_Torch(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject{ pGraphic_Device }
{
}

CLong_Torch::CLong_Torch(const CLong_Torch& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CLong_Torch::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLong_Torch::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLong_Torch::Ready_Components()
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


void CLong_Torch::Priority_Update(_float fTimeDelta)
{
}

void CLong_Torch::Update(_float fTimeDelta)
{
}

void CLong_Torch::Late_Update(_float fTimeDelta)
{
}

HRESULT CLong_Torch::Pre_Render()
{
    return S_OK;
}

HRESULT CLong_Torch::Render()
{
    return S_OK;
}

HRESULT CLong_Torch::Post_Render()
{
    return S_OK;
}


CLong_Torch* CLong_Torch::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CLong_Torch* pInstance = new CLong_Torch(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Cloned : CLong_Torch");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CLong_Torch* CLong_Torch::Clone(void* pArg)
{
    CLong_Torch* pInstance = new CLong_Torch(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CLong_Torch");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLong_Torch::Free()
{
    __super::Free();
    Safe_Release(m_pLightCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pMaterialCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTransformCom);
}

json CLong_Torch::Serialize()
{
    return json();
}

void CLong_Torch::Deserialize(const json& j)
{
    SET_TRANSFORM(j, m_pTransformCom);
}
