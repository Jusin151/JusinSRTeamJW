#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Ranged_Weapon.h"
#include "Image_Manager.h"


BEGIN(Engine)
class CLight;
class CTexture;
class CTransform;
class CSound_Source;
class CVIBuffer_Rect;  // UI는 사각형으로
class CParticle_System;
END

BEGIN(Client)
class CMinigun final : public CRanged_Weapon
{
	enum class State 
	{
		Idle,
		Loop,
		Firing,
		SpinDown
	};
private:
	CMinigun(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMinigun(const CMinigun& Prototype);
	virtual ~CMinigun() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);
private:
	HRESULT Ready_Components();
public:
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Attack(_float fTimeDelta);
	void Attack_WeaponSpecific(_float fTimeDelta) override;
	void Late_Update(_float fTimeDelta);
public:
	HRESULT Render();
public:
	HRESULT On_Collision();
private:
	HRESULT CreateBulletShell();
	 
private:
	CLight*				m_pLightCom			= { nullptr };
	CParticle_System*	m_pParticleCom		= { nullptr };
private:
	State m_eState = State::Idle;
	Weapon_DESC m_Staff_INFO{};
private: 
	_float m_fSpinSpeed = {};
	const _float m_fSpinDecreaseRate = { 0.2f }; // 초당 몇초 감소하게 할껀지
	_float m_fHoldTime = {};
public:
	static CMinigun* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) ;
	virtual void Free();
	
};
END