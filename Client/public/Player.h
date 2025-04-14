#pragma once

#include "Client_Defines.h"
#include "Weapon_Base.h"
#include "LandObject.h"
#include "Inventory.h"
#include "Item_Manager.h"
#include "CollisionObject.h"
#include "Observer.h"
#include "UI_Manager.h"
#include "UI_Event.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CSound_Source;
class CVIBuffer_Cube;
class CCollider_Sphere;
class CCollider_Cube;
END

BEGIN(Client)

class CPlayer final : public CCollisionObject
{
	enum class WEAPON_STATE
	{
		IDLE,
		FIRE,
		CHANGE
	};

private:
	CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
	virtual HRESULT On_Collision(CCollisionObject* other)override;
	CTransform* Get_TransForm() { return m_pTransformCom; }
private:
	void Move(_float fTimeDelta);
	void Attack(_float fTimeDelta);
	void Input_Key(_float fTimeDelta);


private: //옵저버 관련

	vector<CObserver*> m_pObservers{};
public:
	void Add_Observer(CObserver* pObserver)
	{
		m_pObservers.push_back(pObserver);
	}
	virtual void Notify(_int value, const wstring& type)
	{
		for (auto& obs : m_pObservers)
			obs->OnNotify(&value, type);
	}
	void Set_Hp(_int iHp);
	void Take_Damage(_uint Damage);

	void Set_Ap(_int iAp)override { m_iAp = iAp; }
	_bool Set_Mp(_int iMp);
	
	void Add_Ammo(const _wstring& stWeaponName, _int iAmmo);
	void Add_Strength(_int Str);  // 근접무기 전용 데미지 증가

	void Add_MaxHP(_int Hp);

	// 최대체력 증가
	void Add_HP(_int Hp); // 현재체력 증가
	void Add_MP(_int Mp);
	void Add_Sprit(_int Sprit); // 정신력 증가
	
	void Add_SkillPoint(_int SkillPoint); // 스킬포인트 증가
	
	void Add_Capacity(_int type); //원거리무기군 최대탄약증가

	void Add_Exp(_int Exp); // 경험치 증가

	_bool TryUseMana(_int amount) // 스태프 마나 고갈 체크
	{
		if (m_iPlayerMP.first < amount)
			return false;

		m_iPlayerMP.first -= amount;
		Notify(m_iPlayerMP.first, L"MP");
		return true;
	}


	void Add_Weapon(const _wstring& stWeaponTag);


public:
	void Set_DoubleAmmoGain(_bool bEnable) { m_bDoubleAmmoGain = bEnable; }
	void Set_DoubleSpeedGain() 
	{ 
	     m_fSpeed = 1.f; 
	}
	 _bool Has_Item(const _wstring& stItemTag);
	 HRESULT Add_Item(const _wstring& stItemTag);

	 pair<_int, _int> Get_PlayerHp() { return m_iPlayerHP; }
	 pair<_int, _int> Get_PlayerMp() { return m_iPlayerMP; }
	 CWeapon_Base* Get_Current_Weapon() { return m_pPlayer_Weapon; }
private:
			bool m_bDoubleAmmoGain = false; // 탄약 획득량 2배 여부
			bool m_bDoubleSpeedGain = false; // 탄약 획득량 2배 여부
private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();
	HRESULT Ready_Player_SetUP();
public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(); 

private: // 인벤관련
	void Input_WeapontoInven(wstring tag, _uint Index)
	{
		m_pPlayer_Inven->Add_Weapon(tag, Index);
	}

	void Input_ItemtoInven();
	_bool m_bInven_Render_State{};

	void Equip(_float fTimeDelta);

private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CSound_Source* m_pSoundCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
private:
	POINT m_ptOldMousePos = {};
	_float m_fMouseSensor = {};
	WEAPON_STATE m_eWeaponState = WEAPON_STATE::IDLE;
private:
	CGameObject* m_pCamera = nullptr;
	CCollider_Cube* m_pColliderCom = { nullptr };
public:
	void StopAction() { m_bTimeControl = !m_bTimeControl; }

private:
	_float			m_fShakeTime = {};
	_bool m_bTimeControl = { false };
	_float m_fSaveTime = {};
	LONG			m_lMiddlePointX = { g_iWinSizeX / 2 };

private: // 플레이어 관련
	pair<_int, _int> m_iPlayerHP{};    // 플레이어 현재/최대체력
	pair<_int, _int> m_iPlayerMP{};    // ``  현재/최대마나
	pair<_int, _int> m_iPlayerEXP{};   //`` 현재/최대경험치
	_uint m_iStr{}; // 근력
	_uint m_iLife{};//생명력
	_uint m_iSprit{};//정신력
	_uint m_iCapacity{};//용량
	_uint m_iLevel{}; // 레벨
	_uint m_iSkillpoint{};//스킬포인트
	_uint m_iStatpoint{};//스탯포인트
	CWeapon_Base* m_pPlayer_Weapon = { nullptr };
	CInventory* m_pPlayer_Inven = { nullptr };

	_float m_fAvoidTime = { 0.f };
};


END