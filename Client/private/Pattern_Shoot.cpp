#include "Pattern_Shoot.h"
#include "HellBoss.h"
#include "HellBoss_IdleState.h" 
#include "HellBoss_Bullet.h"

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
        switch (pBoss->Get_Phase())
        {
        case CHellBoss::PHASE1:
            pBoss->Set_Animation("4_Shoot");
            break;
        case CHellBoss::PHASE2:
            pBoss->Set_Animation("0_Phase2_Shoot");
            break;
        case CHellBoss::PHASE3:
            pBoss->Set_Animation("K_Phase3_Nova"); // 예시
            break;
        case CHellBoss::PHASE4:
            pBoss->Set_Animation("M_Phase4_Death"); // 예시
            break;
        }

        m_bStarted = true;
        m_fAccTime = 0.f;
        m_bHasFired = false;
    }

    m_fAccTime += fDeltaTime;

    if (!m_bHasFired)
    {
        int iCurFrame = pBoss->Get_CurAnimationFrame();
        bool bShouldFire = false;

        switch (pBoss->Get_Phase())
        {
        case CHellBoss::PHASE1:
            bShouldFire = (iCurFrame >= 36);
            break;
        case CHellBoss::PHASE2:
            bShouldFire = (iCurFrame >= 104);
            break;
        case CHellBoss::PHASE3:
            bShouldFire = (iCurFrame >= 224); // 예시: Nova 시전 시작
            break;
        case CHellBoss::PHASE4:
            bShouldFire = (iCurFrame >= 312); // 예시: 죽는 순간 쏨?
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
            case CHellBoss::PHASE2:
                pDesc.wBulletType = L"0_Phase2_Shoot";
                break;
            case CHellBoss::PHASE3:
                pDesc.wBulletType = L"K_Phase3_Nova";
                break;
            case CHellBoss::PHASE4:
                pDesc.wBulletType = L"M_Phase4_Death";
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
            if (m_fAccTime >= 4.0f)
            {
                m_bHasFired = false;
                pBoss->Change_State(new CHellBoss_IdleState());
                m_fAccTime = 0.f;
            }
        }
        break;

    case CHellBoss::PHASE3:
        // 예시: 이후 로직 정의 필요
        break;

    case CHellBoss::PHASE4:
        // 예시: 이후 로직 정의 필요
        break;
    }
}




void CPattern_Shoot::Update(CHellBoss* pBoss, float fDeltaTime)
{

}


