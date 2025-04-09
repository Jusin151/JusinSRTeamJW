#pragma once

#include "Monster_Base.h"
#include "AnimationManager.h" 
#include "HellBoss_State.h"
#include "Pattern_Base.h"
#include "Player.h"
#include "HellBoss_Bullet.h"


class CHellBoss_SkillBase : public CGameObject
{
public:


private:
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
private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();
public:
	static CHellBoss_SkillBase* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();
private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

};

