#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "Weapon_Base.h"
#include "Collider_Cube.h"



BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
class CCollider_Cube;
END

BEGIN(Client)

class CMelee_Weapon : public CWeapon_Base
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
    virtual HRESULT Initialize_Prototype()PURE;
    virtual HRESULT Initialize(void* pArg)PURE; 
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();
    virtual HRESULT Ready_Components();
    void Free();
    CGameObject* Clone(void* pArg) ;


public:

    void Attack(_float fTimeDelta);

protected:
    class CCollider_Cube* m_pColliderCom = { nullptr };
    // 근접 무기의 경우 일단 플레이어랑 맞추고 조금씩 pos만 변형해서 쓰면 될듯?
    CTransform* m_pColTransformCom = {nullptr};
    _bool m_bIsAnimating    { false};

private:
    _float m_fElapsedTime = 0.0f;
    _int m_iCurrentFrame = 0;
    const _float m_fFrameDuration = 2.0f;
    


};
END
