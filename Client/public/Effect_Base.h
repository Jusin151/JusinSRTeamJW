#pragma once

#include "Client_Defines.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Particle_System.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CMaterial;
class CParticle_System;
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
        _float fScaleY = {};
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
    virtual HRESULT Ready_Components();
public:
    virtual void Priority_Update(_float fTimeDelta)PURE;
    virtual void Update(_float fTimeDelta)PURE;
    virtual void Late_Update(_float fTimeDelta)PURE;
public:
    virtual HRESULT Render()override;
public:
    virtual  void Free();

protected:
    CShader*            m_pShaderCom            = {};
    CTexture*           m_pTextureCom           = {};
    CMaterial*          m_pMaterialCom          = {};
    CTransform*         m_pTransformCom         = {};
    CVIBuffer_Rect*     m_pVIBufferCom          = {};
    CParticle_System*   m_pParticleCom          = {};
    EFFECT_DESC         m_Weapon_Effect_INFO    = {};
    

protected: // 이펙트 관련
    _float  m_fLifeTime = {}; // 최대 몇초까지 남게할껀지
    _float  m_fCurrentTime = {}; // 현재 시간
    _bool   m_bDead = {}; // 죽으면 Active 활성화
    _float  m_fAnimationSpeed = {}; // 애니메이션속도
    _float  m_fElapsedTime = {};
    _uint    m_iCurrentFrame = {};
    _uint    m_iLastFrame = {};
    const _float m_fFrameDuration = { 2.0f };
};
END
