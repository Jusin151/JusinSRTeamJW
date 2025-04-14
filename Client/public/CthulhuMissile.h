#pragma once
#include "Projectile_Base.h"
BEGIN(Engine)
class CShader;
class CMaterial;
END

BEGIN(Client)
class CCthulhuMissile final : public CProjectile_Base
{
public:
	typedef struct tagMissileDesc : public PROJ_DESC
	{
		_int iSplitLevel = 0;
		tagMissileDesc(const _float3& _vPos, const _float3& _vDir, _float _fSpeed, _int _iSplitLevel = 0)
			: PROJ_DESC{ _vPos, _vDir, _fSpeed }, iSplitLevel(_iSplitLevel)
		{
		}

		tagMissileDesc() {}

	}Missile_DESC;
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
	void SplitAttack(_float fTimeDelta);
private:
	_uint m_iCurrentFrame = 0;
	_uint m_iMaxFrame = 14;
	_float m_fLifeTime = 5.f;
	_float m_fFrame = 0.f;
	_float m_fAnimationSpeed = 5.f;
	_bool m_bIsHit = false;
	_bool m_bIsDead = false;
	_bool m_bIsColned = false;
	class CPlayer* m_pTarget = nullptr;

	_int     m_iSplitLevel{ 0 };     // 남은 분리 단계
	_float   m_fElapsedTime{ 0.f };   
	_float   m_fSplitDelay{0.5f};    
public:
	static CCthulhuMissile* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();

};
END
