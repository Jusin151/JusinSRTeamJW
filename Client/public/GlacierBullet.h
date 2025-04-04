#pragma once

#include "Projectile_Base.h"


class CGlacierBullet : public CProjectile_Base
{
private:
	CGlacierBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGlacierBullet(const CGlacierBullet& Prototype);
	virtual ~CGlacierBullet() = default;

public:


	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
	virtual HRESULT On_Collision(CCollisionObject* other)override;


private:

	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();

private:
	
public:
	static CGlacierBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();

};


