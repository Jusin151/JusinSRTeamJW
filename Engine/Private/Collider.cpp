#include "Collider.h"

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
{
}

CCollider::CCollider(const CCollider& Prototype)
	: CComponent{ Prototype }
	, m_pIB{ Prototype.m_pIB }
	, m_iNumIndices{ Prototype.m_iNumIndices }
	, m_iIndexStride{ Prototype.m_iIndexStride }
	, m_iNumLine{ Prototype.m_iNumLine }
{
	Safe_AddRef(m_pIB);
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

HRESULT CCollider::Create_IndexBuffer()
{
	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iNumIndices * m_iIndexStride,
		0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, 0)))
		return E_FAIL;

	return S_OK;
}

void CCollider::Free()
{
	__super::Free();
	Safe_Release(m_pIB);
}
