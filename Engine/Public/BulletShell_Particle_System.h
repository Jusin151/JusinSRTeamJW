#pragma once
#include "Particle_System.h"
BEGIN(Engine)
class CTransform;
class ENGINE_DLL CBulletShell_Particle_System final : public CParticle_System
{
public:
	typedef struct tagBulletShellParticleDesc : tagParticleSystemDesc
	{
		BOUNDINGBOX		Bounding_Box;
		BOUNDINGSPHERE	Bound;
		_uint			iNumParticles;
	}BULLETSHELLDESC;
private:
	CBulletShell_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBulletShell_Particle_System(const CBulletShell_Particle_System& Prototype);
	virtual ~CBulletShell_Particle_System() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Add_Particle(BOUNDINGBOX bound);

public:
	virtual void Reset_Particle(ATTRIBUTE* pAttribute) override;
	virtual void Update(float fTimeDelta) override;
public:
	virtual HRESULT Render() override;
	void Set_WorldMat(const _float4x4& WorldMat) { m_WorldMat = WorldMat; }

private:
	BOUNDINGBOX						m_Bounding_Box = {};
	BOUNDINGSPHERE					m_Bound = {};
	_float4x4						m_WorldMat = {};
public:
	static	CBulletShell_Particle_System* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CBulletShell_Particle_System* Clone(void* pArg) override;
	virtual void Free();
};
END;
