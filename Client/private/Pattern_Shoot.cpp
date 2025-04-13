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
    }

    m_fAccTime += fDeltaTime;

    if (pBoss->Get_Phase() == PHASE2 || pBoss->Get_Phase() == PHASE3 || pBoss->Get_Phase() == PHASE4)
    {
        if (m_fAccTime >= m_fNextFireTime && m_iFiredCount < 100)
        {
            CHellBoss_Bullet::PowerBlastDesc pDesc{};
            if (pBoss->Get_Phase() == PHASE2)
                pDesc.wBulletType = L"0_Phase2_Shoot";
            else
                pDesc.wBulletType = L"O_ArmCut_Shoot";

            pDesc.isLeft = (m_iFiredCount % 2 == 0);
            pBoss->Get_GameInstance()->Add_GameObject_FromPool(
                LEVEL_HONG, LEVEL_HONG, TEXT("Layer_HellBoss_PHASE2_HandBullet"), &pDesc);

            m_iFiredCount++;
            m_fNextFireTime += 0.2f;
        }
    }

    if (pBoss->Get_AnimationFinished() && m_fAccTime >= 1.0f)
    {
        m_bStarted = false;
        m_fAccTime = 0.f;
    }
    m_fAccTime += fDeltaTime;

    if (pBoss->Get_Phase() == PHASE2)
    {

        if (m_fAccTime >= m_fNextFireTime && m_iFiredCount < 100)
        {
            CHellBoss_Bullet::PowerBlastDesc pDesc{};
            pDesc.wBulletType = L"0_Phase2_Shoot";
            pDesc.isLeft = (m_iFiredCount % 2 == 0);
            if (!pBoss->Get_GameInstance()->Add_GameObject_FromPool(LEVEL_HONG, LEVEL_HONG, TEXT("Layer_HellBoss_PHASE2_HandBullet"), &pDesc))
            {
                MSG_BOX("HellBoss_Bullet 생성 실패");
            }
            m_iFiredCount++;
            m_fNextFireTime += 0.2f;  // 몇초마다 발사할껀지
        }
    }
    else if (pBoss->Get_Phase() == PHASE3)
    {
        if (m_fAccTime >= m_fNextFireTime && m_iFiredCount < 100)
        {
            CHellBoss_Bullet::PowerBlastDesc pDesc{};
            pDesc.wBulletType = L"O_ArmCut_Shoot";
            pDesc.isLeft = 1;
            if (!pBoss->Get_GameInstance()->Add_GameObject_FromPool(LEVEL_HONG, LEVEL_HONG, TEXT("Layer_HellBoss_PHASE2_HandBullet"), &pDesc))
            {
                MSG_BOX("HellBoss_Bullet 생성 실패");
            }
            m_iFiredCount++;
            m_fNextFireTime += 0.2f;  // 몇초마다 발사할껀지
        }
    }

    else
    {
        if (!m_bHasFired)
        {
            int iCurFrame = pBoss->Get_CurAnimationFrame();
            bool bShouldFire = false;
            switch (pBoss->Get_Phase())
            {
            case PHASE1:
                bShouldFire = (iCurFrame >= 36);
                break;
            }

            if (bShouldFire)
            {
                m_bHasFired = true;
                CHellBoss_Bullet::PowerBlastDesc pDesc{};
                switch (pBoss->Get_Phase())
                {
                case PHASE1:
                    pDesc.wBulletType = L"4_Shoot";
                    break;
                }
                if (!pBoss->Get_GameInstance()->Add_GameObject_FromPool(LEVEL_HONG, LEVEL_HONG, TEXT("Layer_HellBoss_PHASE1_HandBullet"),&pDesc))
                {
                    MSG_BOX("HellBoss_Bullet 생성 실패");
                }
               /* if (FAILED(pBoss->Get_GameInstance()->Add_GameObject(
                    LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss_Bullet"),
                    LEVEL_HONG, TEXT("Layer_HellBoss_Bullet"), &pDesc)))*/
            }
        }
    }

    switch (pBoss->Get_Phase())
    {
    case PHASE1:
        if (pBoss->Get_AnimationFinished())
        {
            m_bStarted = false;
            m_fAccTime = 0.f;
            pBoss->Change_State(new CHellBoss_IdleState());
        }
        break;
    case PHASE2:
        if (pBoss->Get_AnimationFinished())
        {
            // 애니메이션이 끝나고 최소 4초가 경과하면 다음 상태로 전환
            if (m_fAccTime >= 1.0f)
            {
                m_bStarted = false;
                pBoss->Change_State(new CHellBoss_IdleState());
                m_fAccTime = 0.f;
            }
        }
        break;
    case PHASE3:
        if (pBoss->Get_AnimationFinished())
        {
           
        }
    
        break;
    case PHASE4:
  
        break;
    }
}





void CPattern_Shoot::Update(CHellBoss* pBoss, float fDeltaTime)
{

}


