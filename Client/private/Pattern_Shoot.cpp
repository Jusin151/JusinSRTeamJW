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
    }

    if (!m_bHasFired && pBoss->Get_CurAnimationFrame() >= 36)
    {
        m_bHasFired = true;
        m_wBulletType = L"4_Shoot";
        if (FAILED(pBoss->Get_GameInstance()->Add_GameObject(
            LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss_Bullet"),
            LEVEL_HONG, TEXT("Layer_HellBoss_Bullet"),&m_wBulletType)))
        {
            MSG_BOX("HellBoss_Bullet 생성 실패");
        }
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

void CPattern_Shoot::Update(CHellBoss* pBoss, float fDeltaTime)
{

}


