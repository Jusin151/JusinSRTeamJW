#pragma once

#include "Client_Defines.h"
#include "Weapon_Base.h"
#include "LandObject.h"
#include "Inventory.h"
#include "Item_Manager.h"
#include "CollisionObject.h"


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
	void Move(_float fTimeDelta);
	CTransform* Get_TransForm() { return m_pTransformCom; }

public:
	inline void Add_Ammo(_int iAmmo);
	void Set_Hp(_int iHp) override;
	void Set_Ap(_int iAp)override { m_iAp = iAp; }

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

private: // 인벤관련
	void Input_WeapontoInven(wstring tag,_uint type);
	void Input_ItemtoInven();


	_bool m_bInven_Render_State{};

	void Equip(_float fTimeDelta);
	void UnEquip(_float fTimeDelta);

private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();
private:
	_float			m_fShakeTime = {};
	_bool m_bTimeControl = { false };
	_float m_fSaveTime = {};
	LONG			m_lMiddlePointX = {g_iWinSizeX / 2};
	_float3			m_vOldPos = {};
	_bool m_bPlayerHP_init= { false };
	_bool m_bPlayerMP_init = {false };
public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
public:
	void Taimu_S_to_pu(){m_bTimeControl = !m_bTimeControl;}


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