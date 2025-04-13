#include "BulletShell_Particle_System.h"

CBulletShell_Particle_System::CBulletShell_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CParticle_System{ pGraphic_Device }
{
}

CBulletShell_Particle_System::CBulletShell_Particle_System(const CBulletShell_Particle_System& Prototype)
    : CParticle_System{ Prototype }
{
}

HRESULT CBulletShell_Particle_System::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBulletShell_Particle_System::Initialize(void* pArg)
{
    BULLETSHELLDESC desc = *reinterpret_cast<BULLETSHELLDESC*>(pArg);
    m_Bounding_Box = desc.Bounding_Box;
    m_vPos = { 0.f, 0.f, 0.f };
    m_fSize = 0.8f;
    m_VBSize = 2048;
    m_VBOffset = 0;
    m_VBBatchSize = 512;
    m_iMaxParticles = desc.iNumParticles;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    for (_uint i = 0; i < m_iMaxParticles; ++i)
    {
        Add_Particle();
    }
    return S_OK;
}

void CBulletShell_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute)
{
    pAttribute->bIsAlive = true;
    pAttribute->vPosition = { 0.f, 0.f, 0.f };
    //GetRandomVector(&pAttribute->vPosition, &m_Bounding_Box.m_vMin, &m_Bounding_Box.m_vMax);
    pAttribute->vVelocity = { GetRandomFloat(-1.f, 1.0f), GetRandomFloat(0.f, 5.0f), GetRandomFloat(-1.f, 1.0f) };
    pAttribute->vAcceleration = { 1.5f, 1.2f, 0.0f };
    pAttribute->fAge = 0;
    pAttribute->fLifetime = 10.0f;
    pAttribute->iIndex = 0;
    //pAttribute->vDir = {0.f, 1.f, 0.f};
    //pAttribute->vDir = { GetRandomFloat(0.f, 1.0f), GetRandomFloat(0.f, 1.0f), GetRandomFloat(0.f, 1.0f) };
    pAttribute->vCurrentColor = 0xFFFFFFFF;
    pAttribute->fSize = m_fSize;
}

void CBulletShell_Particle_System::Update(float fTimeDelta)
{
    /*for (auto& i : m_Particles)
    {
        if (i.bIsAlive)
        {
            _float3 worldPos = {};
            D3DXVec3TransformCoord(&worldPos, &i.vPosition, &m_WorldMat);
            if (worldPos.y - m_Bounding_Box.m_vMin.y > 0.1f)
            {
                i.vPosition += (i.vVelocity * i.vAcceleration.x) * fTimeDelta;
                i.vVelocity.y -= GRAVITY * fTimeDelta;
            }
            i.fAge += fTimeDelta;
        }
        if (i.fAge > i.fLifetime)
            i.bIsAlive = false;
    }*/
    __super::Late_Update(fTimeDelta);
}

HRESULT CBulletShell_Particle_System::Render()
{
    __super::Pre_Render();

    PARTICLE* v = 0;
    m_pTexture->Bind_Resource(0);
    m_pGraphic_Device->SetFVF(D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE | D3DFVF_TEX1);
    m_pGraphic_Device->SetStreamSource(0, m_PointVB, 0, sizeof(PARTICLE));

    m_PointVB->Lock(
        m_VBOffset * sizeof(PARTICLE),
        m_VBBatchSize * sizeof(PARTICLE),
        (void**)&v,
        m_VBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

    ATTRIBUTE i = *m_Particles.begin();

    //i.vDir 에서 둘
    //-i.vDir 에서 둘
    _float3 vNormal = {};
    D3DXVec3Normalize(&vNormal, &i.vDir);
    D3DXVECTOR3 up = { 0.0f, 1.0f, 0.0f };
    D3DXVec3Cross(&vNormal, &i.vDir, &up);
    D3DXVec3Normalize(&vNormal, &vNormal);
    //v[0].vPosition = m_vPos - vNormal * 4.f / 2.f;
    v[0].vPosition = i.vPosition + _float3{ -0.5f, 0.5f, 0.0f }; // 왼쪽 위 (Top-Left)
    v[0].fSize = i.fSize;
    v[0].vColor = i.vCurrentColor;
    v[0].vTexCoord = { 0, 0 };
    ////v[1].vPosition = m_vPos + vNormal * 4.f / 2.f;
    v[1].vPosition = i.vPosition + _float3{ 0.5f,  0.5f, 0.0f }; // 오른쪽 위 (Top-Right)
    v[1].fSize = i.fSize;
    v[1].vColor = i.vCurrentColor;
    v[1].vTexCoord = { 1, 0 };
    ////v[2].vPosition = m_vPos - vNormal * 4.f / 2.f;
    v[2].vPosition = i.vPosition + _float3{ -0.5f, -0.5f, -0.0f }; // 왼쪽 아래 (Bottom-Left)
    v[2].fSize = i.fSize;
    v[2].vColor = i.vCurrentColor;
    v[2].vTexCoord = { 0, 1 };
    ////v[3].vPosition = m_vPos + vNormal * 4.f / 2.f;
    v[3].vPosition = i.vPosition + _float3{ 0.5f, -0.5f, -0.0f }; // 오른쪽 아래 (Bottom-Right)
    v[3].fSize = i.fSize;
    v[3].vColor = i.vCurrentColor;
    v[3].vTexCoord = { 1, 1 };
    m_PointVB->Unlock();
    //m_pGraphic_Device->DrawPrimitive(D3DPT_POINTLIST, 0, 4);
    //m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
    m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    __super::Post_Render();
    return S_OK;
}

CBulletShell_Particle_System* CBulletShell_Particle_System::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CBulletShell_Particle_System* pInstance = new CBulletShell_Particle_System(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBulletShell_Particle_System");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CBulletShell_Particle_System* CBulletShell_Particle_System::Clone(void* pArg)
{
    CBulletShell_Particle_System* pInstance = new CBulletShell_Particle_System(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBulletShell_Particle_System");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBulletShell_Particle_System::Free()
{
    __super::Free();
}
    