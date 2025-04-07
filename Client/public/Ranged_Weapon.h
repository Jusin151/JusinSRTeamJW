#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "Weapon_Base.h"
#include "PickingSys.h"
#include "Collider.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
END

BEGIN(Client)

class CRanged_Weapon : public CWeapon_Base
{
public:
    typedef struct tagRanged_Info
    {
        int MaxAmmo;         // 최대 탄약 수
        int CurrentAmmo;     // 현재 탄약
    }Ranged_DESC;

    struct CollisionInfo {
        CCollider* pCollider = nullptr; // 충돌한 콜라이더 포인터
        _float3 vHitPosition{};         // 충돌 지점
        float fDistance = std::numeric_limits<float>::max(); // 광선 원점으로부터의 거리

        // 거리를 기준으로 오름차순 정렬하기 위한 비교 연산자
        bool operator<(const CollisionInfo& other) const {
            return fDistance < other.fDistance;
        }
    };


protected:
    CRanged_Weapon(LPDIRECT3DDEVICE9 pGraphic_Device);
    CRanged_Weapon(const CRanged_Weapon& Prototype);
    virtual ~CRanged_Weapon() = default;

public:
    virtual HRESULT Initialize_Prototype()PURE;
    virtual HRESULT Initialize(void* pArg)PURE;
    virtual HRESULT Ready_Components();
public:
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
public:
    virtual HRESULT Render()override;
public:
    virtual HRESULT Ready_Picking();
    virtual HRESULT Picking_Object(_uint EffectNum,_uint Damage);
    void Wall_Picking(CCollider* pCollider, _uint EffectNum);
    void Monster_Hit(CCollider* pCollider, _uint Damage);
    void CreateHitEffect(CCollider* pClosestCollider, const _float3& vWorldHitPos, _uint Damage);
    void CreateBossHitEffect(CCollider* pClosestCollider, const _float3& vWorldHitPos, _uint Damage);
public:
    CGameObject* Clone(void* pArg) override;
    void Free();
      
public: 
    Ranged_DESC Ranged_INFO = {};
    _bool m_bAttackInput = { false };
public: //옵저버 관련
    virtual void Attack_WeaponSpecific(_float fTimeDelta)PURE;
   virtual  void Attack(_float fTimeDelta)
    {
        Attack_WeaponSpecific(fTimeDelta);

    }
  void Add_Ammo(int iAmmo)
   {
       Ranged_INFO.CurrentAmmo += iAmmo;
   
       if (Ranged_INFO.CurrentAmmo > Ranged_INFO.MaxAmmo)
           Ranged_INFO.CurrentAmmo = Ranged_INFO.MaxAmmo;

       if(m_bIsActive)
           Notify_Bullet(); // UI 갱신
   }
  void Set_MaxBullet(_int capacity)
  {
      Ranged_INFO.MaxAmmo = capacity;

      // 현재 탄약이 더 크면 줄여야 함
      if (Ranged_INFO.CurrentAmmo > Ranged_INFO.MaxAmmo)
          Ranged_INFO.CurrentAmmo = Ranged_INFO.MaxAmmo;

      Notify_Bullet();
  }

  


protected:
    _float m_fElapsedTime = { 0.f };
    _int m_iCurrentFrame = { 0 };
    const _float m_fFrameDuration = { 2.0f };
    _bool m_bIsAnimating={false};
    bool m_bHasFired = { false }; // 발사가 한번만 되게 하는거
protected:
    CPickingSys* m_pPickingSys = { nullptr }; // 피킹 객체
    _bool m_bWall = { false };   // 벽체크용 변수
    _bool m_bMonster = { false }; // 몬스터 체크용 변수=
    void Move_Hand(_float fTimeDelta) override;
protected:
    _float  m_fRange = { 100.f };


public:
    void Notify_Bullet()
    {
     
        if (m_pObserver)
            m_pObserver->OnNotify(&Ranged_INFO, L"BULLET");
    }
};

END
