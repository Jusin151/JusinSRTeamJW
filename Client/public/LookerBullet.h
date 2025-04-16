#pragma once

#include "Projectile_Base.h"


class CLookerBullet : public CProjectile_Base
{
private:
	CLookerBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLookerBullet(const CLookerBullet& Prototype);
	virtual ~CLookerBullet() = default;

public:


	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
	virtual HRESULT On_Collision(CCollisionObject* other)override;


private:

	void Update_Frame(_float fTimeDelta);

	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();

private:
	// 애니메이션 돌리기 위해.. 한 프레임 시간, 현재 프레임 수
	_float m_fElapsedTime = { 0.f };
	_int m_iCurrentFrame = { 0 };

	_float				m_fLifeTime = 0.f;

public:
	static CLookerBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();

};


