#include "Pattern_Shoot.h"
#include "HellBoss.h"
#include "HellBoss_IdleState.h" 
#include "HellBoss_Bullet.h"

CPattern_Shoot::CPattern_Shoot()
    : m_bStarted(false)
    , m_fAccTime(0.f)
    , m_fDuration(1.5f)
    , m_iFiredCount(0)
    , m_fNextFireTime(0.f)
{
}
void CPattern_Shoot::Execute(CHellBoss* pBoss, float fDeltaTime)
{
    if (!m_bStarted)
    {
        m_bStarted = true;
        m_fAccTime = 0.f;
        m_iFiredCount = 0;
        m_fNextFireTime = 0.f;
        m_bHasFired = false;
    }

    m_fAccTime += fDeltaTime;

    if (pBoss->Get_Phase() == PHASE2)
    {
        if (m_fAccTime >= m_fNextFireTime && m_iFiredCount < 100)
        {
            CHellBoss_Bullet::PowerBlastDesc pDesc{};
            pDesc.wBulletType = L"0_Phase2_Shoot";
            pDesc.isLeft = (m_iFiredCount % 2 == 0);

            if (!pBoss->Get_GameInstance()->Add_GameObject_FromPool(
                LEVEL_HONG, LEVEL_HONG,
                TEXT("Layer_HellBoss_PHASE2_HandBullet"), &pDesc))
            {
                MSG_BOX("HellBoss_Bullet 생성 실패");
            }

            m_iFiredCount++;
            m_fNextFireTime += 0.2f;
        }
    }

    else if (pBoss->Get_Phase() == PHASE3)
    {
        int iCurFrame = pBoss->Get_CurAnimationFrame();

        if (iCurFrame == 130 && !m_bHasFired)
        {
            m_bHasFired = true;

            for (int i = 0; i < 2; ++i)
            {
                CHellBoss_Bullet::PowerBlastDesc pDesc{};
                pDesc.wBulletType = L"O_ArmCut_Shoot";
                pDesc.isLeft = (i % 2 == 0); // 좌우 번갈아

                if (!pBoss->Get_GameInstance()->Add_GameObject_FromPool(
                    LEVEL_HONG, LEVEL_HONG,
                    TEXT("Layer_HellBoss_PHASE2_HandBullet"), &pDesc))
                {
                    MSG_BOX("HellBoss_Bullet 생성 실패");
                }
            }
        }

        if (pBoss->Get_AnimationFinished())
        {
            m_bStarted = false;
            m_bHasFired = false;
            pBoss->Change_State(new CHellBoss_IdleState());
        }
    }

    else if (pBoss->Get_Phase() == PHASE1)
    {
        if (!m_bHasFired)
        {
            int iCurFrame = pBoss->Get_CurAnimationFrame();
            if (iCurFrame >= 36)
            {
                m_bHasFired = true;
                CHellBoss_Bullet::PowerBlastDesc pDesc{};
                pDesc.wBulletType = L"4_Shoot";

                if (!pBoss->Get_GameInstance()->Add_GameObject_FromPool(
                    LEVEL_HONG, LEVEL_HONG,
                    TEXT("Layer_HellBoss_PHASE1_HandBullet"), &pDesc))
                {
                    MSG_BOX("HellBoss_Bullet 생성 실패");
                }
            }
        }

        if (pBoss->Get_AnimationFinished())
        {
            m_bStarted = false;
            pBoss->Change_State(new CHellBoss_IdleState());
        }
    }
}






void CPattern_Shoot::Update(CHellBoss* pBoss, float fDeltaTime)
{

}


