#pragma once

#include "Monster_Base.h"
#include "Weapon_Headers.h"

class CStaff_Bullet : public CMonster_Base 
{
public:
	


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
	virtual HRESULT On_Collision(_float fTimeDelta) override;
	// 일단 쫓아가는 패턴만
	virtual void Select_Pattern(_float fTimeDelta) override;

	void Attack_Melee();

private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	// 텍스처 추가 
	HRESULT Ready_Components();


private:
	// 공격 때 만들 콜라이더
	CCollider_Cube* m_pAttackCollider = { nullptr };

public:
	static CStaff_Bullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();
private:
	CTransform* m_Player_Transform = { nullptr };
	_float3 Player_RIght{};
	_float3 Player_Up{};
	_float3 Player_Look{};
	_float3 Player_Pos{};
private:
	_float m_fSpeed = 10.0f; // 탄환 속도
	_float m_fElapsedTime = 0.0f; // 애니메이션 프레임 시간
	_int m_iCurrentFrame = 0; // 현재 애니메이션 프레임
};

