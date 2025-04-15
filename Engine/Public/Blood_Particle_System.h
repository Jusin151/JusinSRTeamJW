#pragma once
#include "Particle_System.h"
BEGIN(Engine)
class ENGINE_DLL CBlood_Particle_System final : public CParticle_System
{
public:
	typedef struct tagBloodParticleDesc : tagParticleSystemDesc
	{
		BOUNDINGSPHERE		Bound;
		_uint				iNumParticles;
	}BLOODDESC;
private:
	CBlood_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBlood_Particle_System(const CBlood_Particle_System& Prototype);
	virtual ~CBlood_Particle_System() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Reset_Particle(ATTRIBUTE* pAttribute, void* pArg = nullptr) override;
	virtual void Update(float fTimeDelta) override;

public:
	virtual HRESULT Pre_Render() override;

private:
	BOUNDINGSPHERE		m_Bound = {};
public:
	static	CBlood_Particle_System* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CBlood_Particle_System* Clone(void* pArg) override;
	virtual void Free();
};
END;

