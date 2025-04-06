#pragma once

#include "Bullet_Base.h"
#include "Weapon_Headers.h"

BEGIN(Engine)
class CParticle_System;
END

BEGIN(Client)

class CHellBoss_Bullet : public CBullet_Base
{
	enum BULLET_MODE { ROTATING, LAUNCHING };
	BULLET_MODE m_eBulletMode = ROTATING; 
public:
	struct PowerBlastDesc
	{
		_bool bIsPowerBlast = { false }; // << 이거 추가
		wstring wBulletType;
		_int     iIndex;
		_int     iTotalCount;
	};


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
public:
	const wstring& Get_BulletType() const { return m_wBulletType; }
public:
	void Launch_Toward_Player();
	void Set_BulletIndex(_int iIndex) { m_iBulletIndex = iIndex; }
private:
	_bool m_bJustSpawned = { true };
	_bool m_bInitializedPos = {false}; 
	wstring m_wBulletType = {};
	wstring m_wBullet_Texture = {};
	_float3 m_fBullet_Scale{};


	_float m_fFrameDuration = {}; // 이미지 간 시간 간격
	_int   m_iMaxFrame = {};          // 애니메이션 마지막 프레임 
	_int   m_iFrameCount = {};        // 총 이미지 수


	_float m_fRotateAngle = 0.f;
	_float m_fRadius = 3.0f;
	_int m_iBulletIndex = 0;
private:
	_float m_fFixedAngle = 0.f; 
	PowerBlastDesc pDesc{};
	_float m_fCurScale = 2.f; // Power_Blast 초기 스케일

};
END
