#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
	enum CHANNELID { CN_BACKGROUND = 0, CN_SFX, CN_END};
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT		Initialize(_int iNumChannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void* pArg = nullptr);
	void Priority_Update();
	void Update();
	void Late_Update();
	void		Play(CHANNELID eChannelID, _wstring strSoundTag, void* pArg = nullptr);
	void		PlayEvent(_wstring strEventPath, void* pArg = nullptr);
	void		Stop_All_Event();
	void		Stop();
	Studio::Bank* LoadBank(_wstring strBankPath, void * pArg = nullptr);

private:

private:
	FMOD::Studio::System*	m_pStudioSystem;
	FMOD::System*			m_pCoreSystem;

public:
	static CSound_Manager* Create(_int iNumChannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void* pArg = nullptr);
	virtual void Free() override;
};

END