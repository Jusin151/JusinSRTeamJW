﻿#pragma once

#include "Client_Defines.h"
#include "Weapon_Base.h"
#include "LandObject.h"
#include "Inventory.h"
#include "Item_Manager.h"
#include "CollisionObject.h"
#include "Observer.h"
#include "UI_Manager.h"
#include "UI_Event.h"
#include "Observer.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CSound_Source;
class CVIBuffer_Cube;
class CCollider_Sphere;
class CCollider_Cube;
END

BEGIN(Client)

class CPlayer final : public CCollisionObject, public CObserver
{
public:
	typedef struct tagPlayerInfo
	{
		_uint iStr{0u}; // 근력
		_uint iLife{ 0u };//생명력
		_uint iSprit{ 0u };//정신력
		_uint iCapacity{ 0u };//용량
		_uint iLevel{ 0u }; // 레벨
		_uint iSkillpoint{ 0u };//스킬포인트
		_uint iStatpoint{ 0u };//스탯포인트

		tagPlayerInfo() = default;

		// 전체 초기화 생성자
		tagPlayerInfo(_uint _iStr,
			_uint _iLife,
			_uint _iSprit,
			_uint _iCapacity,
			_uint _iLevel,
			_uint _iSkillpoint,
			_uint _iStatpoint)
			: iStr(_iStr),
			iLife(_iLife),
			iSprit(_iSprit),
			iCapacity(_iCapacity),
			iLevel(_iLevel),
			iSkillpoint(_iSkillpoint),
			iStatpoint(_iStatpoint)
		{
		}

		// 연산자 오버로딩
		tagPlayerInfo operator-(const tagPlayerInfo& other) const {
			return tagPlayerInfo(
				iStr - other.iStr,
				iLife - other.iLife,
				iSprit - other.iSprit,
				iCapacity - other.iCapacity,
				iLevel - other.iLevel,
				iSkillpoint - other.iSkillpoint,
				iStatpoint - other.iStatpoint
			);
		}
	}PLAYER_INFO;


private:

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
public: // 헬보스와 관련된 
	void KnockBack(_float3 vDir, _float fPower);
private:
	_float3 m_vKnockBackDir = { 0.f, 0.f, 0.f }; // 밀치는 방향
	_float  m_fKnockBackPower = 0.f;             // 밀침 힘
	_bool   m_bKnockBack = false;                // 밀림 여부

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

	// 최대체력 증가
	void Add_HP(_int Hp); // 현재체력 증가
	void Add_MP(_int Mp);
	
	void Add_SkillPoint(_int SkillPoint); // 스킬포인트 증가
	
	_bool Add_Sprit(_int Sprit); // 정신력 증가
	_bool Add_Strength(_int Str);  // 근접무기 전용 데미지 증가
	_bool Add_MaxHP(_int Hp); //최대체력증가
	_bool Add_Capacity(_int type); //원거리무기군 최대탄약증가

	void Add_Exp(_int Exp); // 경험치 증가

	_bool TryUseMana(_int amount) // 스태프 마나 고갈 체크
	{
		if (m_iPlayerMP.first < amount)
			return false;

		m_iPlayerMP.first -= amount;
		Notify(m_iPlayerMP.first, L"MP");
		return true;
	}
	
	_bool Add_Skill(const _wstring& stSkillName)
	{
		if (m_setSkill.find(stSkillName) != m_setSkill.end()) return false;

		m_setSkill.insert(stSkillName);
		return true;
	}

	_bool Has_Skill(const _wstring& stSkillName)
	{
		return m_setSkill.find(stSkillName) != m_setSkill.end();
	}


	void Add_Weapon(const _wstring& stWeaponTag);


public:
	void Set_DoubleAmmoGain(_bool bEnable) { m_bDoubleAmmoGain = bEnable; }
	void Set_DoubleSpeedGain() 
	{ 
	     m_fSpeed = 1.3f; 
	}
	 _bool Has_Item(const _wstring& stItemTag);
	 HRESULT Add_Item(const _wstring& stItemTag);

	 pair<_int, _int> Get_PlayerHp() { return m_iPlayerHP; }
	 pair<_int, _int> Get_PlayerMp() { return m_iPlayerMP; }
	 CWeapon_Base* Get_Current_Weapon() { return m_pPlayer_Weapon; }
	 LEVEL Get_ClearLevel()const { return m_eClearLevel; }
	 void Set_ClearLevel(LEVEL eLevel) { m_eClearLevel = eLevel; }
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
	void OnNotify(void* pArg, const wstring& type) override;

private:
	_float			m_fShakeTime = {};
	_bool m_bTimeControl = { false };
	_float m_fSaveTime = {};
	LONG			m_lMiddlePointX = { g_iWinSizeX / 2 };
	_bool m_bInputLocked{ false };

private: // 플레이어 관련
	pair<_int, _int> m_iPlayerHP{};    // 플레이어 현재/최대체력
	pair<_int, _int> m_iPlayerMP{};    // ``  현재/최대마나
	pair<_int, _int> m_iPlayerEXP{};   //`` 현재/최대경험치

	CWeapon_Base* m_pPlayer_Weapon = { nullptr };
	CInventory* m_pPlayer_Inven = { nullptr };

	LEVEL m_eClearLevel{ LEVEL_END };

	_float m_fAvoidTime = { 0.f };

	PLAYER_INFO m_vPlayerInfo{};
	PLAYER_INFO m_vOriginPlayerInfo{};
	unordered_set<_wstring> m_setSkill; // 스킬 보유용
public:
	const auto& Get_PlayerInfo() const { return m_vPlayerInfo; }
	//_uint Get_StatPoint() { return m_iStatpoint; }
	//_uint Get_SkillPoint() { return m_iSkillpoint; }
	//_uint Get_Level() { return m_iLevel; }
	void Use_SkillPoint(_uint amount);

};


END