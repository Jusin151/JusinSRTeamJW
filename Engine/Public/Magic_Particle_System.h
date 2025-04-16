#pragma once
#include "Particle_System.h"
BEGIN(Engine)
class ENGINE_DLL CMagic_Particle_System final : public CParticle_System
{
public:
	typedef struct tagMagicParticleDesc : tagParticleSystemDesc
	{
		BOUNDINGSPHERE		Bound;
		_float3				vOrigin;
		_uint				iNumParticles;
		DWORD				dwInitialColor;
		DWORD				dwFadeColor;
	}MAGICDESC;
private:
	CMagic_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMagic_Particle_System(const CMagic_Particle_System& Prototype);
	virtual ~CMagic_Particle_System() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Reset_Particle(ATTRIBUTE* pAttribute, void * pArg = nullptr) override;
	virtual void Update(float fTimeDelta) override;

public:
	virtual HRESULT Pre_Render() override;

private:
	BOUNDINGSPHERE		m_Bound = {};
	DWORD				m_dwInitialColor;
	DWORD				m_dwFadeColor;
public:
	static	CMagic_Particle_System* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CMagic_Particle_System* Clone(void* pArg) override;
	virtual void Free();
};
END;

