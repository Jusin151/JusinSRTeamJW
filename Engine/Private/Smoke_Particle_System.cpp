#include "Smoke_Particle_System.h"

CSmoke_Particle_System::CSmoke_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CParticle_System{ pGraphic_Device }
{
}

CSmoke_Particle_System::CSmoke_Particle_System(const CSmoke_Particle_System& Prototype)
    : CParticle_System{ Prototype }
{
}

HRESULT CSmoke_Particle_System::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSmoke_Particle_System::Initialize(void* pArg)
{
    SMOKEDESC desc = *reinterpret_cast<SMOKEDESC*>(pArg);
    m_Bounding_Box = desc.Bounding_Box;
    m_vPos = { 0.f, 0.f, 0.f };
    m_fSize = desc.fSize;
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

void CSmoke_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute, void* pArg)
{
    pAttribute->bIsAlive = true;
    GetRandomVector(&pAttribute->vPosition, &m_Bound.m_vCenter, m_Bound.m_fRadius);
    pAttribute->vVelocity = { GetRandomFloat(-1.f, 1.0f), GetRandomFloat(1.f, 5.0f), GetRandomFloat(-1.f, 1.0f) };
    pAttribute->vAcceleration = { 0.8, 0.8, 0.8f };
    pAttribute->fAge = 0;
    pAttribute->fLifetime = 10.0f;
    pAttribute->iIndex = 0;
    pAttribute->vCurrentColor = 0xFFFFFFFF;
    pAttribute->fSize = m_fSize;
}

void CSmoke_Particle_System::Update(float fTimeDelta)
{
    const _uint numTextures = m_pTexture->Get_NumTextures();
    if (numTextures <= 0) {
        return;
    }

    // 거리 기반 애니메이션을 위한 최대 거리 (이전과 동일)
    const float maxAnimDistance = 15.0f; // 예시 값
    if (maxAnimDistance <= 0.0f) {
        return;
    }

    for (auto& i : m_Particles)
    {
        if (i.bIsAlive)
        {
            // --- 1. 시간 비율 계산 (0.0 ~ 1.0) ---
            float ageRatio = 0.0f;
            if (i.fLifetime > 0.0f) {
                ageRatio = i.fAge / i.fLifetime;
                ageRatio = std::max(0.0f, std::min(ageRatio, 1.0f));
            }

            // --- 2. 거리 비율 계산 (0.0 ~ 1.0) ---
            _float3 pos = i.vPosition;
            float currentDistance = pos.Length();
            float distanceRatio = currentDistance / maxAnimDistance;
            distanceRatio = max(0.0f, std::min(distanceRatio, 1.0f));

            // --- 3. 두 비율 조합 (최댓값 사용) ---
            // 시간과 거리 중 더 많이 진행된 쪽의 비율을 최종 비율로 사용
            float finalRatio = max(ageRatio, distanceRatio);

            /* --- 다른 조합 방식 예시 (주석 처리됨) ---
            // 가중 평균 (예: 시간 60%, 거리 40%)
            // float timeWeight = 0.6f;
            // float distanceWeight = 0.4f;
            // float finalRatio = timeWeight * ageRatio + distanceWeight * distanceRatio;
            // finalRatio = std::max(0.0f, std::min(finalRatio, 1.0f)); // 결과 클램핑

            // 곱셈
            // float finalRatio = ageRatio * distanceRatio;
            */

            // --- 4. 최종 비율을 프레임 인덱스로 매핑 ---
            float targetFrame = finalRatio * static_cast<float>(numTextures);
            i.iIndex = static_cast<int>(targetFrame);
            i.iIndex = min(i.iIndex, numTextures - 1); // 인덱스 범위 제한


            // --- 위치 및 나이 업데이트 ---
            i.vPosition += (i.vVelocity * i.vAcceleration.x) * fTimeDelta;
            i.fAge += fTimeDelta;
        }

        // --- 수명 체크 ---
        if (i.fAge > i.fLifetime) {
            i.bIsAlive = false;
        }
    }
}

HRESULT CSmoke_Particle_System::Pre_Render()
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

HRESULT CSmoke_Particle_System::Render()
{
    Pre_Render();
    if (!m_Particles.empty())
    {
        m_pTexture->Bind_Resource(rand() % m_pTexture->Get_NumTextures());
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

CSmoke_Particle_System* CSmoke_Particle_System::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CSmoke_Particle_System* pInstance = new CSmoke_Particle_System(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSmoke_Particle_System");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CSmoke_Particle_System* CSmoke_Particle_System::Clone(void* pArg)
{
    CSmoke_Particle_System* pInstance = new CSmoke_Particle_System(*this); 

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CSmoke_Particle_System");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSmoke_Particle_System::Free()
{
    __super::Free();
}
