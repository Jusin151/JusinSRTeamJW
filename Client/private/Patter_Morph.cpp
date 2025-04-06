#include "Patter_Morph.h"
#include "HellBoss.h"

CPatter_Morph::CPatter_Morph()
{
    m_bStarted = false;
    m_fAccTime = 0.f;
    m_fDuration = 1.5f; //  대기
}

void CPatter_Morph::Execute(CHellBoss* pBoss, float fDeltaTime)
{
    if (!m_bStarted)
    {
        m_bStarted = true;
        m_fAccTime = 0.f;

        
        pBoss->Set_Animation("5_Morph");

        // 사운드 재생 
        //  pBoss->Play_Sound("진화 재생.wav 같은느낌");

        //  이펙트 생성 
        // pBoss->Fire_Morph(); 같은거 
    }

    //  시간
    m_fAccTime += fDeltaTime;

    if (m_fAccTime >= m_fDuration)
    {
        m_bStarted = false;
        m_fAccTime = 0.f;

        // 상(보스가 Idle 상태로 돌아가게 할 예정)
        // pBoss->Change_State(new CHellBoss_IdleState());
    }
}

void CPatter_Morph::Update(CHellBoss* pBoss, _float fDeltaTime)
{
}
