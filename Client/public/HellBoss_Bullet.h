#pragma once

#include "Bullet_Base.h"
#include "Weapon_Headers.h"

BEGIN(Engine)
class CParticle_System;
END

BEGIN(Client)

class CHellBoss_Bullet : public CBullet_Base
{


private:
	CHellBoss_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHellBoss_Bullet(const CHellBoss_Bullet& Prototype);
	virtual ~CHellBoss_Bullet() = default;

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
private:
	_float3				m_vDir = {};
	CParticle_System*	m_pParticleCom = { nullptr };
	CTransform*			m_pParticleTransformCom = { nullptr };
	CLight*				m_pLightCom = { nullptr };
	_float				m_fLifeTime = 0.f;
private:
	CTransform* m_HellBoss_Transform = { nullptr };
	_float3 m_fHellBoss_RIght{};
	_float3 m_fHellBoss_Up{};
	_float3 m_fHellBoss_Look{};
	_float3 m_fHellBoss_Pos{};
public:
	void Set_Direction(const _float3& vDir) { m_vDir = vDir; }
	CTransform* Get_Transform() const { return m_pTransformCom; }
public:
	static CHellBoss_Bullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();
private:

};
END
