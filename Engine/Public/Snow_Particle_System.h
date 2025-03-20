#pragma once
#include	"Particle_System.h"
BEGIN(Engine)
class ENGINE_DLL CSnow_Particle_System final : public CParticle_System
{
	typedef struct tagSnowParticleDesc
	{
		CBounding_Box pBounding_Box;
		_uint		  iNumParticles;
	}SNOWDESC;
private:
	CSnow_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSnow_Particle_System(const CParticle_System& Prototype);
	virtual ~CSnow_Particle_System() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Reset_Particle(ATTRIBUTE* pAttribute) override;
	virtual void Update(float fTimeDelta) override;

public:
	static CSnow_Particle_System* Clone(void* pArg);
	virtual void Free();
};

END