#include "VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer { pGraphic_Device }
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer& Prototype)
	: CVIBuffer{ Prototype }
{
}


HRESULT CVIBuffer_Cube::Initialize_Prototype()
{
	m_iNumVertices = 8;
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


	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, reinterpret_cast<void**>(&pVertices), 0);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexcoord = pVertices[0].vPosition;
	pVertices[0].vNormal = { 0.f, 0.f, 0.f };

	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexcoord = pVertices[1].vPosition;
	pVertices[1].vNormal = { 0.f, 0.f, 0.f };

	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexcoord = pVertices[2].vPosition;
	pVertices[2].vNormal = { 0.f, 0.f, 0.f };

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexcoord = pVertices[3].vPosition;
	pVertices[3].vNormal = { 0.f, 0.f, 0.f };

	pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vTexcoord = pVertices[4].vPosition;
	pVertices[4].vNormal = { 0.f, 0.f, 0.f };

	pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vTexcoord = pVertices[5].vPosition;
	pVertices[5].vNormal = { 0.f, 0.f, 0.f };

	pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vTexcoord = pVertices[6].vPosition;
	pVertices[6].vNormal = { 0.f, 0.f, 0.f };

	pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vTexcoord = pVertices[7].vPosition;
	pVertices[7].vNormal = { 0.f, 0.f, 0.f };
	
#pragma endregion

#pragma region INDEX_BUFFER
	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	_ushort* pIndices = { nullptr };

	m_pIB->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	/* +x */
	pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

	/* -x */
	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;


	/* +y */
	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;

	/* -y */
	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;

	/* +z */
	pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;

	/* -z */
	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

	m_pIB->Unlock();
	
#pragma endregion
	// 각 면의 노멀 계산 (인덱스 버퍼 기반)
	D3DXVECTOR3 faceNormals[12]; // 12개의 삼각형 면
	for (int i = 0; i < 12; ++i)
	{
		D3DXVECTOR3 v0 = pVertices[pIndices[i * 3]].vPosition;
		D3DXVECTOR3 v1 = pVertices[pIndices[i * 3 + 1]].vPosition;
		D3DXVECTOR3 v2 = pVertices[pIndices[i * 3 + 2]].vPosition;

		D3DXVECTOR3 edge1 = v1 - v0;
		D3DXVECTOR3 edge2 = v2 - v0;
		D3DXVec3Cross(&faceNormals[i], &edge1, &edge2); // 외적
		D3DXVec3Normalize(&faceNormals[i], &faceNormals[i]); // 정규화
	}

	// 각 정점의 노멀 계산 (면 노멀 평균)
	for (int i = 0; i < 8; ++i)
	{
		D3DXVECTOR3 vertexNormal = { 0.f, 0.f, 0.f };
		int numFaces = 0;

		// 정점 i가 사용되는 모든 면을 찾아서 노멀을 더함
		for (int j = 0; j < 12; ++j) // 12 triangles
		{
			if (pIndices[j * 3] == i || pIndices[j * 3 + 1] == i || pIndices[j * 3 + 2] == i)
			{
				vertexNormal += faceNormals[j];
				numFaces++;
			}
		}

		// 평균 계산 및 정규화
		vertexNormal /= (float)numFaces;
		D3DXVec3Normalize(&pVertices[i].vNormal, &vertexNormal);
	}

	m_pVB->Unlock();

	return S_OK;
}

