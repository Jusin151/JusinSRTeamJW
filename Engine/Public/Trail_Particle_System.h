#pragma once
#include	"Particle_System.h"
BEGIN(Engine)
class ENGINE_DLL CTrail_Particle_System final : public CParticle_System
{
public:
	typedef struct tagTrailParticleDesc : tagParticleSystemDesc
	{
		_uint			iNumParticles;
		_float			fWidth;
		_float			fDistance;
	}TRAILDESC;
private:
	CTrail_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTrail_Particle_System(const CTrail_Particle_System& Prototype);
	virtual ~CTrail_Particle_System() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Reset_Particle(ATTRIBUTE* pAttribute) override;
	virtual void Update(float fTimeDelta) override;
	void Set_Dir(_float3 vDir) { m_vDir = -vDir; }

public:
	virtual HRESULT Render() override;

private:
	_float3		m_vDir = {};
	_float		m_fWidth = {};
	_float		m_fDistance = {};

public:
	static CTrail_Particle_System* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CTrail_Particle_System* Clone(void* pArg) override;
	virtual void Free();
};

END