#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "CollisionObject.h"
#include "Transform.h"
#include "Observer.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CMaterial;
class CSound_Source;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
END

BEGIN(Client)

class CWeapon_Base abstract : public CCollisionObject
{
public:
    enum WEAPON_ID
    {
        Claymore, Axe, ShotGun, Magnum, Staff,
        Minigun, Harvester,Sonic
    };

public:

    typedef struct tagWeapon_INFO
    {
        WEAPON_ID WeaponID{};          // 무기 고유 ID
        _float2 vPos = {};  // 무기 화면 좌표
        _float2 vSize = {};  // 무기 크기
        _uint Damage{};            // 기본 공격력
        _float AttackSpeed{};       // 공격 속도(사실상 쿨타임?
        wstring TextureKey{};      // 무기에 사용될 텍스처 키

    }Weapon_DESC;


protected:
    Weapon_DESC m_Weapon_INFO{};

protected:
    CWeapon_Base(LPDIRECT3DDEVICE9 pGraphic_Device);
    CWeapon_Base(const CWeapon_Base& Prototype);
    virtual ~CWeapon_Base() = default;

public:
    virtual HRESULT Initialize_Prototype()PURE;
    virtual HRESULT Initialize(void* pArg)PURE;
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta)PURE;
    virtual void Late_Update(_float fTimeDelta)PURE;
    virtual HRESULT Render()PURE;
    virtual HRESULT Ready_Components()PURE;
    virtual  void Free();
    virtual void Attack(_float fTimeDelta)PURE;
    virtual void Move_Hand(_float fTimeDelta);
    virtual HRESULT On_Collision(CCollisionObject* other);
    virtual void Attack_WeaponSpecific(_float fTimeDelta) PURE;


protected:
    CShader* m_pShaderCom = {};
    CTexture* m_pTextureCom = {};
    CMaterial* m_pMaterialCom = {};
    CTransform* m_pTransformCom = {};
    CSound_Source* m_pSoundCom = {};
    CVIBuffer_Rect* m_pVIBufferCom = {};
    
public:
    _bool  Get_MouseClick() const { return m_bIsMouseClick; }
    void   Set_MouseClick(_bool type) { m_bIsMouseClick = type; }
    virtual wstring GetLayerID() { return L"Default"; }
    _float3 GetInitialPos() const { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
    WEAPON_ID Get_Weapon_ID() { return m_Weapon_INFO.WeaponID; }

protected:
    bool m_bIsMouseOver = {};
    bool m_bIsMouseClick = {};

    // 상태별 시작 인덱스 관리
    map<string, pair<int, int>> m_TextureRanges{};

 
    int GetTextureIndex(string& stateName, int frameNum)
    {
        auto range = m_TextureRanges[stateName];
        return range.first + (frameNum % (range.second - range.first + 1));
    }

protected:
    _float m_fAnimationSpeed = {}; // 애니메이션속도
    _bool m_bTriggerPressed = { false };
    bool m_bPrevMouseDown = false;


protected://근접무기쪽관련
    _float m_fElapsedTime = {};
    _int m_iCurrentFrame = {};
    _int m_iLastFrame = {};
    const _float m_fFrameDuration = { 2.0f };
protected: // 손훈들리는 거 관련
    _float t = {}; 
    _float speed = { 0.1f }; 
    _float2 m_vInitialPos = {};
protected: // 옵저버 관련
    CObserver* m_pObserver = { nullptr };
public:
    void Add_Observer(CObserver* pObserver)
    {
        m_pObserver = pObserver;
    }
    virtual void Notify_Bullet()PURE;
    
};
END
