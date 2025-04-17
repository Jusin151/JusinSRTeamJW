#pragma once

#include "Projectile_Base.h"


class CAttackMelee : public CProjectile_Base
{
public:
	typedef struct meleeDesc: public tagProjectileDesc {
		_float fLifeTIme;
		_int   iDamage;
		_float3 vScale;

	}MELEE_DESC;
private:
	CAttackMelee(LPDIRECT3DDEVICE9 pGraphic_Device);
	CAttackMelee(const CAttackMelee& Prototype);
	virtual ~CAttackMelee() = default;

public:


	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
	virtual HRESULT On_Collision(CCollisionObject* other)override;


private:

	HRESULT Ready_Components();

private:

	_float m_fLifeTime = { 0.f };

public:
	static CAttackMelee* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();

};


