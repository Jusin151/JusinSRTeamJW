#include "Sound_Manager.h"

_uint CSound_Manager::sNextID = { 0 };

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

    Load_Bank(BANKPATH L"Master.streams.bank");
    Load_Bank(BANKPATH L"Master.strings.bank");
    Load_Bank(BANKPATH L"Master.bank");


    Load_Bank(BANKPATH L"Background.streams.bank");
    Load_Bank(BANKPATH L"Background.assets.bank");
    Load_Bank(BANKPATH L"Background.bank");

    //Play_Event();

	return S_OK;
}

void CSound_Manager::Priority_Update(_float fTimeDelta)
{
	if (nullptr == m_pCoreSystem)
		return;
	if (nullptr == m_pStudioSystem)
		return;
}

void CSound_Manager::Update(_float fTimeDelta)
{
	if (nullptr == m_pCoreSystem)
		return;
	if (nullptr == m_pStudioSystem)
		return;
 	m_pStudioSystem->update();
	m_pCoreSystem->update();
}

void CSound_Manager::Late_Update(_float fTimeDelta)
{
	if (nullptr == m_pCoreSystem)
		return;
	if (nullptr == m_pStudioSystem)
		return;
}

void CSound_Manager::Play(CHANNELID eChannelID, _wstring strSoundTag, void* pArg)
{

}

Studio::EventInstance* CSound_Manager::Play_Event(_wstring strEventPath, void* pArg)
{
    unsigned int retID = 0;
    // 이벤트가 존재하는지 확인
    auto iter = mEvents.find(strEventPath);
    if (iter != mEvents.end())
    {
        // 이벤트의 인스턴스를 생성한다.
        FMOD::Studio::EventInstance* event = nullptr;
        iter->second->createInstance(&event);
        if (event)
        {
            // 이벤트 인스턴스를 시작한다.
            event->start();
            // 새 아이디를 얻어 맵에 추가한다.
            sNextID++;
            retID = sNextID;
            mEventInstances.emplace(retID, event);

            //// release는 이벤트 인스턴스가 정지할 때 이벤트 소멸자 실행을 예약한다.
            //// 반복하지 않는 이벤트는 자동으로 정지한다.
            //event->release();
        }
    }
    return event(this, retID);
}

void CSound_Manager::Stop_All_Event()
{
	Studio::Bus* m_pBus = nullptr;
	m_pStudioSystem->getBus("bus:/", &m_pBus);
	m_pBus->stopAllEvents(FMOD_STUDIO_STOP_IMMEDIATE);
	//Update();
}

void CSound_Manager::Load_Bank(const _wstring& strBankPath)
{
    // 두 번 로딩되지 않게 한다.
    if (mBanks.find(strBankPath) != mBanks.end())
        return;

    // 뱅크 로드
    FMOD::Studio::Bank* bank = nullptr;
    FMOD_RESULT result = m_pStudioSystem->loadBankFile(
         WideToUtf8(strBankPath.c_str()).c_str(),    // 뱅크의 파일 이름
        FMOD_STUDIO_LOAD_BANK_NORMAL,   // 일반적인 방식으로 로딩
        &bank                           // 뱅크 포인터 저장
    );
    const int maxPathLength = 512;
    if (result == FMOD_OK)
    {
        //BANKPATH
        //strBankPath.
        // 뱅크를 맵에 추가
        mBanks.emplace(strBankPath, bank);
        // 스트리밍 형식이 아닌 모든 샘플 데이터를 로드
        bank->loadSampleData();
        // 뱅크의 이벤트 수를 얻는다
        int numEvents = 0;
        bank->getEventCount(&numEvents);
        if (numEvents > 0)
        {
            // 뱅크에서 이벤트 디스크립션 리스트를 얻는다
            // (뱅크의 이벤트 수와 같은 길이의 events vector를 선언)
            vector<FMOD::Studio::EventDescription*> events(numEvents);
            bank->getEventList(events.data(), numEvents, &numEvents);
            char eventName[maxPathLength];
            for (auto e : events)
            {
                // event:/Explosion2D 같은 이벤트의 경로를 얻는다.
                e->getPath(eventName, maxPathLength, nullptr);
                // 이벤트를 맵에 추가한다.
                mEvents.emplace(Utf8ToWide(eventName), e);
            }
        }
    }

    // 뱅크에 있는 버스들을 로드
    int numBuses = 0;
    bank->getBusCount(&numBuses);
    if (numBuses > 0)
    {
        // 뱅크에 있는 버스의 리스트를 가져온다
        std::vector<FMOD::Studio::Bus*> buses(numBuses);
        bank->getBusList(buses.data(), numBuses, &numBuses);
        char busName[512];
        for (auto bus : buses)
        {
            // 버스의 경로를 가져온다. (예를 들어 bus:/SFX 와 같은)
            bus->getPath(busName, 512, nullptr);
            // 버스를 맵에 추가한다.
            mBuses.emplace(Utf8ToWide(busName), bus);
        }
    }
}

void CSound_Manager::Unload_Bank(const _wstring& strBankPath)
{
    //// 로드 되지 않았다면 무시
    //auto iter = mBanks.find(strBankPath);
    //if (iter != mBanks.end())
    //{
    //    return;
    //}

    //// 먼저 이 뱅크의 모든 이벤트를 삭제한다.
    //FMOD::Studio::Bank* bank = iter->second;
    //int numEvents = 0;
    //bank->getEventCount(&numEvents);
    //if (numEvents > 0)
    //{
    //    // 이 뱅크에서 event descriptions들을 가져온다.
    //    std::vector<FMOD::Studio::EventDescription*> events(numEvents);
    //    // 이벤트의 리스트를 가져온다.
    //    bank->getEventList(events.data(), numEvents, &numEvents);
    //    char eventName[512];
    //    for (auto e : events)
    //    {
    //        // 이 이벤트의 경로를 가져온다
    //        e->getPath(eventName, 512, nullptr);
    //        // 이 이벤트를 삭제한다.
    //        auto eventi = mEvents.find(eventName);
    //        if (eventi != mEvents.end())
    //        {
    //            mEvents.erase(eventi);
    //        }
    //    }
    //}

    //// 이 뱅크의 모든 버스를 삭제한다.
    //int numBuses = 0;
    //bank->getBusCount(&numBuses);
    //if (numBuses > 0)
    //{
    //    // 이 뱅크의 모든 버스 리스트를 가져온다.
    //    std::vector<FMOD::Studio::Bus*> buses(numBuses);
    //    bank->getBusList(buses.data(), numBuses, &numBuses);
    //    char busName[512];
    //    for (auto bus : buses)
    //    {
    //        // 버스의 경로를 가져온다. (예를 들어 bus:/SFX 와 같은)
    //        bus->getPath(busName, 512, nullptr);
    //        // 이 버스를 삭제한다.
    //        auto busi = mBuses.find(busName);
    //        if (busi != mBuses.end())
    //        {
    //            mBuses.erase(busi);
    //        }
    //    }
    //}

    //// 샘플 데이터와 뱅크를 unload
    //bank->unloadSampleData();
    //bank->unload();
    //// banks map에서 bank 삭제
    //mBanks.erase(iter);
}

//Studio::Bank* CSound_Manager::Load_Bank(_wstring strBankPath, void* pArg)
//{
//	Studio::Bank* pBank = nullptr;
//	m_pStudioSystem->loadBankFile(reinterpret_cast<const char*>(strBankPath.c_str()), FMOD_STUDIO_LOAD_BANK_NORMAL, &pBank);
//
//	return nullptr;
//}

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
}
