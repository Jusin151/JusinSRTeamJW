#pragma once
#include "Effect_Base.h"
BEGIN(Client)
class CMagic_Effect final : public CEffect_Base
{
public:
    typedef struct tagMagicDesc : EFFECT_DESC {
        _uint       type;
        DWORD        dwInitialColor;
        DWORD        dwFadeColor;
    } HIT_DESC;
private:
    CMagic_Effect(LPDIRECT3DDEVICE9 pGraphic_Device);
    CMagic_Effect(const CMagic_Effect& Prototype);
    virtual ~CMagic_Effect() = default;
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
    static CMagic_Effect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CMagic_Effect* Clone(void* pArg) override;
    virtual void Free()override;
private:
    CGameObject* m_pTarget = { nullptr };
    DWORD        m_dwInitialColor;
    DWORD        m_dwFadeColor;
};
END

