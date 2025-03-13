#include "Base.h"

BEGIN(Engine)

class CSound_Device final : public CBase
{
	enum CHANNELID { CN_BACKGROUND = 0, CN_SFX, CN_END};
private:
	CSound_Device();
	virtual ~CSound_Device() = default;

public:
	HRESULT		Initialize(System** ppOut, int maxchannels, FMOD_INITFLAGS flags, void* pArg = nullptr);
	void		Play(CHANNELID eChannelID, _wstring strSoundTag, void* pArg);

private:
	void		Set_Parameters(int maxchannels, FMOD_INITFLAGS flags, void* pArg = nullptr);

private:
	FMOD::System* m_pSystem;
	_uint		m_iMaxChannels;

public:
	static CSound_Device* Create(System** ppOut, int maxchannels, FMOD_INITFLAGS flags, void* pArg = nullptr);
	virtual void Free() override;
};

END