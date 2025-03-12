#include "Sound_Device.h"

HRESULT CSound_Device::Initialize(System** ppOut, int maxchannels, FMOD_INITFLAGS flags, void* pArg)
{
	return E_NOTIMPL;
}

void CSound_Device::Play(CHANNELID eChannelID, _wstring strSoundTag, void* pArg)
{
}

void CSound_Device::Set_Parameters(int maxchannels, FMOD_INITFLAGS flags, void* pArg)
{
}

CSound_Device* CSound_Device::Create(System** ppOut, int maxchannels, FMOD_INITFLAGS flags, void* pArg)
{
	CSound_Device* pInstance = new CSound_Device();

	if (FAILED(pInstance->Initialize(ppOut, maxchannels, flags)))
	{
		MSG_BOX("Failed to Created : CSound_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Device::Free()
{
	//Safe_Release();
}
