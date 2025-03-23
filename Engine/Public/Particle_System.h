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
		_wstring strTexturePath;
		_float fSize;
	} PARTICLEDESC;
protected:
	typedef struct tagInstancedData
	{
		_float3 vOffset;
		DWORD	instanceColor;
	}INSTNACEDATA;
	typedef struct tagParticleVertex
	{
		_float3 vPosition;
		DWORD	vColor;
	}PARTICLEVTX;
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
	void Set_Origin(const _float3& vOrigin) { m_vPos = vOrigin; }
	bool IsEmpty();
	bool IsDead();
	void Remove_Dead_Particles();

protected:

	D3DXVECTOR3						m_vPos = {};
	LPDIRECT3DVERTEXBUFFER9			m_PointVB = { nullptr };
	LPDIRECT3DTEXTURE9				m_pTexture = { nullptr };

	//인스턴싱 변수
	LPDIRECT3DVERTEXBUFFER9			m_InstanceVB = { nullptr };
	LPDIRECT3DVERTEXDECLARATION9	m_pVertexDecl = { nullptr };
	LPDIRECT3DVERTEXSHADER9			m_pVertexShader = { nullptr };

	BOUNDINGBOX						m_Bounding_Box = {};
	_float							m_fEmit_Rate = {};
	_float							m_fSize = {};
	_uint							m_iMaxParticles = {};
	list<ATTRIBUTE>					m_Particles = {};
	
	DWORD	m_VBSize;
	DWORD	m_VBOffset;
	DWORD	m_VBBatchSize;
	
public:
	virtual CParticle_System* Clone(void* pArg) override;
	void Free() override;
};

END