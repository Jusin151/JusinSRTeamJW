#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "Weapon_Headers.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
END

BEGIN(Client)

class CWeapon_Base abstract : public CGameObject
{
public:
    enum WEAPON_ID
    {
        Claymore
    };


public:

    typedef struct tagWeapon_INFO
    {
        WEAPON_ID WeaponID{};          // 무기 고유 ID
        _float2 vPos = {};  // UI 화면 좌표
        _float2 vSize = {};  // UI 크기
        _float Damage{};            // 기본 공격력
        _float AttackSpeed{};       // 공격 속도
        _float Range{};             // 공격 범위 (근접/원거리 공통)
        _float Cooldown{};          // 공격 쿨타임
        _bool IsEquipped{};         // 현재 장착 중인지 여부
        wstring TextureKey{};      // 무기 UI에 사용될 텍스처 키
    }Weapon_DESC;


 
protected:
    CWeapon_Base(LPDIRECT3DDEVICE9 pGraphic_Device);
    CWeapon_Base(const CWeapon_Base& Prototype);
    virtual ~CWeapon_Base() = default;

public:
    virtual HRESULT Initialize_Prototype()PURE;
    virtual HRESULT Initialize(void* pArg)PURE;
    virtual void Priority_Update(_float fTimeDelta)PURE;
    virtual void Update(_float fTimeDelta)PURE;
    virtual void Late_Update(_float fTimeDelta)PURE;
    virtual HRESULT Render()override;
    virtual HRESULT Ready_Components();
    virtual  void Free();
    virtual void Attack()PURE;

protected:
    CTexture* m_pTextureCom = nullptr;
    CVIBuffer_Rect* m_pVIBufferCom = nullptr;
    CTransform* m_pTransformCom = nullptr;

public:
    _bool  Get_MouseClick() const { return m_bIsMouseClick; }
    void   Set_MouseClick(_bool type) { m_bIsMouseClick = type; }
    virtual wstring GetLayerID() { return L"Default"; }

    Weapon_DESC m_Weapon_INFO;

protected:


    bool m_bIsMouseOver = {};
    bool m_bIsMouseClick = {};



};

END
