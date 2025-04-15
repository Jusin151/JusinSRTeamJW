#pragma once

#include "Client_Defines.h"
#include "Weapon_Base.h"
#include "LandObject.h"
#include "Inventory.h"
#include "Item_Manager.h"
#include "CollisionObject.h"
#include "Observer.h"
#include "UI_Manager.h"
#include "UI_Event.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CSound_Source;
class CVIBuffer_Cube;
class CCollider_Sphere;
class CCollider_Cube;
END

class CHellBoss_AfterImage : public CGameObject
{
public:
    struct DESC
    {
        _float3 vPos;
        _float3 vRight;
        _float3 vUp;
        _float3 vLook;
        _int iAnimFrame;
    };

public:
    CHellBoss_AfterImage(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual ~CHellBoss_AfterImage() = default;

    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update(_float fDeltaTime) override;
    virtual void Late_Update(_float fDeltaTime) override;
    virtual HRESULT Render() override;

private:
    HRESULT Ready_Components();
    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();
public:
  static  CHellBoss_AfterImage* Create(LPDIRECT3DDEVICE9 pGraphic_Device);

    CGameObject* Clone(void* pArg);

    void Free();

private:
    CTransform* m_pTransformCom = nullptr;
    CVIBuffer_Rect* m_pVIBufferCom = nullptr;
    CTexture* m_pTextureCom = nullptr;
    CShader* m_pShaderCom = nullptr;

    _float m_fLifeTime = 0.f;
    _float m_fAlpha = 0.6f;
    _int m_iFrame = 0;
};
