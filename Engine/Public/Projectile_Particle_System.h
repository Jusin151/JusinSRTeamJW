#pragma once
#include	"Particle_System.h"
BEGIN(Engine)
class ENGINE_DLL CProjectile_Particle_System final : public CParticle_System
{
public:
	typedef struct tagTrailParticleDesc : tagParticleSystemDesc
	{
		_uint			iNumParticles;
		_float			fIntervaltime;
	}TRAILDESC;
private:
	CProjectile_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device);
	CProjectile_Particle_System(const CParticle_System& Prototype);
	virtual ~CProjectile_Particle_System() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Reset_Particle(ATTRIBUTE* pAttribute) override;
	virtual void Update(float fTimeDelta) override;
	void Set_Position(_float3 pos) { m_vPos = pos; }

private:
	_float		m_fInterval = {};
	_float		m_fElapsedTime = {};

public:
	static CProjectile_Particle_System* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CProjectile_Particle_System* Clone(void* pArg) override;
	virtual void Free();
};

END