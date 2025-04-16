#pragma once
#include "Effect_Base.h"
BEGIN(Client)
class CGib_Effect final : public CEffect_Base
{
public:
    enum MonsterType { MT_NORMAL, MT_SNOWSPIDER, MT_YETI, MT_GLACIER_HIT, MT_GLACIER_DEAD, MT_HELLHOUND_DAMAGED, MT_HELLHOUND_DEAD, MT_END };
    typedef struct tagGibDesc : EFFECT_DESC {
        _uint       type;
    } HIT_DESC;
private:
    CGib_Effect(LPDIRECT3DDEVICE9 pGraphic_Device);
    CGib_Effect(const CGib_Effect& Prototype);
    virtual ~CGib_Effect() = default;
public:
    virtual HRESULT Initialize_Prototype()override;
    virtual HRESULT Initialize(void* pArg)override;
    virtual HRESULT Ready_Components()override;
public:
    virtual void Priority_Update(_float fTimeDelta)override;
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
public:
    HRESULT Pre_Render();
    virtual HRESULT Render()override;
    HRESULT Post_Render();
public:
    static CGib_Effect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGib_Effect* Clone(void* pArg) override;
    virtual void Free()override;
private:
    CGameObject*        m_pTarget = { nullptr };
    MonsterType         m_eMonsterType = { MT_NORMAL };
};
END