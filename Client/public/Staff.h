#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Ranged_Weapon.h"
#include "Image_Manager.h"


BEGIN(Engine)
class CLgiht;
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END


BEGIN(Client)
class CStaff final : public CRanged_Weapon
{
	enum class State 
	{
		Idle,
		Charging, 
		Charged,
		Firing
	};

private:
	CStaff(LPDIRECT3DDEVICE9 pGraphic_Device);
	CStaff(const CStaff& Prototype);
	virtual ~CStaff() = default;
private:
	HRESULT Ready_Components();
public:
	 HRESULT Initialize_Prototype();
	 HRESULT Initialize(void* pArg);
public:
	 void Priority_Update(_float fTimeDelta);
	 void Update(_float fTimeDelta);
	 void Attack(_float fTimeDelta);
	 void Attack_WeaponSpecific(_float fTimeDelta) override;
	 void Late_Update(_float fTimeDelta);

public:
	 HRESULT Render();
	 Weapon_DESC m_Staff_INFO{};
	 HRESULT On_Collision();

private:
	CLight* m_pLightCom = { nullptr };
	State	m_eState = State::Idle;
	_bool m_bFireLock = { false };
	_bool m_bCharged = { false };

public:
	static CStaff* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) ;
	virtual void Free();

};
END