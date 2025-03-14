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
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iFVF = D3DFVF_XYZ | D3DFVF_TEX1/* | D3DFVF_TEXCOORDSIZE2(0)*/;
	m_iNumPritimive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iNumIndices = m_iNumPritimive * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = D3DFMT_INDEX32;

#pragma region VERTEX_BUFFER
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	VTXPOSTEX* pVertices = { nullptr };


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
			
			pVertices[iIndex].vTexcoord = _float2((_float)j / (m_iNumVerticesX - 1.f), (_float)i / (m_iNumVerticesZ - 1.f));
		}
	}



	m_pVB->Unlock();
#pragma endregion

#pragma region INDEX_BUFFER
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

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];
		}
	}

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
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iFVF = D3DFVF_XYZ | D3DFVF_TEX1/* | D3DFVF_TEXCOORDSIZE2(0)*/;
	m_iNumPritimive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iNumIndices = m_iNumPritimive * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = D3DFMT_INDEX32;

#pragma region VERTEX_BUFFER
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	VTXPOSTEX* pVertices = { nullptr };

//	11111111 11100111 11100111 11100111
//&	00000000 00000000 00000000 11111111
//
//	00000000 00000000 00000000 11100111


		



	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, reinterpret_cast<void**>(&pVertices), 0);

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint			iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = _float3(j, (pPixels[iIndex] & 0x000000ff) / 15.f, i);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f) * 50.f, i / (m_iNumVerticesZ - 1.f) * 50.f);
		}
	}

	m_pVB->Unlock();

	Safe_Delete_Array(pPixels);
#pragma endregion

#pragma region INDEX_BUFFER
	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	_uint* pIndices = { nullptr };

	m_pIB->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint			iIndex = i * m_iNumVerticesX + j;

			_uint			iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];
		}
	}

	m_pIB->Unlock();

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
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
