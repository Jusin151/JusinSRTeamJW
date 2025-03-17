#include "Melee_Weapon.h"
#include "GameObject.h"

CMelee_Weapon::CMelee_Weapon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CWeapon_Base{pGraphic_Device}
{
}

CMelee_Weapon::CMelee_Weapon(const CMelee_Weapon& Prototype)
	: CWeapon_Base(Prototype)
{
}

HRESULT CMelee_Weapon::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CMelee_Weapon::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CMelee_Weapon::Priority_Update(_float fTimeDelta)
{
}

void CMelee_Weapon::Update(_float fTimeDelta)
{
}

void CMelee_Weapon::Late_Update(_float fTimeDelta)
{
}

HRESULT CMelee_Weapon::Render()
{
	return E_NOTIMPL;
}

HRESULT CMelee_Weapon::Ready_Components()
{
	return E_NOTIMPL;
}

void CMelee_Weapon::Free()
{
}

CGameObject* CMelee_Weapon::Clone(void* pArg)
{
	return nullptr;
}

void CMelee_Weapon::Attack()
{
}
