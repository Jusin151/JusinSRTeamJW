#pragma once
#include "Effect_Base.h"
BEGIN(Client)
class CBulletShell_Effect final : public CEffect_Base
{
public:
    typedef struct tagBulletShellDesc : EFFECT_DESC {
        _uint       type;
    } BULLETSHELLDESC;
private:
    CBulletShell_Effect(LPDIRECT3DDEVICE9 pGraphic_Device);
    CBulletShell_Effect(const CBulletShell_Effect& Prototype);
    virtual ~CBulletShell_Effect() = default;
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
    static CBulletShell_Effect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CBulletShell_Effect* Clone(void* pArg) override;
    virtual void Free()override;
private:
    CGameObject* m_pTarget = { nullptr };
    CSound_Source* m_pSoundCom = { nullptr };
};
END