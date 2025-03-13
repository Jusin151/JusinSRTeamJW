#include "Sound_Manager.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize(_int iNumChannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void* pArg)
{
	if (FAILED(Studio::System::create(&m_pStudioSystem)))
	{
		MSG_BOX("Failed to Created : m_pStudioSystem");
		return E_FAIL;
	}
	m_pStudioSystem->getCoreSystem(&m_pCoreSystem);
	m_pStudioSystem->initialize(iNumChannels, studioFlags, flags, pArg);

	return S_OK;
}

void CSound_Manager::Priority_Update()
{
	if (nullptr == m_pCoreSystem)
		return;
	if (nullptr == m_pStudioSystem)
		return;
}

void CSound_Manager::Update()
{
	if (nullptr == m_pCoreSystem)
		return;
	if (nullptr == m_pStudioSystem)
		return;
	m_pStudioSystem->update();
	m_pCoreSystem->update();
}

void CSound_Manager::Late_Update()
{
	if (nullptr == m_pCoreSystem)
		return;
	if (nullptr == m_pStudioSystem)
		return;
}

void CSound_Manager::Play(CHANNELID eChannelID, _wstring strSoundTag, void* pArg)
{

}

CSound_Manager* CSound_Manager::Create(_int iNumChannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void* pArg)
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Initialize(iNumChannels, studioFlags, flags, pArg)))
	{
		MSG_BOX("Failed to Created : CSound_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	__super::Free();

	//대소문자 구별로 인한 매크로 함수 호출 불가능
	if (nullptr != m_pStudioSystem)
	{
		m_pStudioSystem->release();
		m_pStudioSystem = nullptr;
	}
		
	if (nullptr != m_pCoreSystem)
	{
		m_pCoreSystem->release();
		m_pCoreSystem = nullptr;
	}
	//Safe_Release(m_pStudioSystem);
	//Safe_Release(m_pCoreSystem);
	//Safe_Release();
}
