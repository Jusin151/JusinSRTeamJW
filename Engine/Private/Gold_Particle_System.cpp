#include "Gold_Particle_System.h"

CGold_Particle_System::CGold_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CParticle_System { pGraphic_Device }
{
}

CGold_Particle_System::CGold_Particle_System(const CGold_Particle_System& Prototype)
    : CParticle_System{ Prototype }
    , m_vDir { Prototype.m_vDir }
{
}

HRESULT CGold_Particle_System::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGold_Particle_System::Initialize(void* pArg)
{
    GOLDDESC desc = *reinterpret_cast<GOLDDESC*>(pArg);
    m_Bounding_Box = desc.Bounding_Box;
    m_vPos = { 0.f, 0.f, 0.f };
    m_fSize = 0.1f;
    m_VBSize = 2048;
    m_VBOffset = 0;
    m_VBBatchSize = 512;
    m_iMaxParticles = desc.iNumParticles;
    m_vDir = { 0.0f, 1.0f, 0.0f };

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    for (_uint i = 0; i < m_iMaxParticles; ++i)
    {
        Add_Particle();
    }
    return S_OK;
}

void CGold_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute)
{
    pAttribute->bIsAlive = true;
    GetRandomVector(&pAttribute->vPosition, &m_Bounding_Box.m_vMin, &m_Bounding_Box.m_vMax);
    pAttribute->vVelocity = { 0.f, GetRandomFloat(0.f, 1.0f), 0.f};
    pAttribute->fAge = 0;
    pAttribute->fLifetime = 2.0f;

    pAttribute->fSize = m_fSize;
    pAttribute->iIndex = 0; //rand() % m_pTexture->Get_NumTextures();
    
    pAttribute->vInitialColor = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
    pAttribute->vCurrentColor = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
    pAttribute->vColorFade = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 0.0f);
}

void CGold_Particle_System::Update(float fTimeDelta)
{
    for (auto& i : m_Particles)
    {
        i.vPosition += i.vVelocity * fTimeDelta;
        i.fAge += fTimeDelta;
        
        // --- 색상 페이드 아웃 및 수명 관리 (기존과 동일) ---
        float ratio = i.fAge / i.fLifetime;
        i.vCurrentColor = ColorLerp(i.vInitialColor, i.vColorFade, ratio);
        if (!m_Bounding_Box.Is_Point_Inside(i.vPosition))
        {
            Reset_Particle(&i);
        }
        
        /*if (i.fAge > i.fLifetime)
            i.bIsAlive = false;*/
    }
}

HRESULT CGold_Particle_System::Pre_Render()
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

CGold_Particle_System* CGold_Particle_System::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CGold_Particle_System* pInstance = new CGold_Particle_System(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CGold_Particle_System");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGold_Particle_System* CGold_Particle_System::Clone(void* pArg)
{
    CGold_Particle_System* pInstance = new CGold_Particle_System(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CGold_Particle_System");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGold_Particle_System::Free()
{
    __super::Free();
}
