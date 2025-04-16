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
        __super::Add_Particle();
    }
    return S_OK;
}

void CBulletShell_Particle_System::Add_Particle(BOUNDINGBOX bound)
{
    ATTRIBUTE attribute = {};
    m_Bounding_Box = bound;
    Reset_Particle(&attribute);
    m_Particles.push_back(attribute);
}

void CBulletShell_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute, void* pArg)
{
    pAttribute->bIsAlive = true;
    pAttribute->vPosition = m_vPos;
    pAttribute->vVelocity = { GetRandomFloat(0.f, 3.0f), GetRandomFloat(0.f, 5.0f), GetRandomFloat(-1.f, 1.0f) };
    pAttribute->vAcceleration = { 1.5f, 1.2f, 0.0f };
    pAttribute->fAge = 0;
    pAttribute->fLifetime = 10.0f;
    pAttribute->iIndex = 0;
    //pAttribute->vDir = {-1.f, 0.f, 1.f};
    pAttribute->vDir = { GetRandomFloat(-1.2f, -1.f), GetRandomFloat(0.8f, 1.2f) ,  GetRandomFloat(0.8f, 1.2f) };
    pAttribute->vCurrentColor = 0xFFFFFFFF;
    pAttribute->fSize = m_fSize;
}
//event:/Weapons/case_pistol_3
void CBulletShell_Particle_System::Update(float fTimeDelta)
{
    for (auto& i : m_Particles)
    {
        if (i.bIsAlive)
        {
            _float3 worldPos = {};
            D3DXVec3TransformCoord(&worldPos, &i.vPosition, &m_WorldMat);
            if (worldPos.y > m_Bounding_Box.m_vMin.y - 0.75f)
            {
                i.vPosition += (i.vVelocity * i.vAcceleration.x) * fTimeDelta;
                i.vVelocity.y -= GRAVITY * fTimeDelta;
                i.vDir += _float3(GetRandomFloat(-1.0f, 1.0f), 0.f, GetRandomFloat(-1.0f, 1.0f));
            }
            i.fAge += fTimeDelta;
        }
        
        if (i.fAge > i.fLifetime)
            i.bIsAlive = false;
    }
    __super::Late_Update(fTimeDelta);
}

HRESULT CBulletShell_Particle_System::Render()
{
    __super::Pre_Render();
    for (const auto& i : m_Particles) // 모든 파티클(i)에 대해 반복
    {
        PARTICLE* v = 0;
        m_pTexture->Bind_Resource(0);
        m_pGraphic_Device->SetFVF(D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE | D3DFVF_TEX1);
        m_pGraphic_Device->SetStreamSource(0, m_PointVB, 0, sizeof(PARTICLE));

        m_PointVB->Lock(
            0 * sizeof(PARTICLE),
            4 * sizeof(PARTICLE),
            (void**)&v,
            m_VBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

        //ATTRIBUTE i = *m_Particles.begin();

        // --- 벡터 계산 ---
        _float3 center = i.vPosition;      // 1. 기준점
        _float3 dir;                      // 2. "앞" 방향 벡터 (정규화됨)
        _float3 vRight;                   // 4. "옆" 방향 벡터 (정규화됨)
        _float3 worldUp = { 0.0f, 0.0f, 1.0f };

        D3DXVec3Normalize(&dir, &i.vDir); // "앞" 방향 정규화

        // "옆" 방향 계산 (dir과 worldUp 외적)
        D3DXVec3Cross(&vRight, &dir, &worldUp);
        D3DXVec3Normalize(&vRight, &vRight);

        // dir이 worldUp과 평행할 경우 예외 처리
        if (D3DXVec3LengthSq(&vRight) < 0.001f)
        {
            _float3 worldRight = { 0.0f, 0.0f, 0.0f };
            D3DXVec3Cross(&vRight, &dir, &worldRight);
            D3DXVec3Normalize(&vRight, &vRight);
        }

        // --- 사각형 크기 정의 ---
        float halfLength = m_fSize * 0.5f; // "앞/뒤" 방향으로의 절반 길이
        float halfWidth = m_fSize * 0.5f;  // "옆" 방향으로의 절반 너비

        // --- 중간 지점 계산 ---
        // "앞쪽 중심점": 기준점에서 "앞" 방향으로 halfLength 만큼 이동
        _float3 P_fwd = center + dir * halfLength;
        // "뒤쪽 중심점": 기준점에서 "뒤" 방향(-dir)으로 halfLength 만큼 이동
        _float3 P_bwd = center - dir * halfLength; // center + (-dir * halfLength) 와 동일

        // --- 최종 정점 위치 계산 (Triangle Strip 순서: TL, TR, BL, BR) ---

        // v[0]: Top-Left (앞쪽 중심점에서 왼쪽으로 halfWidth 이동)
        v[0].vPosition = P_fwd - vRight * halfWidth;
        v[0].vColor = i.vCurrentColor;
        v[0].vTexCoord = { 0.0f, 0.0f };

        // v[1]: Top-Right (앞쪽 중심점에서 오른쪽으로 halfWidth 이동)
        v[1].vPosition = P_fwd + vRight * halfWidth;
        v[1].vColor = i.vCurrentColor;
        v[1].vTexCoord = { 1.0f, 0.0f };

        // v[2]: Bottom-Left (뒤쪽 중심점에서 왼쪽으로 halfWidth 이동)
        v[2].vPosition = P_bwd - vRight * halfWidth;
        v[2].vColor = i.vCurrentColor;
        v[2].vTexCoord = { 0.0f, 1.0f };

        // v[3]: Bottom-Right (뒤쪽 중심점에서 오른쪽으로 halfWidth 이동)
        v[3].vPosition = P_bwd + vRight * halfWidth;
        v[3].vColor = i.vCurrentColor;
        v[3].vTexCoord = { 1.0f, 1.0f };
        m_PointVB->Unlock();

        
        m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    }
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
    