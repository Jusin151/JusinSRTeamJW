#include "VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer { pGraphic_Device }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
	: CVIBuffer{ Prototype }
	, m_iNumVerticesX { Prototype.m_iNumVerticesX }
	, m_iNumVerticesZ { Prototype.m_iNumVerticesZ }
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ)
{
	//체인지레벨에 클리어 함수들 만드셈 ㅋ
	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;
	m_iNumVertices = iNumVerticesX * iNumVerticesZ;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iFVF = D3DFVF_NORMAL | D3DFVF_XYZ | D3DFVF_TEX1/* | D3DFVF_TEXCOORDSIZE2(0)*/;
	m_iNumPritimive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iNumIndices = m_iNumPritimive * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = D3DFMT_INDEX32;

#pragma region VERTEX_BUFFER
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	VTXNORTEX* pVertices = { nullptr };


	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, reinterpret_cast<void**>(&pVertices), 0);

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint			iIndex = (_uint)i * m_iNumVerticesX + (_uint)j;

			if (j > m_iNumVerticesX / 2)
			{
				
				pVertices[iIndex].vPosition = _float3((_float)j, 10.f, (_float)i);
			} else
			{
				pVertices[iIndex].vPosition = _float3((_float)j, 0.f, (_float)i);
			}
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2((_float)j / (m_iNumVerticesX - 1.f), (_float)i / (m_iNumVerticesZ - 1.f));
		}
	}




#pragma endregion

#pragma region INDEX_BUFFER
	m_pIndices = new _uint[m_iNumIndices];

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	_uint* pIndices = { nullptr };

	m_pIB->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint			iIndex = (_uint)i * m_iNumVerticesX + (_uint)j;

			_uint			iIndices[4] = {
				iIndex + m_iNumVerticesX, 
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1, 
				iIndex
			};

			_float3		vSourDir, vDestDir, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSourDir = pVertices[iIndices[1]].vPosition - pVertices[iIndices[0]].vPosition;
			vDestDir = pVertices[iIndices[2]].vPosition - pVertices[iIndices[1]].vPosition;

			D3DXVec3Normalize(&vNormal, D3DXVec3Cross(&vNormal, &vSourDir, &vDestDir));

			pVertices[iIndices[0]].vNormal += vNormal;
			pVertices[iIndices[1]].vNormal += vNormal;
			pVertices[iIndices[2]].vNormal += vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSourDir = pVertices[iIndices[2]].vPosition - pVertices[iIndices[0]].vPosition;
			vDestDir = pVertices[iIndices[3]].vPosition - pVertices[iIndices[2]].vPosition;

			D3DXVec3Normalize(&vNormal, D3DXVec3Cross(&vNormal, &vSourDir, &vDestDir));

			pVertices[iIndices[0]].vNormal += vNormal;
			pVertices[iIndices[2]].vNormal += vNormal;
			pVertices[iIndices[3]].vNormal += vNormal;
		}
	}

	memcpy(m_pIndices, pIndices, m_iIndexStride * m_iNumIndices);

	for (size_t i = 0; i < m_iNumVertices; i++)	
		D3DXVec3Normalize(&pVertices[i].vNormal, &pVertices[i].vNormal);

	m_pVB->Unlock();
	m_pIB->Unlock();

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
{
	_ulong			dwByte = { };
	HANDLE			hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER		fh{};
	BITMAPINFOHEADER		ih{};

	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

	_uint* pPixels = new _uint[ih.biWidth * ih.biHeight];

	ReadFile(hFile, pPixels, sizeof(_uint) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

	CloseHandle(hFile);

	
	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1/* | D3DFVF_TEXCOORDSIZE2(0)*/;
	m_iNumPritimive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iNumIndices = m_iNumPritimive * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = D3DFMT_INDEX32;

#pragma region VERTEX_BUFFER
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_pVertexPositions = new _float3[m_iNumVertices];

	VTXNORTEX* pVertices = { nullptr };

	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, reinterpret_cast<void**>(&pVertices), 0);

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint			iIndex = i * m_iNumVerticesX + j;

			m_pVertexPositions[iIndex] = pVertices[iIndex].vPosition = _float3(static_cast<_float>(j), (pPixels[iIndex] & 0x000000ff) / 10.f, static_cast<_float>(i));
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f) * 50.f, i / (m_iNumVerticesZ - 1.f) * 50.f);
		}
	}


	Safe_Delete_Array(pPixels);
