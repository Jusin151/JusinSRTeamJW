#include "Pattern_EyeBlast.h"
#include "HellBoss.h"
#include "HellBoss_IdleState.h"

CPattern_EyeBlast::CPattern_EyeBlast()
{
    m_bStarted = false;
    m_fAccTime = 0.f;
    m_fDuration = 1.5f; //  대기
}

void CPattern_EyeBlast::Execute(CHellBoss* pBoss, float fDeltaTime)
{
    if (!m_bStarted)
    {
        m_bStarted = true;
        m_fAccTime = 0.f;

        pBoss->Set_Animation("3_EyeBlast");
    }


    m_fAccTime += fDeltaTime;

    //  애니메이션이 끝났는지 
    if (pBoss->Get_AnimationFinished())
    {
        m_bStarted = false;
        m_fAccTime = 0.f;

      
        pBoss->Change_State(new CHellBoss_IdleState());
    }
}

