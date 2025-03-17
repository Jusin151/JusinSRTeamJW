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

class CMelee_Weapon  : public CWeapon_Base
{

public:
    struct MeleeWeapon_Info
    {
        float KnockbackForce;  // 타격 시 넉백 (0이면 넉백 없음)
        int ComboCount;        // 콤보 공격 가능 횟수
        float ComboDelay;      // 콤보 공격 간 딜레이
    };
protected:
    CMelee_Weapon(LPDIRECT3DDEVICE9 pGraphic_Device);
    CMelee_Weapon(const CMelee_Weapon& Prototype);
    virtual ~CMelee_Weapon() = default;

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
