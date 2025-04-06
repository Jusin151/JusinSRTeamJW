#include "HellBoss_IdleState.h"
#include "HellBoss.h"
#include "Pattern_EyeBlast.h"
#include "Pattern_Shoot.h"
#include "HellBoss_AttackState.h"
#include "HellBoss_WalkState.h" // 추가 필요

void CHellBoss_IdleState::Enter(CHellBoss* pBoss)
{
    if (pBoss->Get_Phase() == CHellBoss::PHASE1)
        pBoss->Set_Animation("1_Idle");
    else if (pBoss->Get_Phase() == CHellBoss::PHASE2)
        pBoss->Set_Animation("6_Phase2_Idle");
}


void CHellBoss_IdleState::Update(CHellBoss* pBoss, float fDeltaTime)
{
	if (!pBoss->HasTarget()) return;

	_float3 vToPlayer = pBoss->Get_PlayerPos() - pBoss->Get_Pos();
	float fDist = D3DXVec3Length(&vToPlayer);

    if (fDist < 20.f) // 공격 거리
    {
        // 공격 패턴 실행
        int randIndex = rand() % 2;
        switch (randIndex)
        {
        case 0: pBoss->Set_AttackPattern(new CPattern_EyeBlast()); break;
        case 1: pBoss->Set_AttackPattern(new CPattern_Shoot()); break;
        }

        pBoss->Change_State(new CHellBoss_AttackState());
        return;
    }
    else // 공격 거리가 아니면 무조건 추노
    {
        pBoss->Change_State(new CHellBoss_WalkState());
        return;
    }


	pBoss->Set_Animation("1_Idle");
}

void CHellBoss_IdleState::Exit(CHellBoss* pBoss)
{

}
