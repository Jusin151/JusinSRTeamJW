#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Ranged_Weapon.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)
class CSonic final : public CRanged_Weapon
{
	enum class State 
	{
		Idle,
		Charging, 
		Charged,
		Firing
	};
private:
	CSonic(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSonic(const CSonic& Prototype);
	virtual ~CSonic() = default;

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

	 virtual void Reset() override
	 {
		 m_eState = State::Idle;
 		 m_iCurrentFrame = 0;
		 m_fElapsedTime = 0.0f;
	 }

private:
	State m_eState = State::Idle;

public:
	static CSonic* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) ;
	virtual void Free();

	_float t = {}; //
	_float speed = { 0.1f }; //
	_float2 m_vInitialPos = {};




	// CRanged_Weapon을(를) 통해 상속됨
	void Attack_WeaponSpecific(_float fTimeDelta) override;

};
END