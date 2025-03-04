#include "VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer { pGraphic_Device }
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{

#pragma region VERTEX_BUFFER
	m_iNumVertices = 6;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iFVF = D3DFVF_XYZ | D3DFVF_TEX1/* | D3DFVF_TEXCOORDSIZE2(0)*/;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	VTXPOSTEX* pVertices = { nullptr };


	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, reinterpret_cast<void**>(&pVertices), 0);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 0.f);

	pVertices[4].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[4].vTexcoord = _float2(1.f, 1.f);

	pVertices[5].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[5].vTexcoord = _float2(0.f, 1.f);

	m_pVB->Unlock();
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Rect* CVIBuffer_Rect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect::Clone(void* pArg)
{
	CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect::Free()
{
	__super::Free();


}
