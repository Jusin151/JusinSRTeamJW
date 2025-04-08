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
        switch (pBoss->Get_Phase())
        {
        case CHellBoss::PHASE1:
            pBoss->Set_Animation("4_Shoot");
            break;
        case CHellBoss::PHASE2:
            pBoss->Set_Animation("0_Phase2_Shoot");
            m_iFiredCount = 0;
            m_fNextFireTime = 0.f;
            break;
        case CHellBoss::PHASE3:
            pBoss->Set_Animation("K_Phase3_Nova");
            break;
        case CHellBoss::PHASE4:
            pBoss->Set_Animation("M_Phase4_Death");
            break;
        }
        m_bStarted = true;
        m_fAccTime = 0.f;
    
    }

    m_fAccTime += fDeltaTime;

    if (pBoss->Get_Phase() == CHellBoss::PHASE2)
    {

        if (m_fAccTime >= m_fNextFireTime && m_iFiredCount < 100)
        {
            CHellBoss_Bullet::PowerBlastDesc pDesc{};
            pDesc.wBulletType = L"0_Phase2_Shoot";
            if (FAILED(pBoss->Get_GameInstance()->Add_GameObject(
                LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss_Bullet"),
                LEVEL_HONG, TEXT("Layer_HellBoss_Bullet"), &pDesc)))
            {
                MSG_BOX("HellBoss_Bullet 생성 실패");
            }
            m_iFiredCount++;
            m_fNextFireTime += 0.2f;  // 몇초마다 발사할껀지
        }
    }
    else
    {
        // PHASE1 등 기존 로직
        if (!m_bHasFired)
        {
            int iCurFrame = pBoss->Get_CurAnimationFrame();
            bool bShouldFire = false;
            switch (pBoss->Get_Phase())
            {
            case CHellBoss::PHASE1:
                bShouldFire = (iCurFrame >= 36);
                break;
            }
            if (bShouldFire)
            {
                m_bHasFired = true;
                CHellBoss_Bullet::PowerBlastDesc pDesc{};
                switch (pBoss->Get_Phase())
                {
                case CHellBoss::PHASE1:
                    pDesc.wBulletType = L"4_Shoot";
                    break;
                }
                if (FAILED(pBoss->Get_GameInstance()->Add_GameObject(
                    LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss_Bullet"),
                    LEVEL_HONG, TEXT("Layer_HellBoss_Bullet"), &pDesc)))
                {
                    MSG_BOX("HellBoss_Bullet 생성 실패");
                }
            }
        }
    }

    switch (pBoss->Get_Phase())
    {
    case CHellBoss::PHASE1:
        if (pBoss->Get_AnimationFinished())
        {
            m_bStarted = false;
            m_fAccTime = 0.f;
            pBoss->Change_State(new CHellBoss_IdleState());
        }
        break;
    case CHellBoss::PHASE2:
        if (pBoss->Get_AnimationFinished())
        {
            // 애니메이션이 끝나고 최소 4초가 경과하면 다음 상태로 전환
            if (m_fAccTime >= 4.0f)
            {
                m_bStarted = false;
                pBoss->Change_State(new CHellBoss_IdleState());
                m_fAccTime = 0.f;
            }
        }
        break;
    case CHellBoss::PHASE3:
        // 추가 로직 필요 시 처리
        break;
    case CHellBoss::PHASE4:
        // 추가 로직 필요 시 처리
        break;
    }
}





void CPattern_Shoot::Update(CHellBoss* pBoss, float fDeltaTime)
{

}


