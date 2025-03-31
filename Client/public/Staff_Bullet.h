#pragma once

#include "Bullet_Base.h"
#include "Weapon_Headers.h"

BEGIN(Engine)
class CParticle_System;
END

BEGIN(Client)

class CStaff_Bullet : public CBullet_Base
{


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
private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();
private:

	CParticle_System* m_pParticleCom = { nullptr };

public:
	static CStaff_Bullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();
private:

};
END