#pragma endregion

#pragma region INDEX_BUFFER
	m_pIndices = new _uint[m_iNumIndices];

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	_uint* pIndices = { nullptr };

	m_pIB->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	_uint		iNumIndices = { 0 };

	for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint			iIndex = i * m_iNumVerticesX + j;

			_uint			iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_float3		vSourDir, vDestDir, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSourDir = pVertices[iIndices[1]].vPosition - pVertices[iIndices[0]].vPosition;
			vDestDir = pVertices[iIndices[2]].vPosition - pVertices[iIndices[1]].vPosition;

			D3DXVec3Normalize(&vNormal, D3DXVec3Cross(&vNormal, &vSourDir, &vDestDir));

			pVertices[iIndices[0]].vNormal += vNormal;
			pVertices[iIndices[1]].vNormal += vNormal;
			pVertices[iIndices[2]].vNormal += vNormal;



			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];


			vSourDir = pVertices[iIndices[2]].vPosition - pVertices[iIndices[0]].vPosition;
			vDestDir = pVertices[iIndices[3]].vPosition - pVertices[iIndices[2]].vPosition;

			D3DXVec3Normalize(&vNormal, D3DXVec3Cross(&vNormal, &vSourDir, &vDestDir));

			pVertices[iIndices[0]].vNormal += vNormal;
			pVertices[iIndices[2]].vNormal += vNormal;
			pVertices[iIndices[3]].vNormal += vNormal;
		}
	}

	for (size_t i = 0; i < m_iNumVertices; i++)
		D3DXVec3Normalize(&pVertices[i].vNormal, &pVertices[i].vNormal);
	memcpy(m_pIndices, pIndices, m_iIndexStride * m_iNumIndices);
	m_pVB->Unlock();
	m_pIB->Unlock();

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

_float3 CVIBuffer_Terrain::Compute_HeightPosition(const _float3& vPosition)
{
	_uint	iIndex = static_cast<_uint>(vPosition.z) * m_iNumVerticesX + static_cast<_uint>(vPosition.x);

	_uint	iIndices[4] = {
		iIndex + m_iNumVerticesX, 
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,
		iIndex
	};

	_float		fWidth = vPosition.x - m_pVertexPositions[iIndices[0]].x;
	_float		fDepth = m_pVertexPositions[iIndices[0]].z - vPosition.z;

	D3DXPLANE		Plane = {};

	/*
	직선의 방정식 : ax + by + c = 0
	y = 기울기x + y절편;

	평면의 방정식 : ax + by + cz + d = 0
	*/

	/* 
	y = (-ax - cz - d) / b
	*/

	/* 오른쪽 위 삼각형 안에 있다. */
	if (fWidth > fDepth)
	{
		// D3DXPlaneFromPointNormal(&Plane, );
		D3DXPlaneFromPoints(&Plane, &m_pVertexPositions[iIndices[0]], &m_pVertexPositions[iIndices[1]], &m_pVertexPositions[iIndices[2]]);	 
		
	}

	/*왼쪽 하단 삼각형에 있다. */
	else
	{
		D3DXPlaneFromPoints(&Plane, &m_pVertexPositions[iIndices[0]], &m_pVertexPositions[iIndices[2]], &m_pVertexPositions[iIndices[3]]);
	}

	return _float3(vPosition.x, (-Plane.a * vPosition.x - Plane.c * vPosition.z - Plane.d) / Plane.b, vPosition.z);
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iNumVerticesX, _uint iNumVerticesZ)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(iNumVerticesX, iNumVerticesZ)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}
