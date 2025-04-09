#include "Pattern_Warp.h"
#include "HellBoss.h"
#include "HellBoss_IdleState.h"
#include "HellBoss_Bullet.h"

CPattern_Warp::CPattern_Warp()
{
    m_bStarted = false;
    m_fAccTime = 0.f;
    m_fDuration = 1.5f; //  대기
}

void CPattern_Warp::Execute(CHellBoss* pBoss, float fDeltaTime)
{
    if (!m_bStarted)
    {
        m_bStarted = true;
        m_fAccTime = 0.f;
        pBoss->Set_Animation("T_Phase2_End"); // 워프 모션
    }

    if (!m_bWarped) 
        m_bWarped = true;   

    m_fAccTime += fDeltaTime;

    if (pBoss->Get_AnimationFinished())
    {
        m_bStarted = false;
        m_bWarped = false;
        m_fAccTime = 0.f;
    }
}


void CPattern_Warp::Update(CHellBoss* pBoss, float fDeltaTime)
{
}

