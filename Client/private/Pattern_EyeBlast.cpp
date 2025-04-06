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

    if (!m_bHasFired && pBoss->Get_CurAnimationFrame() >= 17)
    {
        m_bHasFired = true;
        m_wBulletType = L"3_EyeBlast";
        if (FAILED(pBoss->Get_GameInstance()->Add_GameObject(
            LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss_Bullet"),
            LEVEL_HONG, TEXT("Layer_HellBoss_Bullet"),&m_wBulletType)))
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

