#include "Ranged_Weapon.h"

CRanged_Weapon::CRanged_Weapon(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CWeapon_Base{pGraphic_Device}
{
} 

CRanged_Weapon::CRanged_Weapon(const CRanged_Weapon& Prototype)
	: CWeapon_Base(Prototype)
{
}

HRESULT CRanged_Weapon::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CRanged_Weapon::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CRanged_Weapon::Priority_Update(_float fTimeDelta)
{
}

void CRanged_Weapon::Update(_float fTimeDelta)
{
}

void CRanged_Weapon::Late_Update(_float fTimeDelta)
{
}

HRESULT CRanged_Weapon::Render()
{
	return E_NOTIMPL;
}

HRESULT CRanged_Weapon::Ready_Components()
{
	return E_NOTIMPL;
}


void CRanged_Weapon::Attack()
{
}

CGameObject* CRanged_Weapon::Clone(void* pArg)
{
	CRanged_Weapon* pInstace = new CRanged_Weapon(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("근접무기 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CRanged_Weapon::Free()
{
	__super::Free();
}