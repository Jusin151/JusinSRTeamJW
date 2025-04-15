#pragma once
#include	"Particle_System.h"
BEGIN(Engine)
class ENGINE_DLL CProjectile_Particle_System final : public CParticle_System
{
public:
	typedef struct tagProjectileParticleDesc : tagParticleSystemDesc
	{
		_uint			iNumParticles;
		_float			fDistance;
	}PROJECTILEDESC;
private:
	CProjectile_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device);
	CProjectile_Particle_System(const CProjectile_Particle_System& Prototype);
	virtual ~CProjectile_Particle_System() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Reset_Particle(ATTRIBUTE* pAttribute, void * pArg = nullptr) override;
	virtual void Update(float fTimeDelta) override;
	void Set_Dir(_float3 vDir) { m_vDir = -vDir; }

public:
	virtual HRESULT Pre_Render() override;
	virtual HRESULT Render() override;
	virtual HRESULT Post_Render() override;

public:
	virtual void Add_Particle(void* pArg);

private:
	_float3		m_vDir = {};
	_float		m_fDistance = {};

public:
	static CProjectile_Particle_System* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CProjectile_Particle_System* Clone(void* pArg) override;
	virtual void Free();
};

END