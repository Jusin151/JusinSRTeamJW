#include "Projectile_Particle_System.h"

CProjectile_Particle_System::CProjectile_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CParticle_System{ pGraphic_Device }
{
	
}

CProjectile_Particle_System::CProjectile_Particle_System(const CParticle_System& Prototype)
	: CParticle_System { Prototype }
{

}

HRESULT CProjectile_Particle_System::Initialize_Prototype()
{
 	return S_OK;
}

HRESULT CProjectile_Particle_System::Initialize(void* pArg)
{
	TRAILDESC desc = *reinterpret_cast<TRAILDESC*>(pArg);
	m_fSize = 0.8f;
	m_VBSize = 2048;
	m_VBOffset = 0;
	m_VBBatchSize = 512;
	m_iMaxParticles = desc.iNumParticles;
	m_fInterval = desc.fIntervaltime;

	PARTICLEDESC pDesc = { m_VBSize, desc.strShaderPath, desc.strTexturePath };

	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;
	return S_OK;
}

void CProjectile_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute)
{
	pAttribute->bIsAlive = true;
	pAttribute->fAge = 0;
	pAttribute->vPosition = m_vPos;
	pAttribute->fLifetime = 2.0f;

	pAttribute->vColor = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
}

void CProjectile_Particle_System::Update(float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;
	if (m_fElapsedTime >= 0.1f)
	{
		Add_Particle();
		m_fElapsedTime = 0.f;
	}
	if (m_Particles.size() > m_iMaxParticles)
	{
		m_Particles.pop_front();
	}
	for (auto& i : m_Particles)
	{
		if (i.bIsAlive)
		{
			//i.vPosition += i.vVelocity * fTimeDelta;
			_float4 temp = DWORDToFloat4_Color(i.vColor);
			temp.w *= 0.99f;
			i.vColor = D3DCOLOR_COLORVALUE(temp.x, temp.y, temp.z, temp.w);
			i.fAge += fTimeDelta;

			if (i.fAge > i.fLifetime)
				i.bIsAlive = false;
		}
	}

	__super::Late_Update(fTimeDelta);
}

CProjectile_Particle_System* CProjectile_Particle_System::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CProjectile_Particle_System* pInstance = new CProjectile_Particle_System(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProjectile_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CProjectile_Particle_System* CProjectile_Particle_System::Clone(void* pArg)
{
	CProjectile_Particle_System* pInstance = new CProjectile_Particle_System(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CProjectile_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Particle_System::Free()
{
	__super::Free();
}
