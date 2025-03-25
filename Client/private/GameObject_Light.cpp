#include "GameObject_Light.h"
#include "Light.h"
#include "Transform.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"

CGameObject_Light::CGameObject_Light(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject{ pGraphic_Device }
{
}

CGameObject_Light::CGameObject_Light(const CGameObject_Light& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CGameObject_Light::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameObject_Light::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CGameObject_Light::Priority_Update(_float fTimeDelta)
{
}

void CGameObject_Light::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CGameObject_Light::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
    m_pGameInstance->Add_Light(m_pLightCom);
}

HRESULT CGameObject_Light::Pre_Render()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    //m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    return S_OK;
}

HRESULT CGameObject_Light::Render()
{
    if(FAILED(m_pTextureCom->Bind_Resource(0)))
        return E_FAIL;
    if (FAILED(m_pTransformCom->Bind_Resource()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    Pre_Render();
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;
    Post_Render();

    return S_OK;
}

HRESULT CGameObject_Light::Post_Render()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    return S_OK;
}

HRESULT CGameObject_Light::Ready_Components()
{
    /* For.Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    CLight::LIGHT_INIT lDesc{ L"../../Resources/Lights/TestLight.json" };

    /* For.Com_Light */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Light_Point"),
        TEXT("Com_Light"), reinterpret_cast<CComponent**>(&m_pLightCom), &lDesc)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Light"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    

    return S_OK;
}


CGameObject_Light* CGameObject_Light::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CGameObject_Light* pInstance = new CGameObject_Light(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CGameObject_Light");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject_Light* CGameObject_Light::Clone(void* pArg)
{
    CGameObject_Light* pInstance = new CGameObject_Light(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CGameObject_Light");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGameObject_Light::Free()
{
    __super::Free();
    Safe_Release(m_pLightCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTransformCom);
}

json CGameObject_Light::Serialize()
{
    json j = __super::Serialize();
    auto pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    auto scale = m_pTransformCom->Compute_Scaled();
    auto angle = m_pTransformCom->Get_EulerAngles();
    j["position"] = { pos.x, pos.y, pos.z };
    j["rotation"] = { angle.x, angle.y, angle.z };
    j["scale"] = { scale.x, scale.y, scale.z };
    
    return j;
}

void CGameObject_Light::Deserialize(const json& j)
{
    SET_TRANSFORM(j, m_pTransformCom);
    m_pLightCom->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}
