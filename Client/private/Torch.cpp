#include "GameInstance.h"
#include "Torch.h"

CTorch::CTorch(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject { pGraphic_Device }
{
}

CTorch::CTorch(const CTorch& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CTorch::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTorch::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTorch::Ready_Components()
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

    CMaterial::MATERIAL_DESC materialDesc = {};

    /* For.Com_Material */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
        TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
        return E_FAIL;

    CShader::SHADER_DESC shaderDesc = { L"../../Resources/Shaders/DoubleSideRect.hlsl" };

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), &shaderDesc)))
        return E_FAIL;

    return S_OK;
}


void CTorch::Priority_Update(_float fTimeDelta)
{
}

void CTorch::Update(_float fTimeDelta)
{
}

void CTorch::Late_Update(_float fTimeDelta)
{
}

HRESULT CTorch::Pre_Render()
{
    return S_OK;
}

HRESULT CTorch::Render()
{
    return S_OK;
}

HRESULT CTorch::Post_Render()
{
    return S_OK;
}


CTorch* CTorch::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CTorch* pInstance = new CTorch(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Cloned : CTorch");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CTorch* CTorch::Clone(void* pArg)
{
    CTorch* pInstance = new CTorch(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CTorch");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTorch::Free()
{
    __super::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pMaterialCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTransformCom);
}

json CTorch::Serialize()
{
    return json();
}

void CTorch::Deserialize(const json& j)
{
    SET_TRANSFORM(j, m_pTransformCom);
}
