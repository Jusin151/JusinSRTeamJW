#pragma once

#include "Client_Defines.h"
#include "Weapon_Base.h"
#include "LandObject.h"
#include "Inventory.h"
#include "Item_Manager.h"
#include "CollisionObject.h"
#include "Observer.h"
#include "UI_Manager.h"


BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Cube;
class CCollider_Sphere;
class CCollider_Cube;
END

BEGIN(Client)

class CPlayer final : public CCollisionObject
{

	
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
			obs->OnNotify(value, type);
	}
	void Set_Hp(_int iHp);
	void Set_Ap(_int iAp)override { m_iAp = iAp; }
	inline void Add_Ammo(_int iAmmo);
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
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
private:
	POINT m_ptOldMousePos = {};
	_float m_fMouseSensor = {};
private:
	CGameObject* m_pCamera = nullptr;
	CCollider_Cube* m_pColliderCom = { nullptr };
public:
	void Taimu_S_to_pu() { m_bTimeControl = !m_bTimeControl; }

private:
	_float			m_fShakeTime = {};
	_bool m_bTimeControl = { false };
	_float m_fSaveTime = {};
	LONG			m_lMiddlePointX = {g_iWinSizeX / 2};
	_float3			m_vOldPos = {};
private: // 플레이어 관련
	pair<_uint, _uint> m_iPlayerHP{};    // 플레이어 현재/최대체력
	pair<_uint, _uint> m_iPlayerMP{};    // ``  현재/최대마나
	pair<_uint, _uint> m_iPlayerEXP{};   //`` 현재/최대경험치
	_uint iStr{}; // 근력
	_uint iLife{};//생명력
	_uint iSprit{};//정신력
	_uint iCapacity{};//용량
	_uint iBullet{};//총알 , 임시임 웨폰에서 할께
	CWeapon_Base* m_pPlayer_Weapon={nullptr};
	CInventory*   m_pPlayer_Inven={nullptr};


	
};


END