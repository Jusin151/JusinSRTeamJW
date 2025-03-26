#include "Collider.h"

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
{
}

CCollider::CCollider(const CCollider& Prototype)
	: CComponent{ Prototype }
	, m_WorldMatrix { Prototype.m_WorldMatrix }
	, m_fLocalPos{ Prototype.m_fLocalPos}
	, m_fPos { Prototype.m_fPos}
	, m_fScale { Prototype.m_fScale}
	, m_fMTV { Prototype.m_fMTV }
	, m_fDepth { Prototype.m_fDepth }
	, m_pOwner { Prototype.m_pOwner }
	, m_bIsCollision { Prototype.m_bIsCollision }
	, m_pVB{ Prototype.m_pVB }
	, m_pIB{ Prototype.m_pIB }
	, m_iNumVertices { Prototype.m_iNumVertices }
	, m_iVertexStride{ Prototype.m_iVertexStride }
	, m_iFVF { Prototype.m_iFVF }
	, m_iNumIndices{ Prototype.m_iNumIndices }
	, m_iIndexStride{ Prototype.m_iIndexStride }
	, m_iNumLine{ Prototype.m_iNumLine }
{
	Safe_AddRef(m_pIB);
	Safe_AddRef(m_pVB);
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


	D3DXMatrixIdentity(&m_WorldMatrix);
	return S_OK;
}

HRESULT CCollider::Render()
{
	return S_OK;
}

HRESULT CCollider::Create_VertexBuffer()
{
	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iNumVertices * m_iVertexStride,
		0/*D3DUSAGE_DYNAMIC*/, m_iFVF, D3DPOOL_MANAGED, &m_pVB, 0)))
		return E_FAIL;
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
	Safe_Release(m_pVB);
}
