#pragma once

#include "Monster_Base.h"
#include "Weapon_Headers.h"

class CBullet_Base : public CCollisionObject
{


protected:
	CBullet_Base(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBullet_Base(const CBullet_Base& Prototype);
	virtual ~CBullet_Base() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	// 마지막에 플레이어 방향으로 바라보도록 함
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
protected:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };


public:
	virtual HRESULT On_Collision(CCollisionObject* other)override;

	void Attack_Melee();

private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	// 텍스처 추가 
	HRESULT Ready_Components();


protected:
	// 공격 때 만들 콜라이더, 각 불릿에서 지정
	CCollider* m_pAttackCollider = { nullptr };

public:
	static CBullet_Base* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();
protected:
	CTransform* m_Player_Transform = { nullptr };
	_float3 Player_RIght{};
	_float3 Player_Up{};
	_float3 Player_Look{};
	_float3 Player_Pos{};
protected:
	_float m_fSpeed = 10.0f; // 탄환 속도
	_float m_fElapsedTime = 0.0f; // 애니메이션 프레임 시간
	_int m_iCurrentFrame = 0; // 현재 애니메이션 프레임
};

