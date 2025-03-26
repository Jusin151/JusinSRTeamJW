#include "CollisionObject.h"

CCollisionObject::CCollisionObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CCollisionObject::CCollisionObject(const CCollisionObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CCollisionObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollisionObject::Initialize(void* pArg)
{
	return S_OK;
}

void CCollisionObject::Priority_Update(_float fTimeDelta)
{
}

void CCollisionObject::Update(_float fTimeDelta)
{
}

void CCollisionObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CCollisionObject::Render()
{
	return S_OK;
}

void CCollisionObject::Take_Damage(CCollisionObject* other)
{

	if (other == nullptr)
		return;

	if (Get_Damage() >= 0)
		other->Set_Hp(other->Get_Hp() - Get_Damage());


}

void CCollisionObject::Free()
{
	__super::Free();
}
