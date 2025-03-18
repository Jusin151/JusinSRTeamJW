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
	COL_DESC* tDesc = {};
	tDesc = static_cast<COL_DESC*>(pArg);

	m_fLocalPos = tDesc->fLocalPos;
	m_fScale = tDesc->fScale;
	m_pOwner = tDesc->pOwner;
	m_eType = tDesc->eType;


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
