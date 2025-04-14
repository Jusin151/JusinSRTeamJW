#include "Pattern_Shoot.h"
#include "HellBoss.h"
#include "HellBoss_IdleState.h" 
#include "HellBoss_Bullet.h"
#include "HellBoss_WalkState.h"

CPattern_Shoot::CPattern_Shoot()
    : m_bStarted(false)
    , m_fAccTime(0.f)
    , m_fDuration(1.5f)
    , m_iFiredCount(0)
    , m_fNextFireTime(0.f)
{
}
bool CPattern_Shoot::Is_Finished() const
{
    return m_bFinished;
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

    const float fCycle = 5.0f;

    if (pBoss->Get_Phase() == PHASE4)
    {
        if (m_fAccTime >= fCycle)
        {
            m_fAccTime = 0.f;
            m_iFiredCount = 0;
            m_fNextFireTime = 0.f;
        }

        if (m_fAccTime < fCycle && m_fAccTime >= m_fNextFireTime && m_iFiredCount < 5)
        {
            const _int iBulletPerShot = 5;

            for (_int i = 0; i < iBulletPerShot; ++i)
            {
                CHellBoss_Bullet::PowerBlastDesc pDesc{};
                pDesc.wBulletType = L"0_Phase4_Shoot";
                pDesc.isLeft = (rand() % 2 == 0);
                pDesc.iPatternType = 0; // 

                if (!pBoss->Get_GameInstance()->Add_GameObject_FromPool(
                    LEVEL_HONG, LEVEL_HONG,
                    TEXT("Layer_HellBoss_PHASE4_Bullet"), &pDesc))
                {
                    MSG_BOX("HellBoss_Bullet 생성 실패");
                }
           // event: / Weapons / rocketlauncher_shot
                pBoss->m_pSoundCom->Play_Event(L"event:/Weapons/Rocketlauncher_shot")->SetVolume(0.1f);
            }
            for (_int i = 0; i < iBulletPerShot; ++i)// 발사이펙트용 
            {
                CHellBoss_Bullet::PowerBlastDesc pDesc{};// 발사이펙트용 
                pDesc.wBulletType = L"0_Phase4_Shoot";// 발사이펙트용 
                pDesc.isLeft = (rand() % 2 == 0);// 발사이펙트용 
                pDesc.iPatternType = 1; // 발사이펙트용 

                if (!pBoss->Get_GameInstance()->Add_GameObject_FromPool(// 발사이펙트용 
                    LEVEL_HONG, LEVEL_HONG,
                    TEXT("Layer_HellBoss_PHASE4_Bullet"), &pDesc))// 발사이펙트용 
                {
                    MSG_BOX("HellBoss_Bullet 생성 실패");// 발사이펙트용 
                }
            }

            ++m_iFiredCount;
            m_fNextFireTime += 0.15f;
        }
    }


    if (pBoss->Get_Phase() == PHASE1)
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
                pBoss->m_pSoundCom->Play_Event(L"event:/Weapons/Range/slugshot_reworked_shot")->SetVolume(0.3f);
            }
        }
        if (pBoss->Get_AnimationFinished())
        {
            m_bStarted = false;
            pBoss->Change_State(new CHellBoss_IdleState());
        }
    }

    else if (pBoss->Get_Phase() == PHASE2)
    {
        _float3 vToPlayer = pBoss->Get_PlayerPos() - pBoss->Get_Pos();
        float fDist = D3DXVec3Length(&vToPlayer);

        // 사거리 안에 있을 때만 계속 발사
        if (fDist < 50.f)
        {
            if (m_fAccTime >= m_fNextFireTime)
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

                pBoss->m_pSoundCom->Play_Event(L"event:/Weapons/Range/slugshot_reworked_shot")->SetVolume(0.2f);
                ++m_iFiredCount;
                m_fNextFireTime += 0.2f;
            }
        }
        else
        {
            m_bFinished = true;
        }
    }


    if (pBoss->Get_Phase() == PHASE3)
    {
        int iCurFrame = pBoss->Get_CurAnimationFrame();
        if (iCurFrame >= 130 && !m_bHasFired)
        {
            m_bHasFired = true;

            for (int i = 0; i < 2; ++i)
            {
                CHellBoss_Bullet::PowerBlastDesc pDesc{};
                pDesc.wBulletType = L"O_ArmCut_Shoot";
                pDesc.isLeft = (i % 2 == 0);

                pBoss->Get_GameInstance()->Add_GameObject_FromPool(
                    LEVEL_HONG, LEVEL_HONG,
                    TEXT("Layer_HellBoss_PHASE2_HandBullet"), &pDesc);
            }
        }

        if (pBoss->Get_AnimationFinished())
        {
            m_bStarted = false;
            m_bHasFired = false;
        }
    }




}




void CPattern_Shoot::Update(CHellBoss* pBoss, float fDeltaTime)
{

}


