#include "Fire_Particle_System.h"

CFire_Particle_System::CFire_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CParticle_System{ pGraphic_Device }
{
}

CFire_Particle_System::CFire_Particle_System(const CFire_Particle_System& Prototype)
    : CParticle_System{ Prototype }
{
}

HRESULT CFire_Particle_System::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFire_Particle_System::Initialize(void* pArg)
{
    FIREDESC desc = *reinterpret_cast<FIREDESC*>(pArg);
    m_Bounding_Box = desc.Bounding_Box;
    m_vPos = { 0.f, 0.f, 0.f };
    m_fSize = 1.0f;
    m_VBSize = 2048;
    m_VBOffset = 0;
    m_VBBatchSize = 1;
    m_iMaxParticles = desc.iNumParticles;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    for (_uint i = 0; i < m_iMaxParticles; ++i)
    {
        Add_Particle();
    }
    return S_OK;
}

void CFire_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute)
{
    pAttribute->bIsAlive = true;
    GetRandomVector(&pAttribute->vPosition, &m_Bound.m_vCenter, m_Bound.m_fRadius);
    pAttribute->vVelocity = { GetRandomFloat(-1.f, 1.0f), GetRandomFloat(0.f, 5.0f), GetRandomFloat(-1.f, 1.0f) };
    pAttribute->vAcceleration = { 1.5f, 1.2f, 0.0f };
    pAttribute->fAge = 0;
    pAttribute->fLifetime = 10.0f;
    pAttribute->iIndex = rand() % m_pTexture->Get_NumTextures();
    pAttribute->vCurrentColor = 0xFFFFFFFF;
    pAttribute->fSize = m_fSize;
}

void CFire_Particle_System::Update(float fTimeDelta)
{
    for (auto& i : m_Particles)
    {
        if (i.bIsAlive)
        {
            i.iIndex++;
            i.iIndex %= m_pTexture->Get_NumTextures();
            _float3 worldPos = {};
            D3DXVec3TransformCoord(&worldPos, &i.vPosition, &m_WorldMat);
            if (worldPos.y + abs(m_Bounding_Box.m_vMin.y) > m_Bounding_Box.m_vMin.y * 0.3f)
            {
                i.vPosition += (i.vVelocity * i.vAcceleration.x) * fTimeDelta;
                i.vVelocity.y -= GRAVITY * fTimeDelta;
            }
            i.fAge += fTimeDelta;
        }
        if (i.fAge > i.fLifetime)
            i.bIsAlive = false;
    }
}

HRESULT CFire_Particle_System::Pre_Render()
{
    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, false);
    m_pGraphic_Device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
    m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALEENABLE, true);

    m_pGraphic_Device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(0.0f));

    m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.0f));
    m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.0f));
    m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(1.0f));

    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1); // 정점 색상만 사용
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);    // 텍스쳐 색상 지정

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    return S_OK;
}

HRESULT CFire_Particle_System::Render()
{
    Pre_Render();
    if (!m_Particles.empty())
    {
        m_pGraphic_Device->SetFVF(D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE);
        m_pGraphic_Device->SetStreamSource(0, m_PointVB, 0, sizeof(PARTICLE));

        if (m_VBOffset >= m_VBSize)
            m_VBOffset = 0;

        PARTICLE* v = 0;

        m_PointVB->Lock(
            m_VBOffset * sizeof(PARTICLE),
            m_VBBatchSize * sizeof(PARTICLE),
            (void**)&v,
            m_VBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

        DWORD numParticlesInBatch = 0;
        list<ATTRIBUTE>::iterator i = m_Particles.begin();
        while (i != m_Particles.end())
        {
            if (i->bIsAlive)
            {
                m_pTexture->Bind_Resource(i->iIndex);
                v->vPosition = i->vPosition;
                v->vColor = (D3DCOLOR)i->vCurrentColor;
                v->fSize = i->fSize;
                v++;
                numParticlesInBatch++;

                if (numParticlesInBatch == m_VBBatchSize)
                {
                    m_PointVB->Unlock();

                    m_pGraphic_Device->DrawPrimitive(D3DPT_POINTLIST, m_VBOffset, m_VBBatchSize);

                    m_VBOffset += m_VBBatchSize;

                    if (m_VBOffset >= m_VBSize) m_VBOffset = 0;

                    m_PointVB->Lock(
                        m_VBOffset * sizeof(PARTICLE),
                        m_VBBatchSize * sizeof(PARTICLE),
                        (void**)&v,
                        m_VBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

                    numParticlesInBatch = 0;
                }
            }
            i++;
        }

        m_PointVB->Unlock();

        if (numParticlesInBatch)
        {
            m_pGraphic_Device->DrawPrimitive(D3DPT_POINTLIST, m_VBOffset, numParticlesInBatch);
        }

        m_VBOffset += m_VBBatchSize;
    }

    Post_Render();
    return S_OK;
}

CFire_Particle_System* CFire_Particle_System::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CFire_Particle_System* pInstance = new CFire_Particle_System(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CFire_Particle_System");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CFire_Particle_System* CFire_Particle_System::Clone(void* pArg)
{
    CFire_Particle_System* pInstance = new CFire_Particle_System(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CFire_Particle_System");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFire_Particle_System::Free()
{
    __super::Free();
}
