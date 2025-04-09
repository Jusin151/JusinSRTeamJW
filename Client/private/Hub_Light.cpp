#include "GameInstance.h"
#include "Hub_Light.h"
#include "Player.h"

CHub_Light::CHub_Light(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject{ pGraphic_Device }
{
}

CHub_Light::CHub_Light(const CHub_Light& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CHub_Light::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHub_Light::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Ready_Components()))
        return E_FAIL;
    return S_OK;
}

HRESULT CHub_Light::Ready_Components()
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
    if (FAILED(__super::Add_Component(LEVEL_HUB, TEXT("Prototype_Component_Texture_Hub_Light"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    CLight::LIGHT_INIT lightInit = { L"../../Resources/Lights/HubLight.json" };

    /* For.Com_Light */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Light_Point"),
        TEXT("Com_Light"), reinterpret_cast<CComponent**>(&m_pLightCom), &lightInit)))
        return E_FAIL;

    CMaterial::MATERIAL_DESC materialDesc = {};

    /* For.Com_Material */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
        TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}


void CHub_Light::Priority_Update(_float fTimeDelta)
{
    
}

void CHub_Light::Update(_float fTimeDelta)
{
    _float4x4 view;
    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &view);
    _float4x4 viewInverse = {};
    D3DXMatrixInverse(&viewInverse, nullptr, &view);
    _float3 cameraPos = *reinterpret_cast<_float3*>(&viewInverse.m[3][0]);
    m_pTransformCom->LookAt(cameraPos);
}

void CHub_Light::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
    m_pGameInstance->Add_Light(m_pLightCom);
    _float3 tempPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    tempPos += _float3{ 0.f, 0.35f, 0.f };
    m_pLightCom->Set_Position(tempPos);
}

HRESULT CHub_Light::Pre_Render()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
    _float2 ScaleFactor = { 1.0f, 1.0f };
    _float2 Offset = { 0.f, 0.f };
    m_pShaderCom->Set_UVScaleFactor(&ScaleFactor);
    m_pShaderCom->Set_UVOffsetFactor(&Offset);
    return S_OK;
}

HRESULT CHub_Light::Render()
{
    Pre_Render();
    if (FAILED(m_pTransformCom->Bind_Resource()))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_Resource(0)))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, 0)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Transform()))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Lights()))
        return E_FAIL;
    m_pShaderCom->Begin(2);
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;
    m_pShaderCom->End();
    Post_Render();
    return S_OK;
}

HRESULT CHub_Light::Post_Render()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    return S_OK;
}


CHub_Light* CHub_Light::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CHub_Light* pInstance = new CHub_Light(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CHub_Light");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CHub_Light* CHub_Light::Clone(void* pArg)
{
    CHub_Light* pInstance = new CHub_Light(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CHub_Light");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHub_Light::Free()
{
    __super::Free();
    Safe_Release(m_pLightCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pMaterialCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTransformCom);
}

json CHub_Light::Serialize()
{
    return json();
}

void CHub_Light::Deserialize(const json& j)
{
    SET_TRANSFORM(j, m_pTransformCom);
}
