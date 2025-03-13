#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
	enum CHANNELID { SND_BACKGROUND = 0, SND_SFX, SND_END};
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT			Initialize(_int iNumChannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void* pArg = nullptr);
	void			Priority_Update();
	void			Update();
	void			Late_Update();

public:
	void					Play(CHANNELID eChannelID, _wstring strSoundTag, void* pArg = nullptr);
	Studio::EventInstance*	Play_Event(_wstring strEventPath, void* pArg = nullptr);
	void					Stop_All_Event();
	void					Stop();
	Studio::Bank*			Load_Bank(_wstring strBankPath, void * pArg = nullptr);

private:
	FMOD::Studio::System*	m_pStudioSystem; //스튜디오 객체
	FMOD::System*			m_pCoreSystem;	//코어 객체(전에 사용 했던 방식)

public:
	static CSound_Manager*	Create(_int iNumChannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void* pArg = nullptr);
	virtual void			Free() override;
};

END