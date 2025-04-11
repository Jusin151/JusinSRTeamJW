#pragma once
#include "Effect_Base.h"
BEGIN(Client)
class CStains_Effect final : public CEffect_Base
{
public:
    enum HitType { HT_A, HT_B, HT_END };
    typedef struct tagHitDesc : EFFECT_DESC {
        _uint       type;
    } HIT_DESC;
private:
    CStains_Effect(LPDIRECT3DDEVICE9 pGraphic_Device);
    CStains_Effect(const CStains_Effect& Prototype);
    virtual ~CStains_Effect() = default;
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
    static CStains_Effect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CStains_Effect* Clone(void* pArg) override;
    virtual void Free()override;
private:
    CGameObject*    m_pTarget = { nullptr };
    HitType         m_eHitType = { HT_A };
};
END