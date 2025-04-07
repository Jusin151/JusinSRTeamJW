#pragma once
#include <CollisionObject.h>


BEGIN(Engine)
class CShader;
class CTexture;
class CMaterial;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Sphere;
class CCollider_Cube;
END

BEGIN(Client)
class CCthulhu_Big_Tentacle final : public CCollisionObject
{
private:
    CCthulhu_Big_Tentacle(LPDIRECT3DDEVICE9 pGraphic_Device);
    CCthulhu_Big_Tentacle(const CCthulhu_Big_Tentacle& Prototype);
    virtual ~CCthulhu_Big_Tentacle() = default;

public:
    virtual HRESULT Initialize_Prototype()override;
    virtual HRESULT Initialize(void* pArg)override;
    virtual void Priority_Update(_float fTimeDelta)override;
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    virtual HRESULT Render()override;


public:
    virtual HRESULT On_Collision(CCollisionObject* other);
    void Billboarding(_float fTimeDelta);
    void Set_Position(const _float3& vPos);
    void Set_Texture(_uint iIndex) { if (iIndex < 0) return; m_iCurrentFrame = iIndex; }
private:
    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();

    HRESULT Ready_Components();
private:
    _uint m_iCurrentFrame{ 0 };
    CGameObject* m_pTarget{ nullptr };

    CShader* m_pShaderCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CCollider_Cube* m_pColliderCom = { nullptr };
    CMaterial* m_pMaterialCom = { nullptr };

public:
    static CCthulhu_Big_Tentacle* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
