#pragma once
#include "Effect_Base.h"
BEGIN(Client)
class CSmoke_Effect final : public CEffect_Base
{
public:
    typedef struct tagSmokeDesc : EFFECT_DESC {
        _uint       type;
    } HIT_DESC;
private:
    CSmoke_Effect(LPDIRECT3DDEVICE9 pGraphic_Device);
    CSmoke_Effect(const CSmoke_Effect& Prototype);
    virtual ~CSmoke_Effect() = default;
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
    static CSmoke_Effect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CSmoke_Effect* Clone(void* pArg) override;
    virtual void Free()override;
private:
    CGameObject* m_pTarget = { nullptr };
};
END