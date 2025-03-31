#include "VIBuffer_TexturedCube.h"

CVIBuffer_TexturedCube::CVIBuffer_TexturedCube(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer{ pGraphic_Device }
{
}

CVIBuffer_TexturedCube::CVIBuffer_TexturedCube(const CVIBuffer_TexturedCube& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_TexturedCube::Initialize_Prototype()
{
	// 각 면별로 4개의 정점을 사용 (총 24개 정점)
	m_iNumVertices = 24;
	m_iVertexStride = sizeof(VTXCUBE);
	m_iFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);
	m_iNumPritimive = 12;

	m_iIndexStride = 2;
	m_iNumIndices = 36;
	m_eIndexFormat = D3DFMT_INDEX16;

#pragma region VERTEX_BUFFER
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	VTXCUBE* pVertices = { nullptr };

	m_pVertexPositions = new _float3[m_iNumVertices];

	m_pVB->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

	// 전면 (Front Face: -Z)
	// 왼쪽 위
	m_pVertexPositions[0] = pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexcoord = _float3(0.0f, 0.0f, 0.0f);
	pVertices[0].vNormal = _float3(0.0f, 0.0f, -1.0f);

	// 오른쪽 위
	m_pVertexPositions[1] = pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexcoord = _float3(1.0f, 0.0f, 0.0f);
	pVertices[1].vNormal = _float3(0.0f, 0.0f, -1.0f);

	// 오른쪽 아래
	m_pVertexPositions[2] = pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexcoord = _float3(1.0f, 1.0f, 0.0f);
	pVertices[2].vNormal = _float3(0.0f, 0.0f, -1.0f);

	// 왼쪽 아래
	m_pVertexPositions[3] = pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexcoord = _float3(0.0f, 1.0f, 0.0f);
	pVertices[3].vNormal = _float3(0.0f, 0.0f, -1.0f);

	// 후면 (Back Face: +Z)
	// 왼쪽 위
	m_pVertexPositions[4] = pVertices[4].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[4].vTexcoord = _float3(0.0f, 0.0f, 0.0f);
	pVertices[4].vNormal = _float3(0.0f, 0.0f, 1.0f);

	// 오른쪽 위
	m_pVertexPositions[5] = pVertices[5].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[5].vTexcoord = _float3(1.0f, 0.0f, 0.0f);
	pVertices[5].vNormal = _float3(0.0f, 0.0f, 1.0f);

	// 오른쪽 아래
	m_pVertexPositions[6] = pVertices[6].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[6].vTexcoord = _float3(1.0f, 1.0f, 0.0f);
	pVertices[6].vNormal = _float3(0.0f, 0.0f, 1.0f);

	// 왼쪽 아래
	m_pVertexPositions[7] = pVertices[7].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[7].vTexcoord = _float3(0.0f, 1.0f, 0.0f);
	pVertices[7].vNormal = _float3(0.0f, 0.0f, 1.0f);

	// 왼쪽면 (Left Face: -X)
	// 왼쪽 위
	m_pVertexPositions[8] = pVertices[8].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[8].vTexcoord = _float3(0.0f, 0.0f, 0.0f);
	pVertices[8].vNormal = _float3(-1.0f, 0.0f, 0.0f);

	// 오른쪽 위
	m_pVertexPositions[9] = pVertices[9].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[9].vTexcoord = _float3(1.0f, 0.0f, 0.0f);
	pVertices[9].vNormal = _float3(-1.0f, 0.0f, 0.0f);

	// 오른쪽 아래
	m_pVertexPositions[10] = pVertices[10].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[10].vTexcoord = _float3(1.0f, 1.0f, 0.0f);
	pVertices[10].vNormal = _float3(-1.0f, 0.0f, 0.0f);

	// 왼쪽 아래
	m_pVertexPositions[11] = pVertices[11].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[11].vTexcoord = _float3(0.0f, 1.0f, 0.0f);
	pVertices[11].vNormal = _float3(-1.0f, 0.0f, 0.0f);

	// 오른쪽면 (Right Face: +X)
	// 왼쪽 위
	m_pVertexPositions[12] = pVertices[12].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[12].vTexcoord = _float3(0.0f, 0.0f, 0.0f);
	pVertices[12].vNormal = _float3(1.0f, 0.0f, 0.0f);

	// 오른쪽 위
	m_pVertexPositions[13] = pVertices[13].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[13].vTexcoord = _float3(1.0f, 0.0f, 0.0f);
	pVertices[13].vNormal = _float3(1.0f, 0.0f, 0.0f);

	// 오른쪽 아래
	m_pVertexPositions[14] = pVertices[14].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[14].vTexcoord = _float3(1.0f, 1.0f, 0.0f);
	pVertices[14].vNormal = _float3(1.0f, 0.0f, 0.0f);

	// 왼쪽 아래
	m_pVertexPositions[15] = pVertices[15].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[15].vTexcoord = _float3(0.0f, 1.0f, 0.0f);
	pVertices[15].vNormal = _float3(1.0f, 0.0f, 0.0f);

	// 윗면 (Top Face: +Y)
	// 왼쪽 위
	m_pVertexPositions[16] = pVertices[16].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[16].vTexcoord = _float3(0.0f, 0.0f, 0.0f);
	pVertices[16].vNormal = _float3(0.0f, 1.0f, 0.0f);

	// 오른쪽 위
	m_pVertexPositions[17] = pVertices[17].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[17].vTexcoord = _float3(1.0f, 0.0f, 0.0f);
	pVertices[17].vNormal = _float3(0.0f, 1.0f, 0.0f);

	// 오른쪽 아래
	m_pVertexPositions[18] = pVertices[18].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[18].vTexcoord = _float3(1.0f, 1.0f, 0.0f);
	pVertices[18].vNormal = _float3(0.0f, 1.0f, 0.0f);

	// 왼쪽 아래
	m_pVertexPositions[19] = pVertices[19].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[19].vTexcoord = _float3(0.0f, 1.0f, 0.0f);
	pVertices[19].vNormal = _float3(0.0f, 1.0f, 0.0f);

	// 아랫면 (Bottom Face: -Y)
	// 왼쪽 위
	m_pVertexPositions[20] = pVertices[20].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[20].vTexcoord = _float3(0.0f, 0.0f, 0.0f);
	pVertices[20].vNormal = _float3(0.0f, -1.0f, 0.0f);

	// 오른쪽 위
	m_pVertexPositions[21] = pVertices[21].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[21].vTexcoord = _float3(1.0f, 0.0f, 0.0f);
	pVertices[21].vNormal = _float3(0.0f, -1.0f, 0.0f);

	// 오른쪽 아래
	m_pVertexPositions[22] = pVertices[22].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[22].vTexcoord = _float3(1.0f, 1.0f, 0.0f);
	pVertices[22].vNormal = _float3(0.0f, -1.0f, 0.0f);

	// 왼쪽 아래
	m_pVertexPositions[23] = pVertices[23].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[23].vTexcoord = _float3(0.0f, 1.0f, 0.0f);
	pVertices[23].vNormal = _float3(0.0f, -1.0f, 0.0f);

	m_pVB->Unlock();
#pragma endregion

#pragma region INDEX_BUFFER
	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	_ushort* pIndices = { nullptr };

	m_pIB->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	// 전면 (Front Face: -Z)
	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
	pIndices[3] = 0; pIndices[4] = 2; pIndices[5] = 3;

	// 후면 (Back Face: +Z)
	pIndices[6] = 4; pIndices[7] = 5; pIndices[8] = 6;
	pIndices[9] = 4; pIndices[10] = 6; pIndices[11] = 7;

	// 왼쪽면 (Left Face: -X)
	pIndices[12] = 8; pIndices[13] = 9; pIndices[14] = 10;
	pIndices[15] = 8; pIndices[16] = 10; pIndices[17] = 11;

	// 오른쪽면 (Right Face: +X)
	pIndices[18] = 12; pIndices[19] = 13; pIndices[20] = 14;
	pIndices[21] = 12; pIndices[22] = 14; pIndices[23] = 15;

	// 윗면 (Top Face: +Y)
	pIndices[24] = 16; pIndices[25] = 17; pIndices[26] = 18;
	pIndices[27] = 16; pIndices[28] = 18; pIndices[29] = 19;

	// 아랫면 (Bottom Face: -Y)
	pIndices[30] = 20; pIndices[31] = 21; pIndices[32] = 22;
	pIndices[33] = 20; pIndices[34] = 22; pIndices[35] = 23;

	m_pIB->Unlock();
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_TexturedCube::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_TexturedCube* CVIBuffer_TexturedCube::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CVIBuffer_TexturedCube* pInstance = new CVIBuffer_TexturedCube(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_TexturedCube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_TexturedCube::Clone(void* pArg)
{
	CVIBuffer_TexturedCube* pInstance = new CVIBuffer_TexturedCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_TexturedCube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_TexturedCube::Free()
{
	__super::Free();
}