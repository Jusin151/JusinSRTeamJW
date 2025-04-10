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
    virtual void Move_Hand(_float fTimeDelta);
    void Free();
    CGameObject* Clone(void* pArg) ;
    virtual HRESULT On_Collision(CCollisionObject* other) { return S_OK; }

public:

    void Attack(_float fTimeDelta);

protected:
    class CCollider_Cube* m_pColliderCom = { nullptr };
    // 근접 무기의 경우 일단 플레이어랑 맞추고 조금씩 pos만 변형해서 쓰면 될듯?
    CTransform* m_pColTransformCom = {nullptr};
    _bool m_bIsAnimating  =  { false};
    _bool m_bAttack = { false };
public: // 근접 들었을때 총알바 안뜨게 해야할꺼같아서
    void Notify_Bullet()
    {
        if (m_pObserver)
            m_pObserver->OnNotify(nullptr, L"BULLET_NO");
    }

  
};
END
