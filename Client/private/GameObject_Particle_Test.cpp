#include "GameObject_Particle_Test.h"
#include "Texture.h"
#include "Transform.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"
#include "Particles.h"

CGameObject_Particle_Test::CGameObject_Particle_Test(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject{ pGraphic_Device }
{
}

CGameObject_Particle_Test::CGameObject_Particle_Test(const CGameObject& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CGameObject_Particle_Test::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameObject_Particle_Test::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CGameObject_Particle_Test::Priority_Update(_float fTimeDelta)
{
}

void CGameObject_Particle_Test::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    m_pParticleCom->Update(fTimeDelta);
}

void CGameObject_Particle_Test::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CGameObject_Particle_Test::Pre_Render()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    //m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    return S_OK;
}

HRESULT CGameObject_Particle_Test::Render()
{
    if (FAILED(m_pTransformCom->Bind_Resource()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    Pre_Render();
    /*if (FAILED(m_pParticleCom->Render()))
        return E_FAIL;*/
    Post_Render();

    return S_OK;
}

HRESULT CGameObject_Particle_Test::Post_Render()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    return S_OK;
}

HRESULT CGameObject_Particle_Test::Ready_Components()
{
    /* For.Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    CSnow_Particle_System::SNOWDESC     SnowDesc{};
    SnowDesc.Bounding_Box.m_vMin = { -10, -10, -10};
    SnowDesc.Bounding_Box.m_vMax = { 10, 10, 10};
    SnowDesc.iNumParticles = 512;
    SnowDesc.strShaderPath = L"VertexShader.hlsl";

    SnowDesc.strTexturePath = L"../../Resources/Textures/Particle/snowflake.dds";

    /* For.Com_Particle */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Snow"),
        TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &SnowDesc)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}


CGameObject_Particle_Test* CGameObject_Particle_Test::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CGameObject_Particle_Test* pInstance = new CGameObject_Particle_Test(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CGameObject_Particle_Test");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject_Particle_Test* CGameObject_Particle_Test::Clone(void* pArg)
{
    CGameObject_Particle_Test* pInstance = new CGameObject_Particle_Test(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CGameObject_Particle_Test");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGameObject_Particle_Test::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pParticleCom);
    Safe_Release(m_pTransformCom);
}

json CGameObject_Particle_Test::Serialize()
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

void CGameObject_Particle_Test::Deserialize(const json& j)
{
    SET_TRANSFORM(j, m_pTransformCom);
}
