#include "Particle_System.h"

void CParticle_System::Render()
{
	//m_pGraphic_Device->SetStreamSourceFreq();
}

void CParticle_System::Remove_Dead_Particles()
{
}

CParticle_System::CParticle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
{

}

HRESULT CParticle_System::Initialize(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	return E_NOTIMPL;
}

void CParticle_System::Priority_Update(_float fTimeDelta)
{
}

void CParticle_System::Update(_float fTimeDelta)
{
}

void CParticle_System::Late_Update(_float fTimeDelta)
{
}

void CParticle_System::Add_Particle()
{
	ATTRIBUTE attribute = {};
	Reset_Particle(&attribute);
	
	m_Particles.push_back(attribute);

}

void CParticle_System::Clear(_uint iLevelIndex)
{
}

void CParticle_System::Reset()
{
}

void CParticle_System::Reset_Particle(ATTRIBUTE* pAttribute)
{
	
}

CParticle_System* CParticle_System::Create(LPDIRECT3DDEVICE9 pGraphic_Device, void* pArg)
{
	return nullptr;
}

void CParticle_System::Free()
{
	__super::Free();
}