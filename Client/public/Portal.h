#pragma once
#include "Shop.h"

BEGIN(Client)
class CPortal : public CGameObject
{
private:
    CPortal(LPDIRECT3DDEVICE9 pGraphic_Device);
    CPortal(const CPortal& Prototype);
    virtual ~CPortal() = default;

public:

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

    virtual void Priority_Update(_float fTimeDelta)override;
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    HRESULT SetUp_RenderState();
    HRESULT On_Collision();
    void LookAtPlayer(_float fTimeDelta);
    HRESULT Release_RenderState();
    virtual HRESULT Render()override;
protected:
    CTexture* m_pTextureCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CCollider_Sphere* m_pColliderCom = { nullptr };
private:
    HRESULT Ready_Components();

public:
    static CPortal* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;


    _bool m_bPortal_On{};
private:
    CGameObject* m_pPlayer = { nullptr }; // 상점이 늘 플레이어를 쳐다보게 하기 위해서 선언
    _float m_fFrameTime = {};
    _uint m_iCurrentFrame = {};
};
END