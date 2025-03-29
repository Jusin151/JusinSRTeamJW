#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Melee_Weapon.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)
class CClaymore final : public CMelee_Weapon
{
private:
	CClaymore(LPDIRECT3DDEVICE9 pGraphic_Device);
	CClaymore(const CClaymore& Prototype);
	virtual ~CClaymore() = default;

public:
	 HRESULT Initialize_Prototype();
	 HRESULT Initialize(void* pArg);
	 void Priority_Update(_float fTimeDelta);
	 void Update(_float fTimeDelta);
	 void Late_Update(_float fTimeDelta);
	 HRESULT Render();
	virtual HRESULT On_Collision(CCollisionObject* other);
private:
	HRESULT Ready_Components();
public:
	static CClaymore* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) ;
	virtual void Free();
	void Attack(_float fTimeDelta);
private:
	Weapon_DESC m_Claymore_INFO{};

	// CMelee_Weapon을(를) 통해 상속됨
	void Attack_WeaponSpecific(_float fTimeDelta) override;



	// CMelee_Weapon을(를) 통해 상속됨
	HRESULT Ready_Icon() override;

};
END