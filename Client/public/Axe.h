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
class CAxe final : public CMelee_Weapon
{
private:
	CAxe(LPDIRECT3DDEVICE9 pGraphic_Device);
	CAxe(const CAxe& Prototype);
	virtual ~CAxe() = default;

public:
	 HRESULT Initialize_Prototype();
	 HRESULT Initialize(void* pArg);
	 void Priority_Update(_float fTimeDelta);
	 void Update(_float fTimeDelta);
	 void Late_Update(_float fTimeDelta);
	 HRESULT Render();
	 Weapon_DESC m_Axe_INFO{};

	 virtual HRESULT On_Collision(CCollisionObject* other);

private:
	HRESULT Ready_Components();


public:
	static CAxe* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) ;
	virtual void Free();

	void Attack(_float fTimeDelta);

};
END