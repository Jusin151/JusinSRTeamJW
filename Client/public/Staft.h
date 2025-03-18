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
class CStaft final : public CRanged_Weapon
{
	enum class State 
	{
		Idle,
		Charging, 
		Charged,
		Firing
	};
private:
	CStaft(LPDIRECT3DDEVICE9 pGraphic_Device);
	CStaft(const CStaft& Prototype);
	virtual ~CStaft() = default;

public:
	 HRESULT Initialize_Prototype();
	 HRESULT Initialize(void* pArg);
	 void Priority_Update(_float fTimeDelta);
	 void Update(_float fTimeDelta);
	 void Attack(_float fTimeDelta);
	 void Late_Update(_float fTimeDelta);
	 HRESULT Render();
	 Weapon_DESC m_Staft_INFO{};

	 HRESULT On_Collision();

private:
	HRESULT Ready_Components();

private:
	State m_eState = State::Idle;

public:
	static CStaft* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) ;
	virtual void Free();

	_float t = {}; //
	_float speed = { 0.1f }; //
	_float2 m_vInitialPos = {};

};
END