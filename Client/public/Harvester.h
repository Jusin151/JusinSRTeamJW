#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Ranged_Weapon.h"

BEGIN(Engine)
class CLight;
class CTexture;
class CTransform;
class CSound_Source;
class CVIBuffer_Rect;  // UI는 사각형으로
class CParticle_System;
END

BEGIN(Client)
class CHarvester final : public CRanged_Weapon
{
	enum class State
	{
		Idle,
		Firing,
		Reloading
	};
private:
	CHarvester(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHarvester(const CHarvester& Prototype);
	virtual ~CHarvester() = default;
	
public:
	 HRESULT Initialize_Prototype();
	 HRESULT Initialize(void* pArg);
private:
	HRESULT Ready_Components();
public:
	 void Priority_Update(_float fTimeDelta);
	 void Update(_float fTimeDelta);
	 void Attack(_float fTimeDelta);
	 void Late_Update(_float fTimeDelta);
public:
	 HRESULT Render();
	 HRESULT On_Collision();

	 virtual void Reset() override
	 {
		 m_eState = State::Idle;
		 m_iCurrentFrame = 0;
		 m_fElapsedTime = 0.0f;
	 }

private:
	Weapon_DESC m_Staff_INFO{};
	State m_eState = State::Idle;
	_float t = {}; //
	_float speed = { 0.1f }; //
	_float2 m_vInitialPos = {};
	CSound_Source*	m_pSoundCom = { nullptr };
	CLight*			m_pLightCom = { nullptr };
public:
	static CHarvester* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) ;
	virtual void Free();

private:
	




	// CRanged_Weapon을(를) 통해 상속됨
	void Attack_WeaponSpecific(_float fTimeDelta) override;

};
END