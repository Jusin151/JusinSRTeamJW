#include "Texture.h"
#include "Shader.h"

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CComponent { pGraphic_Device }
{
}

CTexture::CTexture(const CTexture& Prototype)
    : CComponent( Prototype )
	, m_iNumTextures { Prototype.m_iNumTextures }
	, m_Textures { Prototype.m_Textures }
	, m_eType { Prototype.m_eType }
{
	for (auto& pTexture : m_Textures)	
		Safe_AddRef(pTexture);	
}

HRESULT CTexture::Initialize_Prototype(TYPE eType, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	m_eType = eType;
	m_iNumTextures = iNumTextures;

	for (size_t i = 0; i < m_iNumTextures; i++)
	{
		IDirect3DBaseTexture9*		pTexture = {nullptr};

		_tchar			szTextureFileName[MAX_PATH] = {};

		wsprintf(szTextureFileName, pTextureFilePath, i);

		D3DXIMAGE_INFO imageInfo;
		// 먼저 이미지 정보를 가져옴
		if (FAILED(D3DXGetImageInfoFromFile(szTextureFileName, &imageInfo)))
			return E_FAIL; // 이미지 정보를 가져오지 못한 경우

		

		HRESULT hr = eType == TYPE_2D ?
			D3DXCreateTextureFromFileEx(
				m_pGraphic_Device,
				szTextureFileName,
				imageInfo.Width,  // 원본 너비
				imageInfo.Height, // 원본 높이
				D3DX_DEFAULT,
				0,
				D3DFMT_UNKNOWN,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				0,
				&imageInfo,
				nullptr,
				reinterpret_cast<IDirect3DTexture9**>(&pTexture)) :
			D3DXCreateCubeTextureFromFileEx(m_pGraphic_Device,
				szTextureFileName,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				0,
				D3DFMT_UNKNOWN,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				0,
				&imageInfo,
				nullptr,
				reinterpret_cast<IDirect3DCubeTexture9**>(&pTexture));

		if (FAILED(hr))
			return E_FAIL;

		if (m_Textures.size() > 320)
		{
			int a = 10;
		}
  		m_Textures.push_back(pTexture);
	}

    return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CTexture::Bind_Resource(_uint iIndex)
{
	if (iIndex >= m_iNumTextures)
		return E_FAIL;

	m_iCurrentTexture = iIndex;
	return m_pGraphic_Device->SetTexture(0, m_Textures[iIndex]);		
}

HRESULT CTexture::Bind_Resource(CShader* pShader, D3DXHANDLE hParameters, _uint iIndex)
{
	if (iIndex >= m_iNumTextures)
		return E_FAIL;
	m_iCurrentTexture = iIndex;
	return pShader->Bind_Texture(hParameters, m_Textures[iIndex]);

}

HRESULT CTexture::GetPixelColor(const _float3& vWorldPos, const _float3& vObjectScale, D3DXCOLOR* pColor)
{
	if (!pColor)
		return E_FAIL;

	// 객체의 위치와 크기 (예시)
	_float3 objectPosition = { 0.0f, 0.0f, 0.0f }; // 객체의 중심 위치

	// 월드 좌표를 객체의 로컬 좌표로 변환
	_float3 localPos = {
		(vWorldPos.x - objectPosition.x) / vObjectScale.x,
		(vWorldPos.y - objectPosition.y) / vObjectScale.y,
		(vWorldPos.z - objectPosition.z) / vObjectScale.z
	};

	// 로컬 좌표를 0~1 범위로 정규화
	float normalizedX = (localPos.x + 0.5f);
	float normalizedY = (localPos.y + 0.5f);

	// 텍스처를 잠급니다.
	D3DLOCKED_RECT lockedRect;
	IDirect3DTexture9* pTexture = static_cast<IDirect3DTexture9*>(m_Textures[m_iCurrentTexture]);
	if (FAILED(pTexture->LockRect(0, &lockedRect, nullptr, D3DLOCK_READONLY)))
		return E_FAIL;

	// 텍스처의 크기를 가져옵니다.
	D3DSURFACE_DESC desc;
	pTexture->GetLevelDesc(0, &desc);

	// 정규화된 좌표를 텍스처 좌표로 변환합니다.
	int x = static_cast<int>(normalizedX * desc.Width);
	int y = static_cast<int>(normalizedY * desc.Height);

	// 텍스처 좌표가 유효한지 확인합니다.
	if (x < 0 || x >= static_cast<int>(desc.Width) || y < 0 || y >= static_cast<int>(desc.Height))
	{
		pTexture->UnlockRect(0);
		return E_FAIL;
	}

	// 픽셀 데이터를 읽어옵니다.
	DWORD* pTexel = reinterpret_cast<DWORD*>(static_cast<BYTE*>(lockedRect.pBits) + y * lockedRect.Pitch + x * sizeof(DWORD));
	D3DXCOLOR color(*pTexel);

	// 읽어온 색상을 반환합니다.
	*pColor = color;

	// 텍스처 잠금을 해제합니다.
	pTexture->UnlockRect(0);

	return S_OK;
}

CTexture* CTexture::Create(LPDIRECT3DDEVICE9 pGraphic_Device, TYPE eType, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pGraphic_Device);

  	if (FAILED(pInstance->Initialize_Prototype(eType, pTextureFilePath, iNumTextures)))
	{
 		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
    __super::Free();

	for (auto& pTexture : m_Textures)
		Safe_Release(pTexture);

	m_Textures.clear();

}
