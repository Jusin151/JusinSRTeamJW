#pragma once
#include "Particle_System.h"
class CParticle_Manager : public CBase
{
private:
	CParticle_Manager(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CParticle_Manager() = default;
public:
	HRESULT Add_Particle_System(CParticle_System* pParticle_System);
	HRESULT Remove_Dead_Particle_System();
	void Update(float fTimeDelta);
	void Render();
private:
	LPDIRECT3DDEVICE9			m_pGraphic_Device;
	list<CParticle_System*>		m_Particle_Systems;
public:
	static CParticle_Manager* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
	
};

