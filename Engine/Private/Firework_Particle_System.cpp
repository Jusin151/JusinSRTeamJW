#include "Firework_Particle_System.h"

CFirework_Particle_System::CFirework_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CParticle_System{ pGraphic_Device }
{
	
}

CFirework_Particle_System::CFirework_Particle_System(const CParticle_System& Prototype)
	: CParticle_System { Prototype }
{

}

HRESULT CFirework_Particle_System::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFirework_Particle_System::Initialize(void* pArg)
{
	FIREWORKDESC desc;
	if (nullptr == pArg)
	{
		FIREWORKDESC desc = *reinterpret_cast<FIREWORKDESC*>(pArg);
		m_Bounding_Box = desc.Bounding_Box;
		m_vPos = desc.vOrigin;
		m_fSize = 0.8f;
		m_VBSize = 2048;
		m_VBOffset = 0;
		m_VBBatchSize = 512;
	}
	desc = *reinterpret_cast<FIREWORKDESC*>(pArg);
	m_Bounding_Box = desc.Bounding_Box;
	m_vPos = desc.vOrigin;
	m_fSize = 0.8f;
	m_VBSize = 2048;
	m_VBOffset = 0;
	m_VBBatchSize = 512;

	PARTICLEDESC pDesc = { m_VBSize };

	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;
	
	for (_uint i = 0; i < desc.iNumParticles; ++i)
	{
		Add_Particle();
	}
	return S_OK;
}

void CFirework_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute)
{
	pAttribute->bIsAlive = true;
	pAttribute->vPosition = m_vPos;

	GetRandomVector(&pAttribute->vVelocity, &m_Bounding_Box.m_vMin, &m_Bounding_Box.m_vMax);

	D3DXVec3Normalize(&pAttribute->vVelocity, &pAttribute->vVelocity);

	pAttribute->vColor = D3DXCOLOR(
		GetRandomFloat(0.0f, 1.0f),
		GetRandomFloat(0.0f, 1.0f),
		GetRandomFloat(0.0f, 1.0f),
		1.0f);

	pAttribute->fAge = 0.0f;
	pAttribute->fLifetime = 100.0f; // lives for 2 seconds
}

void CFirework_Particle_System::Update(float fTimeDelta)
{
	for (auto& i : m_Particles)
	{
		if (i.bIsAlive)
		{
			i.vPosition += i.vVelocity * fTimeDelta;
			i.fAge += fTimeDelta;

			if (i.fAge > i.fLifetime)
				i.bIsAlive = false;
		}
	}

	__super::Late_Update(fTimeDelta);
}

HRESULT CFirework_Particle_System::Pre_Render()
{
	__super::Pre_Render();

	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// read, but don't write particles to z-buffer
	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, false);

	return S_OK;
}

HRESULT CFirework_Particle_System::Post_Render()
{
	__super::Post_Render();

	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, true);
	return S_OK;
}

CFirework_Particle_System* CFirework_Particle_System::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFirework_Particle_System* pInstance = new CFirework_Particle_System(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFirework_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CFirework_Particle_System* CFirework_Particle_System::Clone(void* pArg)
{
	CFirework_Particle_System* pInstance = new CFirework_Particle_System(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CFirework_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFirework_Particle_System::Free()
{
	__super::Free();
}
