#include "Collider.h"

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
{
}

CCollider::CCollider(const CCollider& Prototype)
	: CComponent(Prototype)
{
}

HRESULT CCollider::Initialize_Prototype()
{
	D3DXMatrixIdentity(&m_WorldMatrix);
	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	D3DXMatrixIdentity(&m_WorldMatrix);
	return S_OK;
}

HRESULT CCollider::Render()
{
	return S_OK;
}


void CCollider::Free()
{
	__super::Free();
}
