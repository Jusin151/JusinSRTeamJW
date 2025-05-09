﻿#pragma once

#include "Monster_Base.h"

class CCrocman : public CMonster_Base
{
private:
	CCrocman(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCrocman(const CCrocman& Prototype);
	virtual ~CCrocman() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	// 마지막에 플레이어 방향으로 바라보도록 함
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

	virtual void Deserialize(const json& j) override;

public:
	virtual HRESULT On_Collision(CCollisionObject* other) override;
	// 일정 거리 까지 쫓아가거나, 일정 거리 이내면 콜라이더 생성해서 공격하도록
	virtual void Select_Pattern(_float fTimeDelta) override;

	
	void Attack_Melee(_float fTimeDelta);

	// 각 상태에 맞게 프레임을 설정함
	void Select_Frame(_float fTimeDelta);

private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	// 텍스처 추가 
	HRESULT Ready_Components();


private:
	// 공격 때 만들 콜라이더
	CCollider_Cube* m_pAttackCollider = { nullptr };

	

public:
	static CCrocman* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();
};

