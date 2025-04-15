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
	enum EXPAND_PHASE { EXPAND_SPREADING, EXPAND_LAUNCH };
	EXPAND_PHASE m_eExpandPhase = EXPAND_SPREADING;
public:
	struct PowerBlastDesc
	{
		 wstring wBulletType;
		_bool isLeft = false;
		_int iIndex = 0;
		_int iTotalCount = 0;
		_float3 vAxis = { 0.f, 1.f, 0.f };
		_bool bIsPowerBlast = false;
		_int iPatternType = 0;
	};



private:
	CHellBoss_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHellBoss_Bullet(const CHellBoss_Bullet& Prototype);
	virtual ~CHellBoss_Bullet() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
private:
	HRESULT Ready_Components();
public:
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
public:
	virtual HRESULT Render()override;
private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
public:
	virtual HRESULT On_Collision(CCollisionObject* other)override;
	void Attack_Melee();
	virtual void Reset() override;
	
private:
	_float3				m_vDir = {};
	//CParticle_System*
	CParticle_System*	m_pParticleCom = { nullptr };
	CTransform*			m_pParticleTransformCom = { nullptr };
	CLight*				m_pLightCom = { nullptr };
	_float				m_fLifeTime = 0.f;
private: // 헬보스 관련 정보
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
	_float3 Get_RandomBackOffsetPos();
public:
	const wstring& Get_BulletType() const { return m_wBulletType; }
	_bool m_bShakeTriggered = false;

public:
	void Launch_Toward_Player();
	void Set_BulletIndex(_int iIndex) { m_iBulletIndex = iIndex; }
	void Rotated_BulletPos()
	{
		m_bRotated_Bullet = !m_bRotated_Bullet;
	}
private:
	_bool m_bJustSpawned = { true };
	_bool m_bInitializedPos = {false}; 
	wstring m_wBulletType = {};
	wstring m_wBullet_Texture = {};
	_float3 m_fBullet_Scale{};
	_float m_fRotateTime = 0.f;

	_float m_fFrameDuration = {}; // 이미지 간 시간 간격
	_int   m_iMaxFrame = {};          // 애니메이션 마지막 프레임 
	_int   m_iFrameCount = {};        // 총 이미지 수
	_float m_fRotateAngle = 0.f;
	_float m_fRadius = { 3.0f };
	_int m_iBulletIndex = 0;

private:
	_float m_fElapsedTimeSmoke = {};
	_float m_fDelayTimeSmoke = { 0.02f };
private:
	_float m_fFixedAngle = 0.f; 
	PowerBlastDesc pDesc{};
	_float m_fCurScale = 2.f; 
	_bool m_bReadyToLaunch = false; 
	_float m_fLaunchTimer = 0.f; 
	_float3 m_vAxis{};
	_bool m_bPlayedOnce = false;
	_float3 m_vExpandedPos = {}; // 퍼진 후 위치
	_float  m_fExpandTime = 0.f;
	_bool m_bRotated_Bullet{}; //왼쪽 오른쪽 번걸아가면서 쏘는거
	_uint m_iPatternType{}; 
	_float3 m_vTargetOffsetPos = _float3(0.f, 0.f, 0.f);
	_bool m_bOffsetSet = false;


};
END
