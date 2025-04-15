#pragma once
#include "Particle_System.h"
BEGIN(Engine)
class ENGINE_DLL CFire_Particle_System final : public CParticle_System
{
public:
	typedef struct tagFireParticleDesc : tagParticleSystemDesc
	{
		BOUNDINGBOX		Bounding_Box;
		BOUNDINGSPHERE	Bound;
		_uint			iNumParticles;
	}FIREDESC;
private:
	CFire_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFire_Particle_System(const CFire_Particle_System& Prototype);
	virtual ~CFire_Particle_System() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Reset_Particle(ATTRIBUTE* pAttribute, void * pArg = nullptr) override;
	virtual void Update(float fTimeDelta) override;
public:
	virtual HRESULT Pre_Render() override;
	virtual HRESULT Render() override;
	void Set_WorldMat(const _float4x4& WorldMat) { m_WorldMat = WorldMat; }
private:
	BOUNDINGBOX						m_Bounding_Box = {};
	BOUNDINGSPHERE					m_Bound = {};
	_float4x4						m_WorldMat = {};
	_uint							m_iCurrentFrame = {};
	_uint							m_iLastFrame = {};
public:
	static	CFire_Particle_System* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CFire_Particle_System* Clone(void* pArg) override;
	virtual void Free();
};
END;
