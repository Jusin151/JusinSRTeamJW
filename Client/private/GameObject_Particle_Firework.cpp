#include "GameObject_Particle_Firework.h"
#include "Texture.h"
#include "Transform.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"
#include "Particles.h"

CGameObject_Particle_Firework::CGameObject_Particle_Firework(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject{ pGraphic_Device }
{
}

CGameObject_Particle_Firework::CGameObject_Particle_Firework(const CGameObject& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CGameObject_Particle_Firework::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameObject_Particle_Firework::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CGameObject_Particle_Firework::Priority_Update(_float fTimeDelta)
{
}

void CGameObject_Particle_Firework::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    m_pParticleCom->Update(fTimeDelta);
}

void CGameObject_Particle_Firework::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

HRESULT CGameObject_Particle_Firework::Pre_Render()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    //m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    return S_OK;
}

HRESULT CGameObject_Particle_Firework::Render()
{
    if (FAILED(m_pTransformCom->Bind_Resource()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    Pre_Render();
    if (FAILED(m_pParticleCom->Render()))
        return E_FAIL;
    Post_Render();

    return S_OK;
}

HRESULT CGameObject_Particle_Firework::Post_Render()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    return S_OK;
}

HRESULT CGameObject_Particle_Firework::Ready_Components()
{
    /* For.Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    CFirework_Particle_System::FIREWORKDESC     FireworkDesc{};
    FireworkDesc.Bounding_Box.m_vMin = { -2, -2, -2 };
    FireworkDesc.Bounding_Box.m_vMax = { 2, 2, 2 };
    FireworkDesc.vOrigin = { 5.0f, 5.0f, 0.0f }; //m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    FireworkDesc.iNumParticles = 512;
    FireworkDesc.strTexturePath = L"../../Resources/Textures/Particle/sprite_blood_particle.png";
    FireworkDesc.iNumTextures = { 1 };

    /* For.Com_Particle */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Firework"),
        TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &FireworkDesc)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;    

    return S_OK;
}


CGameObject_Particle_Firework* CGameObject_Particle_Firework::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CGameObject_Particle_Firework* pInstance = new CGameObject_Particle_Firework(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CGameObject_Particle_Firework");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject_Particle_Firework* CGameObject_Particle_Firework::Clone(void* pArg)
{
    CGameObject_Particle_Firework* pInstance = new CGameObject_Particle_Firework(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CGameObject_Particle_Firework");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGameObject_Particle_Firework::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pParticleCom);
    Safe_Release(m_pTransformCom);
}

json CGameObject_Particle_Firework::Serialize()
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

void CGameObject_Particle_Firework::Deserialize(const json& j)
{
    SET_TRANSFORM(j, m_pTransformCom);
}
