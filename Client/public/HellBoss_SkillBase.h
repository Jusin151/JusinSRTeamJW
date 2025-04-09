#pragma once

#include "Monster_Base.h"
#include "AnimationManager.h" 
#include "Effect_Base.h" 


class CHellBoss_SkillBase : public CEffect_Base
{
protected:
	CHellBoss_SkillBase(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHellBoss_SkillBase(const CHellBoss_SkillBase& Prototype);
	virtual ~CHellBoss_SkillBase() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	void Process_Input();
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
protected:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();
public:
	static CHellBoss_SkillBase* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();
protected: 
	CTexture* m_pSkill_TextureCom = { nullptr };
	CTransform* m_pSkill_TransformCom = { nullptr };
	CVIBuffer_Rect* m_pSkill_VIBufferCom = { nullptr };

	void Set_Animation(const string& strAnimKey) { m_AnimationManager.SetCurrentAnimation(strAnimKey); }
	bool Get_AnimationFinished() const { return m_AnimationManager.IsFinished(); }
	_uint Get_CurAnimationFrame() { return m_AnimationManager.GetCurrentFrame(); }
	LPDIRECT3DDEVICE9 Get_Graphic_Device() { return m_pGraphic_Device; }
	CGameInstance* Get_GameInstance() { return m_pGameInstance; }
	CAnimationManager* Get_AnimationManager() { return &m_AnimationManager; }
	CAnimationManager m_AnimationManager = {};

};

