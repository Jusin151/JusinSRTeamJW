#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "Weapon_Base.h"


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
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render()override;
    virtual HRESULT Ready_Components();
    void Free();


    CGameObject* Clone(void* pArg) override;


    // CWeapon_Base을(를) 통해 상속됨
    void Attack() override;

};

END
