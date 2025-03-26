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
	void Inven_Update(_float fTimeDelta);
	_bool m_bInven_Render_State{};
private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();
private:
	pair<_uint, _uint> m_iPlayerHP{};    // 플레이어 현재/최대체력
	pair<_uint, _uint> m_iPlayerMP{};    // ``  현재/최대마나
	pair<_uint, _uint> m_iPlayerBullet{};//`` 현재/최대총알
	pair<_uint, _uint> m_iPlayerEXP{};   //`` 현재/최대경험치

	_float			m_fShakeTime = {};

	LONG			m_lMiddlePointX = {g_iWinSizeX / 2};

	_float3			m_vOldPos = {};

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	CInventory* m_pInven = { nullptr };
	virtual void Free();
	
};


END