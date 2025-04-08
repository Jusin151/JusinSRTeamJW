#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Ranged_Weapon.h"


BEGIN(Engine)
class CLight;
class CTexture;
class CTransform;
class CSound_Source;
class CVIBuffer_Rect;  
class CParticle_System;
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
private:
	HRESULT Ready_Components();
public:
	 void Priority_Update(_float fTimeDelta);
	 void Update(_float fTimeDelta);
	 void Late_Update(_float fTimeDelta);
public:
	 HRESULT Render();
public: //공격 관련
	void Attack_WeaponSpecific(_float fTimeDelta);
	void Attack(_float fTimeDelta);


private:
	State m_eState = State::Idle;
	CLight* m_pLightCom = { nullptr };

public:
	static CShotGun* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};
END