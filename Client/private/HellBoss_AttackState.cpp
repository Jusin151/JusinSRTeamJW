#include "HellBoss.h"
#include "HellBoss_AttackState.h"
#include "HellBoss_IdleState.h"
#include "HellBoss_Bullet.h"
#include "GameInstance.h" 


void CHellBoss_AttackState::Enter(CHellBoss* pBoss)
{
    // 공격 애니메이션 설정
    pBoss->Set_Animation("3_EyeBlast");
   
        if (FAILED(pBoss->Get_GameInstance()->Add_GameObject(LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss_Bullet"),
            LEVEL_HONG, TEXT("Layer_HellBoss_Bullet"))))
        {
            MSG_BOX("HellBoss_Bullet 생성 실패");
        }

}

#include "Pattern_EyeBlast.h"
#include "HellBoss_Bullet.h" // 필요 시

void CHellBoss_AttackState::Update(CHellBoss* pBoss, float fDeltaTime)
{
    // EyeBlast일 때만 총알 발사
    if (dynamic_cast<CPattern_EyeBlast*>(pBoss->Get_AttackPattern()))
    {
        static bool bHasFired = false;

        if (!bHasFired && pBoss->Get_CurAnimationFrame() >= 15)
        {
            bHasFired = true;

            if (FAILED(pBoss->Get_GameInstance()->Add_GameObject(
                LEVEL_HONG,
                TEXT("Prototype_GameObject_HellBoss_Bullet"),
                LEVEL_HONG,
                TEXT("Layer_HellBoss_Bullet"))))
            {
                MSG_BOX("HellBoss_Bullet 생성 실패");
            }
        }

        if (pBoss->Get_AnimationFinished())
            bHasFired = false; // 애니메이션 종료 후 초기화
    }

    pBoss->Use_Attack(fDeltaTime);
}


void CHellBoss_AttackState::Exit(CHellBoss* pBoss)
{
	
}
