#pragma once
#include "Component.h"
#include "Bounding_Box.h"
#include "Bounding_Sphere.h"

BEGIN(Engine)

class ENGINE_DLL CParticle_System abstract : public CComponent
{
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
	class CTexture*				m_pTextureCom = { nullptr };
protected:
	D3DXVECTOR3				m_vPos = {};
	_wstring strName = {};
	IDirect3DVertexBuffer9* m_VB = { nullptr };
	CBounding_Box			m_Bounding_Box = {};
	_float					m_fEmit_Rate = {};
	_float					m_fSize = {};
	_uint					m_iMaxParticles = {};
	list<ATTRIBUTE>			m_Particles = {};
	
	DWORD	m_VBSize;
	DWORD	m_VBOffset;
	DWORD	m_VBBatchSize;
	
public:
	static CParticle_System* Clone(void* pArg);
	void Free() override;
};

END