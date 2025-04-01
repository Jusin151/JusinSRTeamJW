#include "GameObject_Projectile_Test.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Material.h"
#include "Transform.h"
#include "Particles.h"
#include "Collider_Cube.h"
#include "VIBuffer_Sphere.h"


CGameObject_Projectile_Test::CGameObject_Projectile_Test(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CProjectile_Base { pGraphic_Device }
{
}

CGameObject_Projectile_Test::CGameObject_Projectile_Test(const CGameObject_Projectile_Test& Prototype)
    : CProjectile_Base { Prototype }
    , m_pMaterialCom { Prototype.m_pMaterialCom }
    , m_pParticleCom { Prototype.m_pParticleCom }
    , m_pGoldParticleCom { Prototype.m_pGoldParticleCom }
{
}

HRESULT CGameObject_Projectile_Test::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameObject_Projectile_Test::Initialize(void* pArg)
{
    /*if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;*/
    if(FAILED(Ready_Components()))
        return E_FAIL;

    m_vDir = { -1.0f, 0.f, 0.f };
    return S_OK;
}

HRESULT CGameObject_Projectile_Test::Ready_Components()
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

    /* For.Com_Material */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
        TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
        return E_FAIL;

    CProjectile_Particle_System::TRAILDESC particleDesc = {};
    particleDesc.iNumParticles = { 1 };
    particleDesc.fDistance = { 10.f };
    particleDesc.fWidth = { 2.f };
    particleDesc.strTexturePath = L"../../Resources/Textures/Particle/sprite_blood_particle.png";

    /* For.Com_Particle */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Projectile"),
        TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &particleDesc)))
        return E_FAIL;

    CGold_Particle_System::GOLDDESC goldDesc = {};
    goldDesc.Bounding_Box.m_vMin = { -1, -1, -1 };
    goldDesc.Bounding_Box.m_vMax = { 1, 1, 1 };
    goldDesc.iNumParticles = { 10 };
    goldDesc.strTexturePath = L"../../Resources/Textures/Particle/particle_gold.png";

    /* For.Com_GoldParticle */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Gold"),
        TEXT("Com_GoldParticle"), reinterpret_cast<CComponent**>(&m_pGoldParticleCom), &goldDesc)))
        return E_FAIL;

    CBlood_Particle_System::BLOODDESC bloodDesc = {};
    bloodDesc.Bound.m_vCenter = { 0.f , 0.f, 0.f };
    bloodDesc.Bound.m_fRadius = 1.f;
    bloodDesc.iNumParticles = { 100 };
    bloodDesc.strTexturePath = L"../../Resources/Textures/Particle/sprite_blood_particle.png";

    /* For.Com_BloodParticle */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Blood"),
        TEXT("Com_BloodParticle"), reinterpret_cast<CComponent**>(&m_pBloodParticleCom), &bloodDesc)))
        return E_FAIL;


    CBulletShell_Particle_System::BULLETSHELLDESC shellDesc = {};
    shellDesc.Bounding_Box.m_vMin = { -1, -1, -1 };
    shellDesc.Bounding_Box.m_vMax = { 1, 1, 1 };
    shellDesc.iNumParticles = { 10 };
    shellDesc.strTexturePath = L"../../Resources/Textures/Particle/sprite_blood_particle.png";

    /* For.Com_BulletShellParticle */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_BulletShell"),
        TEXT("Com_BulletShellParticle"), reinterpret_cast<CComponent**>(&m_pBulletShellParticleCom), &shellDesc)))
        return E_FAIL;

    return S_OK;
}

void CGameObject_Projectile_Test::Priority_Update(_float fTimeDelta)
{
}

void CGameObject_Projectile_Test::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    m_pParticleCom->Update(fTimeDelta);
    m_pGoldParticleCom->Update(fTimeDelta);
    m_pBloodParticleCom->Update(fTimeDelta);
    
    m_pTransformCom->Go(m_vDir, fTimeDelta);

    dynamic_cast<CProjectile_Particle_System*>(m_pParticleCom)->Set_Dir(m_vDir);
    //m_pParticleCom->Set_Origin(m_vDir);
}

void CGameObject_Projectile_Test::Late_Update(_float fTimeDelta)
{
    if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).x > 50.f || m_pTransformCom->Get_State(CTransform::STATE_POSITION).x < -50.f)
        m_vDir *= -1.f;
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CGameObject_Projectile_Test::Pre_Render()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    //m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    return S_OK;
}

HRESULT CGameObject_Projectile_Test::Render()
{
    Pre_Render();

    if (FAILED(m_pTextureCom->Bind_Resource(0)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pMaterialCom->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(m_pParticleCom->Render()))
        return E_FAIL;

    if (FAILED(m_pGoldParticleCom->Render()))
        return E_FAIL;

    if (FAILED(m_pBloodParticleCom->Render()))
        return E_FAIL;

    Post_Render();

    return S_OK;
}

HRESULT CGameObject_Projectile_Test::Post_Render()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    return S_OK;
}

CGameObject_Projectile_Test* CGameObject_Projectile_Test::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CGameObject_Projectile_Test* pInstance = new CGameObject_Projectile_Test(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CGameObject_Projectile_Test");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject_Projectile_Test* CGameObject_Projectile_Test::Clone(void* pArg)
{
    CGameObject_Projectile_Test* pInstance = new CGameObject_Projectile_Test(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CGameObject_Projectile_Test");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGameObject_Projectile_Test::Free()
{
    __super::Free();
    Safe_Release(m_pMaterialCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pParticleCom);
    Safe_Release(m_pGoldParticleCom);
    Safe_Release(m_pBloodParticleCom);
    Safe_Release(m_pBulletShellParticleCom);
}

json CGameObject_Projectile_Test::Serialize()
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

void CGameObject_Projectile_Test::Deserialize(const json& j)
{
    SET_TRANSFORM(j, m_pTransformCom);
}
