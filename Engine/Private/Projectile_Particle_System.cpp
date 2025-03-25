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

	PARTICLEDESC pDesc = { m_VBSize, desc.strShaderPath, desc.strTexturePath };

	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;
	
	for (_uint i = 0; i < desc.iNumParticles; ++i)
	{
		Add_Particle();
	}
	return S_OK;
}

void CProjectile_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute)
{
	pAttribute->bIsAlive = true;

	GetRandomVector(&pAttribute->vPosition, &m_Bounding_Box.m_vMin, &m_Bounding_Box.m_vMax);

	pAttribute->vPosition.y = m_Bounding_Box.m_vMax.y;

	pAttribute->vVelocity = {
		GetRandomFloat(0.0f, 1.0f) * -3.0f,
		GetRandomFloat(0.0f, 1.0f) * -10.0f,
		0.0f
	};
	 pAttribute->vColor = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
}

void CProjectile_Particle_System::Update(float fTimeDelta)
{
	for (auto& i : m_Particles)
	{
		i.vPosition += i.vVelocity * fTimeDelta;
		//D3DXVec3CatmullRom();
		_float4 temp = DWORDToFloat4_Color(i.vColor);
		i.vColor = D3DCOLOR_COLORVALUE(temp.x, temp.y, temp.z, temp.w * 0.9f);
		i.fAge += fTimeDelta;
		/*if (!m_Bounding_Box.Is_Point_Inside(i.vPosition))
		{
			Reset_Particle(&i);
		}*/
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
