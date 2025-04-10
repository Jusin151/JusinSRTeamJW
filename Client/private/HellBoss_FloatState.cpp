#include "HellBoss.h"
#include "HellBoss_FloatState.h"
#include "HellBoss_IdleState.h"
#include "HellBoss_Bullet.h"
#include "Pattern_Shoot.h"
#include "Pattern_Warp.h"
#include "HellBoss_WalkState.h"
#include "HellBoss_CircleState.h"
#include "GameInstance.h" 

void CHellBoss_FloatState::Enter(CHellBoss* pBoss)
{
    if (pBoss->Get_Phase() == PHASE4)
        pBoss->Set_Animation("P_ArmCut_End");

    pBoss->Set_CurPos(pBoss->Get_Transform()->Get_State(CTransform::STATE_POSITION));
}


void CHellBoss_FloatState::Update(CHellBoss* pBoss, float fTimeDelta)
{

    _float3 startPos = pBoss->Get_CurPos(); // 저장한 위치 기준
    startPos.y += fTimeDelta * 5.f;
    pBoss->Get_Transform()->Set_State(CTransform::STATE_POSITION, startPos);



    if (pBoss->Get_AnimationManager()->IsFinished())
    {
        // 부유 상태 종료 후 orbit 상태로 전환
        pBoss->Change_State(new CHellBoss_CircleState());
    }
}


void CHellBoss_FloatState::Exit(CHellBoss* pBoss)
{
	
}
