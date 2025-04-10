#pragma once

#include "Bullet_Base.h"
#include "Weapon_Headers.h"

BEGIN(Engine)
class CParticle_System;
END

BEGIN(Client)

class CHellBoss_DarkBullet : public CBullet_Base
{
public:
	typedef struct Bullet_INFO
	{
		_float3 fScale{};

	}Bullet_DESC;

private:
	CHellBoss_DarkBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHellBoss_DarkBullet(const CHellBoss_DarkBullet& Prototype);
	virtual ~CHellBoss_DarkBullet() = default;

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
	void Set_DarkHoleTransform(CTransform* pTransform)
	{
		if (pTransform)
			m_pDarkHole_Transform = pTransform;
	}

private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();
public:
	void Set_Bullet_Scale(_float3 type){	Staff_Scale =  type;}

private:
	_float3				m_vDir = {};
	CParticle_System*	m_pParticleCom = { nullptr };
	CTransform*			m_pParticleTransformCom = { nullptr };
	CLight*				m_pLightCom = { nullptr };
	_float				m_fLifeTime = 0.f;
	_float3 Staff_Scale{};
	CTransform* m_pDarkHole_Transform = { nullptr };
	_float3 DarkHole_RIght{};
	_float3 DarkHole_Up{};
	_float3 DarkHole_Look{};
	_float3 DarkHole_Pos{};
public:
	static CHellBoss_DarkBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();
protected:
	void* m_pInitArg = { nullptr };

public:
	void Set_InitArg(void* pArg) { m_pInitArg = pArg; }


};
END
