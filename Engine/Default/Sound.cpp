#include "Sound.h"

CSound::CSound(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
{
}

CSound::CSound(const CSound& Prototype)
	: CComponent(Prototype)
	, m_iNumSounds{ Prototype.m_iNumSounds }
	, m_Sounds{ Prototype.m_Sounds }
{
	for (auto& pTexture : m_Sounds)
		Safe_AddRef(pTexture);
}

HRESULT CSound::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	m_iNumSounds = iNumTextures;

	for (size_t i = 0; i < m_iNumSounds; i++)
	{
		FMOD::Sound*		pSound = { nullptr };

		_tchar			szTextureFileName[MAX_PATH] = {};

		wsprintf(szTextureFileName, pTextureFilePath, i);

		/*if (FAILED(D3DXCreateTextureFromFile(m_pGraphic_Device, szTextureFileName, &pTexture)))
			return E_FAIL;*/

		//m_Sounds.push_back(pTexture);
	}

	return S_OK;
}

HRESULT CSound::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CSound::Bind_Resource(_uint iIndex)
{
	if (iIndex >= m_iNumSounds)
		return E_FAIL;

	return S_OK; //m_pGraphic_Device->SetTexture(0, m_Sounds[iIndex]);
}

CSound* CSound::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	CSound* pInstance = new CSound(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CSound");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CSound::Clone(void* pArg)
{
	CSound* pInstance = new CSound(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CSound");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound::Free()
{
	__super::Free();

	for (auto& pSound : m_Sounds)
		Safe_Release(pSound);

	m_Sounds.clear();

}
