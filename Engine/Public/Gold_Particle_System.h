﻿#pragma once
#include "Particle_System.h"
BEGIN(Engine)
class ENGINE_DLL CGold_Particle_System final : public CParticle_System
{
public:
	typedef struct tagGoldParticleDesc : tagParticleSystemDesc
	{
		BOUNDINGBOX		Bounding_Box;
		_uint			iNumParticles;
	}GOLDDESC;
private:
	CGold_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGold_Particle_System(const CGold_Particle_System& Prototype);
	virtual ~CGold_Particle_System() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Reset_Particle(ATTRIBUTE* pAttribute, void * pArg = nullptr) override;
	virtual void Update(float fTimeDelta) override;
	void Set_Dir(_float3 vDir) { m_vDir = -vDir; }
public:
	virtual HRESULT Pre_Render() override;

private:
	_float3		m_vDir = {};
	BOUNDINGBOX						m_Bounding_Box = {};
public:
	static	CGold_Particle_System* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGold_Particle_System* Clone(void* pArg) override;
	virtual void Free();
};
END;
