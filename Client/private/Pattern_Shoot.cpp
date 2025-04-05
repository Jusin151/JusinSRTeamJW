#include "Pattern_Shoot.h"
#include "HellBoss.h"
#include "HellBoss_IdleState.h" 

CPattern_Shoot::CPattern_Shoot()
{
    m_bStarted = false;
    m_fAccTime = 0.f;
    m_fDuration = 1.5f; //  대기
}

void CPattern_Shoot::Execute(CHellBoss* pBoss, float fDeltaTime)
{
    if (!m_bStarted)
    {
        m_bStarted = true;
        m_fAccTime = 0.f;

        
        pBoss->Set_Animation("4_Shoot");

        // 사운드 재생 
        //  pBoss->Play_Sound("발사재생.wav");

        //  이펙트 생성 
        // pBoss->Fire_Shoot(); 같은거 
    }

    //  시간
    m_fAccTime += fDeltaTime;

    if (m_fAccTime >= m_fDuration)
    {
        m_bStarted = false;
        m_fAccTime = 0.f;

        // 상태(보스가 Idle 상태로 돌아가게 할 예정)
        pBoss->Change_State(new CHellBoss_IdleState());
    }
}
