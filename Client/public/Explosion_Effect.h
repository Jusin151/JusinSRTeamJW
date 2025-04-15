#pragma once
#include "Effect_Base.h"
BEGIN(Client)
class CExplosion_Effect final : public CEffect_Base
{
private:
    CExplosion_Effect(LPDIRECT3DDEVICE9 pGraphic_Device);
    CExplosion_Effect(const CExplosion_Effect& Prototype);
    virtual ~CExplosion_Effect() = default;
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
    static CExplosion_Effect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CExplosion_Effect* Clone(void* pArg) override;
    virtual void Free()override;
private:
    CGameObject* m_pTarget = { nullptr };
    //Frame
};
END