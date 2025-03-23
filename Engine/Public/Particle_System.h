#pragma once
#include "Component.h"
#include "BoundingBox.h"
#include "Bounding_Sphere.h"

BEGIN(Engine)

class ENGINE_DLL CParticle_System abstract : public CComponent
{
public:
	typedef struct tagParticleSystemDesc
	{
		DWORD	VBSize;
		_wstring strShaderPath;
	} PARTICLEDESC;
protected:
	CParticle_System(LPDIRECT3DDEVICE9 pGraphic_Device);
	CParticle_System(const CParticle_System& Prototype);
	virtual ~CParticle_System() = default;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

public:
	virtual HRESULT Pre_Render();
	virtual HRESULT Render();
	virtual HRESULT Post_Render();

public:
	virtual void Add_Particle();
	virtual void Reset();
	virtual void Reset_Particle(ATTRIBUTE* pAttribute);
	
public:
	bool IsEmpty();
	bool IsDead();
	void Remove_Dead_Particles();

protected:
	_wstring				strName = {};
	D3DXVECTOR3				m_vPos = {};
	LPDIRECT3DVERTEXBUFFER9 m_VB = { nullptr };
	BOUNDINGBOX				m_Bounding_Box = {};
	_float					m_fEmit_Rate = {};
	_float					m_fSize = {};
	_uint					m_iMaxParticles = {};
	list<ATTRIBUTE>			m_Particles = {};
	
	DWORD	m_VBSize;
	DWORD	m_VBOffset;
	DWORD	m_VBBatchSize;
	
public:
	virtual CParticle_System* Clone(void* pArg) override;
	void Free() override;
};

END