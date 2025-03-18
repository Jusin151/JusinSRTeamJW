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
class CShutGun final : public CRanged_Weapon
{
	enum class State
	{
		Idle,
		Charged,
		Firing
	};
private:
	CShutGun(LPDIRECT3DDEVICE9 pGraphic_Device);
	CShutGun(const CShutGun& Prototype);
	virtual ~CShutGun() = default;

public:
	 HRESULT Initialize_Prototype();
	 HRESULT Initialize(void* pArg);
	 void Priority_Update(_float fTimeDelta);
	 void Update(_float fTimeDelta);
	 void Attack(_float fTimeDelta);
	 void Late_Update(_float fTimeDelta);
	 HRESULT Render();
	 Weapon_DESC m_ShotGun_INFO{};

	 HRESULT On_Collision();

private:
	HRESULT Ready_Components();



public:
	static CShutGun* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) ;
	virtual void Free();

	_float t = {}; //
	_float speed = { 0.1f }; //
	_float2 m_vInitialPos = {};

};
END