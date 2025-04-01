#include "Blood_Particle_System.h"

CBlood_Particle_System::CBlood_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CParticle_System{ pGraphic_Device }
{
}

CBlood_Particle_System::CBlood_Particle_System(const CBlood_Particle_System& Prototype)
    : CParticle_System { Prototype }
	, m_Bound { Prototype.m_Bound }
{
}

HRESULT CBlood_Particle_System::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBlood_Particle_System::Initialize(void* pArg)
{
	BLOODDESC desc = *reinterpret_cast<BLOODDESC*>(pArg);
	m_Bound = desc.Bound;
    m_vPos = { 0.f, 0.f, 0.f };
    m_fSize = 0.1f;
    m_VBSize = 2048;
    m_VBOffset = 0;
    m_VBBatchSize = 512;
    m_iMaxParticles = desc.iNumParticles;

    PARTICLEDESC pDesc = { m_VBSize, desc.strShaderPath, desc.strTexturePath };

    if (FAILED(__super::Initialize(&pDesc)))
        return E_FAIL;
    for (_uint i = 0; i < m_iMaxParticles; ++i)
    {
        Add_Particle();
    }
    return S_OK;
}

void CBlood_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute)
{
    pAttribute->bIsAlive = true;
    GetRandomVector(&pAttribute->vPosition, &m_Bound.m_vCenter, m_Bound.m_fRadius);
    pAttribute->vPosition.z = -1.f;
    pAttribute->vVelocity = { GetRandomFloat(-1.f, 1.0f), GetRandomFloat(-1.f, 1.0f), 0.f };
    pAttribute->fAge = 0;
    pAttribute->fLifetime = 2.0f;

	pAttribute->fSize = m_fSize / D3DXVec3Length(&pAttribute->vPosition);
    pAttribute->vColor = 0xFF883932;
}

void CBlood_Particle_System::Update(float fTimeDelta)
{
	for (auto& i : m_Particles)
	{
		if (i.bIsAlive)
		{
			i.vPosition += i.vVelocity * fTimeDelta;
			i.fAge += fTimeDelta;
			i.fSize = m_fSize / D3DXVec3Length(&i.vPosition);
			if (i.fAge > i.fLifetime)
				Reset_Particle(&i);
		}
	}
}

CBlood_Particle_System* CBlood_Particle_System::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBlood_Particle_System* pInstance = new CBlood_Particle_System(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBlood_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBlood_Particle_System* CBlood_Particle_System::Clone(void* pArg)
{
	CBlood_Particle_System* pInstance = new CBlood_Particle_System(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBlood_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlood_Particle_System::Free()
{
	__super::Free();
}
