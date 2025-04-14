#include "Pattern_EyeBlast.h"
#include "HellBoss.h"
#include "HellBoss_IdleState.h"
#include "HellBoss_Bullet.h"

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

    if (!m_bHasFired && pBoss->Get_CurAnimationFrame() >= 17) // 눈깔발사
    {
        pBoss->m_pSoundCom->Play_Event(L"event:/Weapons/Range/staff_huge_shot")->SetVolume(0.3f);
        //pBoss->m_pSoundCom->Play_Event(L"event:/Weapons/ssg_open")->SetVolume(0.3f);
        m_bHasFired = true;
        CHellBoss_Bullet::PowerBlastDesc pDesc{};
        pDesc.wBulletType = L"3_EyeBlast";
        if (!pBoss->Get_GameInstance()->Add_GameObject_FromPool(LEVEL_HONG, LEVEL_HONG, TEXT("Layer_HellBoss_PHASE1_EyeBullet"), &pDesc))
        {
            MSG_BOX("HellBoss_Bullet 생성 실패");
        }
    }

    m_fAccTime += fDeltaTime;

    if (pBoss->Get_AnimationFinished())
    {
        m_bStarted = false;
        m_bHasFired = false;
        m_fAccTime = 0.f;
    }
}


void CPattern_EyeBlast::Update(CHellBoss* pBoss, float fDeltaTime)
{
}