//HRESULT CVIBuffer_Cube::Initialize_Prototype()
//{
//
//	m_iNumVertices = 8;
//	m_iVertexStride = sizeof(VTXPOSTEX);
//	m_iFVF = D3DFVF_XYZ | D3DFVF_TEX1/* | D3DFVF_TEXCOORDSIZE2(0)*/;
//	m_iNumPritimive = 12;
//
//	m_iIndexStride = 2;
//	m_iNumIndices = 36;
//	m_eIndexFormat = D3DFMT_INDEX16; 
//
//#pragma region VERTEX_BUFFER
//	if (FAILED(__super::Create_VertexBuffer()))
//		return E_FAIL;
//
//	VTXPOSTEX* pVertices = { nullptr };
//
//
//	m_pVB->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);
//
//	// 앞면 (z = -0.5)
//	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
//	pVertices[0].vTexcoord = _float2(0.f, 0.f);
//
//	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
//	pVertices[1].vTexcoord = _float2(1.f, 0.f);
//
//	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
//	pVertices[2].vTexcoord = _float2(1.f, 1.f);
//
//	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
//	pVertices[3].vTexcoord = _float2(0.f, 1.f);
//
//	// 뒷면 (z = 0.5)
//	pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
//	pVertices[4].vTexcoord = _float2(0.f, 0.f);
//
//	pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
//	pVertices[5].vTexcoord = _float2(1.f, 0.f);
//
//	pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
//	pVertices[6].vTexcoord = _float2(1.f, 1.f);
//
//	pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
//	pVertices[7].vTexcoord = _float2(0.f, 1.f);
//
//
//	m_pVB->Unlock();
//#pragma endregion
//
//#pragma region INDEX_BUFFER
//	if (FAILED(__super::Create_IndexBuffer()))
//		return E_FAIL;
//
//	_ushort* pIndices = { nullptr };
//
//	m_pIB->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);
//
//	_uint iIndex = 0;
//	// 앞
//	pIndices[iIndex++] = 0;
//	pIndices[iIndex++] = 1;
//	pIndices[iIndex++] = 2;
//
//	pIndices[iIndex++] = 0;
//	pIndices[iIndex++] = 2;
//	pIndices[iIndex++] = 3;
//
//	// 오른쪽
//	pIndices[iIndex++] = 1;
//	pIndices[iIndex++] = 5;
//	pIndices[iIndex++] = 6;
//			 
//	pIndices[iIndex++] = 1;
//	pIndices[iIndex++] = 6;
//	pIndices[iIndex++] = 2;
//
//	// 왼쪽
//	pIndices[iIndex++] = 4;
//	pIndices[iIndex++] = 0;
//	pIndices[iIndex++] = 3;
//
//	pIndices[iIndex++] = 4;
//	pIndices[iIndex++] = 3;
//	pIndices[iIndex++] = 7;
//
//	// 뒤
//	pIndices[iIndex++] = 5;
//	pIndices[iIndex++] = 4;
//	pIndices[iIndex++] = 7;
//
//	pIndices[iIndex++] = 5;
//	pIndices[iIndex++] = 7;
//	pIndices[iIndex++] = 6;
//
//	// 아래
//	pIndices[iIndex++] = 3;
//	pIndices[iIndex++] = 2;
//	pIndices[iIndex++] = 6;
//
//	pIndices[iIndex++] = 3;
//	pIndices[iIndex++] = 6;
//	pIndices[iIndex++] = 7;
//
//	// 위
//	pIndices[iIndex++] = 4;
//	pIndices[iIndex++] = 5;
//	pIndices[iIndex++] = 1;
//
//	pIndices[iIndex++] = 4;
//	pIndices[iIndex++] = 1;
//	pIndices[iIndex++] = 0;
//
//	m_pIB->Unlock();
//#pragma endregion
//
//	return S_OK;
//}
//HRESULT CVIBuffer_Cube::Initialize_Prototype()
//{
//    // 큐브의 각 면은 4개의 버텍스가 필요하므로 (6면 x 4버텍스 = 24)
//    m_iNumVertices = 24;
//    m_iVertexStride = sizeof(VTXPOSTEX); 
//    m_iFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
//    m_iNumPritimive = 12;  // 12개의 삼각형 (6면 x 2삼각형)
//    m_iIndexStride = 2;
//    m_iNumIndices = 36;    // 12개의 삼각형 x 3개의 인덱스
//    m_eIndexFormat = D3DFMT_INDEX16;
//
//#pragma region VERTEX_BUFFER
//    if (FAILED(__super::Create_VertexBuffer()))
//        return E_FAIL;
//
//    VTXPOSTEX* pVertices = { nullptr };
//    m_pVB->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);
//
//    // 앞면 (노말: 0, 0, -1) - 정점 0-3
//    pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);   // 좌상단
//.
//    pVertices[0].vTexcoord = _float2(0.f, 0.f);
//
//    pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);    // 우상단
//    pVertices[1].vTexcoord = _float2(1.f, 0.f);
//
//    pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);   // 우하단
//    pVertices[2].vTexcoord = _float2(1.f, 1.f);
//
//    pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);  // 좌하단
//    pVertices[3].vTexcoord = _float2(0.f, 1.f);
//
//    // 뒷면 (노말: 0, 0, 1) - 정점 4-7
//    pVertices[4].vPosition = _float3(0.5f, 0.5f, 0.5f);    // 우상단
//    pVertices[4].vTexcoord = _float2(0.f, 0.f);
//
//    pVertices[5].vPosition = _float3(-0.5f, 0.5f, 0.5f);   // 좌상단
//    pVertices[5].vNormal = _float3(0.0f, 0.0f, 1.0f);
//    pVertices[5].vTexcoord = _float2(1.f, 0.f);
//
//    pVertices[6].vPosition = _float3(-0.5f, -0.5f, 0.5f);  // 좌하단
//    pVertices[6].vNormal = _float3(0.0f, 0.0f, 1.0f);
//    pVertices[6].vTexcoord = _float2(1.f, 1.f);
//
//    pVertices[7].vPosition = _float3(0.5f, -0.5f, 0.5f);   // 우하단
//    pVertices[7].vNormal = _float3(0.0f, 0.0f, 1.0f);
//    pVertices[7].vTexcoord = _float2(0.f, 1.f);
//
//    // 오른쪽 면 (노말: 1, 0, 0) - 정점 8-11
//    pVertices[8].vPosition = _float3(0.5f, 0.5f, -0.5f);    // 좌상단
//    pVertices[8].vNormal = _float3(1.0f, 0.0f, 0.0f);
//    pVertices[8].vTexcoord = _float2(0.f, 0.f);
//
//    pVertices[9].vPosition = _float3(0.5f, 0.5f, 0.5f);    // 우상단
//    pVertices[9].vNormal = _float3(1.0f, 0.0f, 0.0f);
//    pVertices[9].vTexcoord = _float2(1.f, 0.f);
//
//    pVertices[10].vPosition = _float3(0.5f, -0.5f, 0.5f);  // 우하단
//    pVertices[10].vNormal = _float3(1.0f, 0.0f, 0.0f);
//    pVertices[10].vTexcoord = _float2(1.f, 1.f);
//
//    pVertices[11].vPosition = _float3(0.5f, -0.5f, -0.5f);  // 좌하단
//    pVertices[11].vNormal = _float3(1.0f, 0.0f, 0.0f);
//    pVertices[11].vTexcoord = _float2(0.f, 1.f);
//
//    // 왼쪽 면 (노말: -1, 0, 0) - 정점 12-15
//    pVertices[12].vPosition = _float3(-0.5f, 0.5f, 0.5f);  // 좌상단
//    pVertices[12].vNormal = _float3(-1.0f, 0.0f, 0.0f);
//    pVertices[12].vTexcoord = _float2(0.f, 0.f);
//
//    pVertices[13].vPosition = _float3(-0.5f, 0.5f, -0.5f);  // 우상단
//    pVertices[13].vNormal = _float3(-1.0f, 0.0f, 0.0f);
//    pVertices[13].vTexcoord = _float2(1.f, 0.f);
//
//    pVertices[14].vPosition = _float3(-0.5f, -0.5f, -0.5f); // 우하단
//    pVertices[14].vNormal = _float3(-1.0f, 0.0f, 0.0f);
//    pVertices[14].vTexcoord = _float2(1.f, 1.f);
//
//    pVertices[15].vPosition = _float3(-0.5f, -0.5f, 0.5f); // 좌하단
//    pVertices[15].vNormal = _float3(-1.0f, 0.0f, 0.0f);
//    pVertices[15].vTexcoord = _float2(0.f, 1.f);
//
//    // 위쪽 면 (노말: 0, 1, 0) - 정점 16-19
//    pVertices[16].vPosition = _float3(-0.5f, 0.5f, 0.5f);  // 좌상단
//    pVertices[16].vNormal = _float3(0.0f, 1.0f, 0.0f);
//    pVertices[16].vTexcoord = _float2(0.f, 0.f);
//
//    pVertices[17].vPosition = _float3(0.5f, 0.5f, 0.5f);   // 우상단
//    pVertices[17].vNormal = _float3(0.0f, 1.0f, 0.0f);
//    pVertices[17].vTexcoord = _float2(1.f, 0.f);
//
//    pVertices[18].vPosition = _float3(0.5f, 0.5f, -0.5f);   // 우하단
//    pVertices[18].vNormal = _float3(0.0f, 1.0f, 0.0f);
//    pVertices[18].vTexcoord = _float2(1.f, 1.f);
//
//    pVertices[19].vPosition = _float3(-0.5f, 0.5f, -0.5f);  // 좌하단
//    pVertices[19].vNormal = _float3(0.0f, 1.0f, 0.0f);
//    pVertices[19].vTexcoord = _float2(0.f, 1.f);
//
//    // 아래쪽 면 (노말: 0, -1, 0) - 정점 20-23
//    pVertices[20].vPosition = _float3(-0.5f, -0.5f, -0.5f); // 좌상단
//    pVertices[20].vNormal = _float3(0.0f, -1.0f, 0.0f);
//    pVertices[20].vTexcoord = _float2(0.f, 0.f);
//
//    pVertices[21].vPosition = _float3(0.5f, -0.5f, -0.5f);  // 우상단
//    pVertices[21].vNormal = _float3(0.0f, -1.0f, 0.0f);
//    pVertices[21].vTexcoord = _float2(1.f, 0.f);
//
//    pVertices[22].vPosition = _float3(0.5f, -0.5f, 0.5f);  // 우하단
//    pVertices[22].vNormal = _float3(0.0f, -1.0f, 0.0f);
//    pVertices[22].vTexcoord = _float2(1.f, 1.f);
//
//    pVertices[23].vPosition = _float3(-0.5f, -0.5f, 0.5f); // 좌하단
//    pVertices[23].vNormal = _float3(0.0f, -1.0f, 0.0f);
//    pVertices[23].vTexcoord = _float2(0.f, 1.f);
//
//    m_pVB->Unlock();
//#pragma endregion
//
//#pragma region INDEX_BUFFER
//    if (FAILED(__super::Create_IndexBuffer()))
//        return E_FAIL;
//
//    _ushort* pIndices = { nullptr };
//    m_pIB->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);
//    _uint iIndex = 0;
//
//    // 앞면 (0-3)
//    pIndices[iIndex++] = 0;
//    pIndices[iIndex++] = 1;
//    pIndices[iIndex++] = 2;
//
//    pIndices[iIndex++] = 0;
//    pIndices[iIndex++] = 2;
//    pIndices[iIndex++] = 3;
//
//    // 뒷면 (4-7)
//    pIndices[iIndex++] = 4;
//    pIndices[iIndex++] = 5;
//    pIndices[iIndex++] = 6;
//
//    pIndices[iIndex++] = 4;
//    pIndices[iIndex++] = 6;
//    pIndices[iIndex++] = 7;
//
//    // 오른쪽 면 (8-11)
//    pIndices[iIndex++] = 8;
//    pIndices[iIndex++] = 9;
//    pIndices[iIndex++] = 10;
//
//    pIndices[iIndex++] = 8;
//    pIndices[iIndex++] = 10;
//    pIndices[iIndex++] = 11;
//
//    // 왼쪽 면 (12-15)
//    pIndices[iIndex++] = 12;
//    pIndices[iIndex++] = 13;
//    pIndices[iIndex++] = 14;
//
//    pIndices[iIndex++] = 12;
//    pIndices[iIndex++] = 14;
//    pIndices[iIndex++] = 15;
//
//    // 위쪽 면 (16-19)
//    pIndices[iIndex++] = 16;
//    pIndices[iIndex++] = 17;
//    pIndices[iIndex++] = 18;
//
//    pIndices[iIndex++] = 16;
//    pIndices[iIndex++] = 18;
//    pIndices[iIndex++] = 19;
//
//    // 아래쪽 면 (20-23)
//    pIndices[iIndex++] = 20;
//    pIndices[iIndex++] = 21;
//    pIndices[iIndex++] = 22;
//
//    pIndices[iIndex++] = 20;
//    pIndices[iIndex++] = 22;
//    pIndices[iIndex++] = 23;
//
//    m_pIB->Unlock();
//#pragma endregion
//
//    return S_OK;
//}
HRESULT CVIBuffer_Cube::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Cube* CVIBuffer_Cube::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Cube::Clone(void* pArg)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();

}
