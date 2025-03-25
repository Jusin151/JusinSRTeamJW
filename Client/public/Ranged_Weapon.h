#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "Weapon_Base.h"
#include "PickingSys.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
END

BEGIN(Client)

class CRanged_Weapon : public CWeapon_Base
{
public:
    struct RangedWeapon_Info
    {
        int MaxAmmo;         // 최대 탄약 수
        int CurrentAmmo;     // 현재 탄약
        float ReloadTime;    // 재장전 시간
        float BulletSpeed;   // 탄환 속도
        float BulletSpread;  // 탄 퍼짐 정도
    };

protected:
    CRanged_Weapon(LPDIRECT3DDEVICE9 pGraphic_Device);
    CRanged_Weapon(const CRanged_Weapon& Prototype);
    virtual ~CRanged_Weapon() = default;

public:
    virtual HRESULT Initialize_Prototype()PURE;
    virtual HRESULT Initialize(void* pArg)PURE;
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render()override;
    virtual HRESULT Ready_Components();
    virtual HRESULT Ready_Picking();
    virtual HRESULT Picking_Object(_uint EffectNum);
    void Free();


    CGameObject* Clone(void* pArg) override;


    // CWeapon_Base을(를) 통해 상속됨
    void Attack(_float fTimeDelta) override;

protected:
    _float m_fElapsedTime = 0.0f;
    _int m_iCurrentFrame = 0;
    const _float m_fFrameDuration = 2.0f;
    _bool m_bIsAnimating{};
    bool m_bHasFired = { false }; // 발사가 한번만 되게 하는거
protected:
    CPickingSys* m_pPickingSys = { nullptr }; // 피킹 객체
    _bool m_bWall = { false };   // 벽체크용 변수
    _bool m_bMonster = { false }; // 몬스터 체크용 변수


    // CWeapon_Base을(를) 통해 상속됨
    void Move_Hand(_float fTimeDelta) override;

};

END
