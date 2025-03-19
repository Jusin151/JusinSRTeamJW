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
class CShotGun final : public CRanged_Weapon
{
private:
	enum class State 
	{
		Idle,
		Firing,
		Reloading
	};

private:
	CShotGun(LPDIRECT3DDEVICE9 pGraphic_Device);
	CShotGun(const CShotGun& Prototype);
	virtual ~CShotGun() = default;

public:
	 HRESULT Initialize_Prototype();
	 HRESULT Initialize(void* pArg);
	 void Priority_Update(_float fTimeDelta);
	 void Update(_float fTimeDelta);
	 void Attack(_float fTimeDelta);
	 void Late_Update(_float fTimeDelta);
	 HRESULT Render();

	 HRESULT On_Collision();

private:
	HRESULT Ready_Components();



public:
	static CShotGun* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) ;
	virtual void Free();
private:
	State m_eState = State::Idle;
	Weapon_DESC m_ShotGun_INFO{};
	_float t = {};
	_float speed = { 0.1f };
	_float2 m_vInitialPos = {};
};
END