#include "GameObject_Plane.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Particles.h"

CGameObject_Plane::CGameObject_Plane(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject{ pGraphic_Device }
{
}

CGameObject_Plane::CGameObject_Plane(const CGameObject& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CGameObject_Plane::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameObject_Plane::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CGameObject_Plane::Priority_Update(_float fTimeDelta)
{
}

void CGameObject_Plane::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    m_pBloodParticleCom->Update(fTimeDelta);
}

void CGameObject_Plane::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CGameObject_Plane::Pre_Render()
{
    return E_NOTIMPL;
}

HRESULT CGameObject_Plane::Render()
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
    if (FAILED(m_pBloodParticleCom->Render()))
        return E_FAIL;

    Post_Render();

    return S_OK;
}

HRESULT CGameObject_Plane::Post_Render()
{
    return S_OK;
}

HRESULT CGameObject_Plane::Ready_Components()
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

    CBlood_Particle_System::BLOODDESC bloodDesc = {};
    bloodDesc.iNumParticles = { 100u };
    bloodDesc.Bound.m_vCenter = { 0.f, 0.f, 0.f };
    bloodDesc.Bound.m_fRadius = 0.1f;
    bloodDesc.iNumParticles = { 100 };
    bloodDesc.strTexturePath = L"../../Resources/Textures/Particle/sprite_blood_particle.png";

    /* For.Com_BloodParticle */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Blood"),
        TEXT("Com_BloodParticle"), reinterpret_cast<CComponent**>(&m_pBloodParticleCom), &bloodDesc)))
        return E_FAIL;

    return S_OK;
}


CGameObject_Plane* CGameObject_Plane::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CGameObject_Plane* pInstance = new CGameObject_Plane(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CGameObject_Plane");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject_Plane* CGameObject_Plane::Clone(void* pArg)
{
    CGameObject_Plane* pInstance = new CGameObject_Plane(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CGameObject_Plane");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGameObject_Plane::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pBloodParticleCom);
}

json CGameObject_Plane::Serialize()
{
    return json();
}

void CGameObject_Plane::Deserialize(const json& j)
{
    SET_TRANSFORM(j, m_pTransformCom);
}
