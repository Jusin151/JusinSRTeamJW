#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
END

BEGIN(Client)

class CEffect_Base abstract : public CGameObject
{
public:
    enum class EFFECT_TYPE
    {
        NONE, // 기본값
        BULLET_HIT, // 총알 흔적
        SLASH, // 근접 흔적
    };
    struct EFFECT_DESC
    {
        _float3 vPos = {};
        _float3 vScale = {};
        _float3 vRight = {};
        _float3 vUp = {};
        _float3 vLook = {};
        _float fLifeTime = {};
        EFFECT_TYPE eType = EFFECT_TYPE::NONE;
    };

protected:
    EFFECT_TYPE m_eType = EFFECT_TYPE::NONE;

protected:
    CEffect_Base(LPDIRECT3DDEVICE9 pGraphic_Device);
    CEffect_Base(const CEffect_Base& Prototype);
    virtual ~CEffect_Base() = default;

public:
    virtual HRESULT Initialize_Prototype()PURE;
    virtual HRESULT Initialize(void* pArg)PURE;
    virtual void Priority_Update(_float fTimeDelta)PURE;
    virtual void Update(_float fTimeDelta)PURE;
    virtual void Late_Update(_float fTimeDelta)PURE;
    virtual HRESULT Render()override;
    virtual HRESULT Ready_Components();
    virtual  void Free();

protected:
    CTexture* m_pTextureCom = {};
    CVIBuffer_Rect* m_pVIBufferCom = {};
    CTransform* m_pTransformCom = {};
    EFFECT_DESC m_Weapon_Effect_INFO{};

protected: // 이펙트 관련
    _float m_fLifeTime = {}; // 최대 몇초까지 남게할껀지
    _float m_fCurrentTime = {}; // 현재 시간
    _bool m_bDead = {}; // 뒈지면 Active 활성화
};
END
