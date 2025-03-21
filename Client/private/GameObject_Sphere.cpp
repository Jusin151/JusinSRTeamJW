#include "GameObject_Sphere.h"
#include "GameInstance.h"
#include "VIBuffer_Sphere.h"

CGameObject_Sphere::CGameObject_Sphere(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject{ pGraphic_Device }
{
}

CGameObject_Sphere::CGameObject_Sphere(const CGameObject& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CGameObject_Sphere::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameObject_Sphere::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CGameObject_Sphere::Priority_Update(_float fTimeDelta)
{
}

void CGameObject_Sphere::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CGameObject_Sphere::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CGameObject_Sphere::Pre_Render()
{
    //m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    D3DMATERIAL9 material;
    ZeroMemory(&material, sizeof(D3DMATERIAL9));

    // 확산광 (Diffuse) 설정: 흰색
    material.Diffuse.r = 1.0f;
    material.Diffuse.g = 1.0f;
    material.Diffuse.b = 1.0f;
    material.Diffuse.a = 1.0f;

    // 주변광 (Ambient) 설정: 약한 회색
    material.Ambient.r = 0.2f;
    material.Ambient.g = 0.2f;
    material.Ambient.b = 0.2f;
    material.Ambient.a = 1.0f;

    // 반사광 (Specular) 설정: 없음
    material.Specular.r = 0.0f;
    material.Specular.g = 0.0f;
    material.Specular.b = 0.0f;
    material.Specular.a = 1.0f;

    // 방출광 (Emissive) 설정: 없음
    material.Emissive.r = 0.0f;
    material.Emissive.g = 0.0f;
    material.Emissive.b = 0.0f;
    material.Emissive.a = 1.0f;

    // 광택 (Power) 설정: 0 (반사광 없음)
    material.Power = 0.0f;

    // 생성된 마테리얼을 디바이스에 설정
    m_pGraphic_Device->SetMaterial(&material);
    return S_OK;
}

HRESULT CGameObject_Sphere::Render()
{
    if (FAILED(m_pTextureCom->Bind_Resource(0)))
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

HRESULT CGameObject_Sphere::Post_Render()
{
    //m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    return S_OK;
}

HRESULT CGameObject_Sphere::Ready_Components()
{
    /* For.Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Sphere"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Base"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}


json CGameObject_Sphere::Serialize()
{
    json j = __super::Serialize();


    //j["texture_tag"] = WideToUtf8(m_strTextureTag);
    //j["texture_path"] = WideToUtf8(m_tStructure_Desc.stTexturePath); 
    //j["vibuffer"] = WideToUtf8(m_strVIBuffer);
    //j["collider_tag"] = WideToUtf8(m_strCollProtoTag);

    auto pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    auto scale = m_pTransformCom->Compute_Scaled();
    auto angle = m_pTransformCom->Get_EulerAngles();
    j["position"] = { pos.x, pos.y, pos.z };
    j["rotation"] = { angle.x, angle.y, angle.z };
    j["scale"] = { scale.x, scale.y, scale.z };
    return j;
}

void CGameObject_Sphere::Deserialize(const json& j)
{
    SET_TRANSFORM(j, m_pTransformCom);
}

CGameObject_Sphere* CGameObject_Sphere::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CGameObject_Sphere* pInstance = new CGameObject_Sphere(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject_Sphere* CGameObject_Sphere::Clone(void* pArg)
{
    CGameObject_Sphere* pInstance = new CGameObject_Sphere(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CGameObject_Sphere");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGameObject_Sphere::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);
}