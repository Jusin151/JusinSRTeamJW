#pragma once
#include "Shop.h"

BEGIN(Client)
class CHub_Portal : public CGameObject 
{
public:
    typedef struct tagPortalDesc
    {
        _uint eLevel;
    }
    PORTAL_DESC;
private:
    CHub_Portal(LPDIRECT3DDEVICE9 pGraphic_Device);
    CHub_Portal(const CHub_Portal& Prototype);
    virtual ~CHub_Portal() = default;

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

    LEVEL Get_Level() const { return m_eLevel; }
    _bool IsOpen_Portal() const { return m_bPortal_On; }
protected:
    CLight* m_pLightCom = { nullptr };
    CShader* m_pShaderCom = { nullptr };
    CMaterial* m_pMaterialCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CCollider_Sphere* m_pColliderCom = { nullptr };
    CParticle_System* m_pParticleCom = { nullptr };
private:
    HRESULT Ready_Components();

public:
    static CHub_Portal* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;


    _bool m_bPortal_On{};
private:
    CGameObject* m_pPlayer = { nullptr }; // 상점이 늘 플레이어를 쳐다보게 하기 위해서 선언
    _float m_fFrameTime = {};
    _uint m_iCurrentFrame = {};
    LEVEL m_eLevel;
public:
    void OnNotify(void* pArg, const wstring& type)
    {
    }
};
END