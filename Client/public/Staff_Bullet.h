#pragma once

#include "Bullet_Base.h"
#include "Weapon_Headers.h"

BEGIN(Engine)
class CLight;
class CShader;
class CMaterial;
class CParticle_System;
END

BEGIN(Client)

class CStaff_Bullet : public CBullet_Base
{
public:
	typedef struct Bullet_INFO
	{
		_float3 fScale{};

	}Bullet_DESC;

private:
	CStaff_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CStaff_Bullet(const CStaff_Bullet& Prototype);
	virtual ~CStaff_Bullet() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	// 마지막에 플레이어 방향으로 바라보도록 함
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

public:
	virtual HRESULT On_Collision(CCollisionObject* other)override;
	void Attack_Melee();

	virtual void Reset() override;
private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();
public:
	void Set_Bullet_Scale(_float3 type)
	{
		Staff_Scale =  type;
	}
private:
	_float3				m_vDir = {};
	
	_float				m_fLifeTime = 0.f;
	_float3 Staff_Scale{};
private:
	CLight*				m_pLightCom					= { nullptr };
	CShader*			m_pShaderCom				= { nullptr };
	CMaterial*			m_pMaterialCom				= { nullptr };
	CTransform*			m_pParticleTransformCom		= { nullptr };
	CParticle_System*	m_pParticleCom				= { nullptr };
public:
	static CStaff_Bullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();
private:

};
END
