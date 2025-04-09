#pragma once

#include "Monster_Base.h"
#include "AnimationManager.h" 
#include "HellBoss_State.h"
#include "Pattern_Base.h"
#include "Player.h"
#include "HellBoss_Bullet.h"
#include "Effect_Base.h"


class CHellBoss_Skill_Warp : public CEffect_Base
{
public:


private:
	CHellBoss_Skill_Warp(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHellBoss_Skill_Warp(const CHellBoss_Skill_Warp& Prototype);
	virtual ~CHellBoss_Skill_Warp() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	void Process_Input();
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();
public:
	static CHellBoss_Skill_Warp* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();


};

