#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Ranged_Weapon.h"
#include "Image_Manager.h"


BEGIN(Engine)
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

public:
	 HRESULT Initialize_Prototype();
	 HRESULT Initialize(void* pArg);
	 void Priority_Update(_float fTimeDelta);
	 void Update(_float fTimeDelta);
	 void Attack(_float fTimeDelta);
	 void Late_Update(_float fTimeDelta);
	 HRESULT Render();
	 Weapon_DESC m_Staff_INFO{};

	 HRESULT On_Collision();

	 HRESULT Ready_Components();



private:
	State m_eState = State::Idle;

public:
	static CStaff* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) ;
	virtual void Free();






	// CRanged_Weapon을(를) 통해 상속됨
	HRESULT Ready_Icon() override;


	// CRanged_Weapon을(를) 통해 상속됨
	void Attack_WeaponSpecific(_float fTimeDelta) override;

};
END