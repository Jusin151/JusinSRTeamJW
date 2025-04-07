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
class CMagnum final : public CRanged_Weapon
{
	enum class State
	{
		Idle,
		Firing,
		Wait
	};

private:
	CMagnum(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMagnum(const CMagnum& Prototype);
	virtual ~CMagnum() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Attack(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT Render();
	Weapon_DESC m_Magnum_INFO{};

private:
	HRESULT Ready_Components();
private:
	State m_eState = State::Idle;private:
	CSound_Source*	m_pSoundCom = { nullptr };
	CLight*			m_pLightCom = { nullptr };

public:
	static CMagnum* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free();


	 bool m_bHasFired = false;


	
	 // CRanged_Weapon을(를) 통해 상속됨
	 void Attack_WeaponSpecific(_float fTimeDelta) override;

};
END