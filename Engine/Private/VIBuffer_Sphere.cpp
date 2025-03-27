#include "VIBuffer_Sphere.h"

CVIBuffer_Sphere::CVIBuffer_Sphere(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer { pGraphic_Device }
{
}

CVIBuffer_Sphere::CVIBuffer_Sphere(const CVIBuffer_Sphere& Prototype)
	: CVIBuffer{ Prototype }
	, m_fRadius{ Prototype.m_fRadius }
	, m_iSlices{ Prototype.m_iSlices }
	, m_iStacks{ Prototype.m_iStacks }
{
}


HRESULT CVIBuffer_Sphere::Initialize_Prototype()
{
	m_fRadius = 1.0f;
	m_iSlices = 32;
	m_iStacks = 16;
	m_iNumVertices = (m_iSlices + 1) * (m_iStacks + 1);
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1; //| D3DFVF_TEXCOORDSIZE3(0);
	m_iNumPritimive = m_iSlices * m_iStacks * 2;
	m_pVertexPositions = new _float3[m_iNumVertices];
	m_iIndexStride = 2;
	m_iNumIndices = m_iNumPritimive * 3;
	m_eIndexFormat = D3DFMT_INDEX16;

#pragma region VERTEX_BUFFER
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	VTXNORTEX* pVertices = { nullptr };
	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, reinterpret_cast<void**>(&pVertices), 0);
	for (int stack = 0; stack <= m_iStacks; ++stack)
	{
		float theta = (float)stack / m_iStacks * D3DX_PI - D3DX_PI / 2.0f; // -PI/2 ~ PI/2 (위도)
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);
		for (int slice = 0; slice <= m_iSlices; ++slice)
		{
			float phi = (float)slice / m_iSlices * D3DX_PI * 2; // 0 ~ 2PI (경도)
			float sinPhi = sinf(phi);
			float cosPhi = cosf(phi);
			int vertexIndex = stack * (m_iSlices + 1) + slice;


			// 위치 계산
			m_pVertexPositions[vertexIndex].x = pVertices[vertexIndex].vPosition.x = m_fRadius * cosTheta * cosPhi;
			m_pVertexPositions[vertexIndex].y = pVertices[vertexIndex].vPosition.y = m_fRadius * sinTheta;
			m_pVertexPositions[vertexIndex].z = pVertices[vertexIndex].vPosition.z = m_fRadius * cosTheta * sinPhi;

			// 법선 계산 (구의 중심에서 정점을 향하는 벡터)
			pVertices[vertexIndex].vNormal = pVertices[vertexIndex].vPosition;
			D3DXVec3Normalize(&pVertices[vertexIndex].vNormal, &pVertices[vertexIndex].vNormal);

			// 텍스처 좌표 (선택 사항)
			pVertices[vertexIndex].vTexcoord.x = (float)slice / m_iSlices;
			pVertices[vertexIndex].vTexcoord.y = (float)stack / m_iStacks;
		}
	}
	m_pVB->Unlock();
#pragma endregion

#pragma region INDEX_BUFFER
	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	_ushort* pIndices = { nullptr };

	m_pIB->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	int index = 0;
	for (int stack = 0; stack < m_iStacks; ++stack)
	{
		for (int slice = 0; slice < m_iSlices; ++slice)
		{
			// Calculate indices for the two triangles forming a quad
			int p0 = stack * (m_iSlices + 1) + slice;
			int p1 = p0 + 1;
			int p2 = (stack + 1) * (m_iSlices + 1) + slice;
			int p3 = p2 + 1;

			//int index = (stack * (m_iSlices + 1) + slice) * 6;
			// First triangle
			pIndices[index++] = p0;
			pIndices[index++] = p2;
			pIndices[index++] = p1;

			// Second triangle
			pIndices[index++] = p1;
			pIndices[index++] = p2;
			pIndices[index++] = p3;
		}
	}

	m_pIB->Unlock();
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Sphere::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Sphere* CVIBuffer_Sphere::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CVIBuffer_Sphere* pInstance = new CVIBuffer_Sphere(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Sphere::Clone(void* pArg)
{
	CVIBuffer_Sphere* pInstance = new CVIBuffer_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Sphere::Free()
{
	__super::Free();

}
