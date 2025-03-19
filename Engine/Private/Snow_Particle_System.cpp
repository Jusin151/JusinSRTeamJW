#include "Snow_Particle_System.h"

CSnow_Particle_System::CSnow_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CParticle_System{ pGraphic_Device }
{
	
}

CSnow_Particle_System::CSnow_Particle_System(const CParticle_System& Prototype)
	: CParticle_System { Prototype }
{

}

HRESULT CSnow_Particle_System::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSnow_Particle_System::Initialize(void* pArg)
{
	SNOWDESC desc = *reinterpret_cast<SNOWDESC*>(pArg);
	m_Bounding_Box = desc.pBounding_Box;
	m_fSize = 0.8f;
	m_VBSize = 2048;
	m_VBOffset = 0;
	m_VBBatchSize = 512;

	for (_uint i = 0; i < desc.iNumParticles; ++i)
	{
		Add_Particle();
	}
	return E_NOTIMPL;
}

void CSnow_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute)
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

void CSnow_Particle_System::Update(float fTimeDelta)
{
	for (auto& i : m_Particles)
	{
		i.vPosition += i.vVelocity * fTimeDelta;
		if (!m_Bounding_Box.Is_Point_Inside(i.vPosition))
		{
			Reset_Particle(&i);
		}
	}
}

void CSnow_Particle_System::Free()
{
	__super::Free();
}
