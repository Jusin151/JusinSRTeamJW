#pragma once

#include "HellBoss_SkillBase.h"

class CHellBoss_Skill_Landing : public CHellBoss_SkillBase
{
private:
	CHellBoss_Skill_Landing(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHellBoss_Skill_Landing(const CHellBoss_Skill_Landing& Prototype);
	virtual ~CHellBoss_Skill_Landing() = default;

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
	static CHellBoss_Skill_Landing* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();


};

