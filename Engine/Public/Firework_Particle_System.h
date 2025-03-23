#pragma once
#include	"Particle_System.h"
BEGIN(Engine)
class ENGINE_DLL CFirework_Particle_System final : public CParticle_System
{
public:
	typedef struct tagFireWorkParticleDesc : tagParticleSystemDesc
	{
		BOUNDINGBOX		Bounding_Box;
		_float3			vOrigin;
		_uint			iNumParticles;
	}FIREWORKDESC;
private:
	CFirework_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFirework_Particle_System(const CParticle_System& Prototype);
	virtual ~CFirework_Particle_System() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Update(float fTimeDelta) override;

public:
	virtual HRESULT Pre_Render() override;
	virtual HRESULT Post_Render() override;
	virtual void Reset_Particle(ATTRIBUTE* pAttribute) override;
	

public:
	static CFirework_Particle_System* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CFirework_Particle_System* Clone(void* pArg) override;
	virtual void Free();
};

END