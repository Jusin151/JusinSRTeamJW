#pragma once
#include "Projectile_Base.h"

BEGIN(Client)
class CCthulhuMissile final : public CProjectile_Base
{
private:
	CCthulhuMissile(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCthulhuMissile(const CCthulhuMissile& Prototype);
	virtual ~CCthulhuMissile() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
	virtual HRESULT On_Collision(CCollisionObject* other)override;
	virtual void Reset() override;

private:

	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();
	void Billboarding(_float fTimeDelta);
	void Update_Animation(_float fTimeDelta);
	void Move(_float fTimeDelta);
private:
	_uint m_iCurrentFrame = 0;
	_uint m_iMaxFrame = 14;
	_float m_fDistance = 0.f;
	_float m_fMaxDistance = 0.f;
	_float m_fFrame = 0.f;
	_float m_fAnimationSpeed = 5.f;
	_bool m_bIsHit = false;
	_bool m_bIsDead = false;
	_bool m_bIsColned = false;
	class CPlayer* m_pTarget = nullptr;
public:
	static CCthulhuMissile* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();

};
END
