#pragma once
#include "Particle.h"
class CParticle_System : public CBase
{
private:
	CParticle_System(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CParticle_System() = default;
	
	virtual HRESULT Initialize(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual void Add_Particle();
	virtual void Clear(_uint iLevelIndex);
	virtual void Reset();
	virtual void Reset_Particle(ATTRIBUTE* pAttribute);
	virtual void Render();
	void Remove_Dead_Particles();
	
protected:
	_wstring strName;
	LPDIRECT3DDEVICE9	m_pGraphic_Device;
	class CGameInstance* m_pGame_Instance;
	D3DXVECTOR3			m_vOrigin;
	//d3d::BoundingBox;
	_float				m_fEmit_Rate;
	_float				m_fSize;
	class Texture*		m_pTex;
	IDirect3DVertexBuffer9* m_VB;
	_uint				m_iMaxParticles;
	list<ATTRIBUTE> m_Particles;
	
	DWORD	m_VBSize;
	DWORD	m_VBOffset;
	DWORD	m_VBBatchSize;
	
public:
	static CParticle_System* Create(LPDIRECT3DDEVICE9 pGraphic_Device, void* pArg);
	void Free() override;
};

