#include "HellBoss_IdleState.h"
#include "HellBoss.h"
#include "Pattern_EyeBlast.h"
#include "Pattern_Shoot.h"
#include "HellBoss_AttackState.h"
#include "HellBoss_WalkState.h" // 추가 필요

void CHellBoss_IdleState::Enter(CHellBoss* pBoss)
{
    if (pBoss->Get_Phase() == PHASE1)
        pBoss->Set_Animation("1_Idle");
    else if (pBoss->Get_Phase() == PHASE2)
        pBoss->Set_Animation("8_Phase2_Charge"); // 애매해서 일던 idle로 박았음 =
	else if (pBoss->Get_Phase() == PHASE3)
		pBoss->Set_Animation("U_ArmCut_Idle");
	else if (pBoss->Get_Phase() == PHASE4)
		pBoss->Set_Animation("G_Phase3_Idle"); 
	else if (pBoss->Get_Phase() == PHASE5)
		pBoss->Set_Animation("N_Phase4_Idle");
}

void CHellBoss_IdleState::Update(CHellBoss* pBoss, float fDeltaTime)
{
	if (!pBoss->HasTarget()) return;

	_float3 vToPlayer = pBoss->Get_PlayerPos() - pBoss->Get_Pos();
	float fDist = D3DXVec3Length(&vToPlayer);

	if (fDist < 20.f) // 공격 거리 안에 들었으면@@@#!#!@#
	{
		if (pBoss->Get_Phase() == PHASE1)
		{
			// 페이즈1은 둘 중 하나 랜덤
			int randIndex = rand() % 2;
			switch (randIndex)
			{
			case 0: pBoss->Set_AttackPattern(new CPattern_EyeBlast()); break;
			case 1: pBoss->Set_AttackPattern(new CPattern_Shoot()); break;
			}
		}
		else if (pBoss->Get_Phase() == PHASE2)
		{
			// 페이즈2는 Shoot만 가능하니까
			pBoss->Set_AttackPattern(new CPattern_Shoot());
		}
		else
		{
			// 이후 페이즈에서 다른 패턴 추가 시 여기에
		}

		pBoss->Change_State(new CHellBoss_AttackState());
		return;
	}
	else // 공격 거리 바깥이면 추노
	{
		pBoss->Change_State(new CHellBoss_WalkState());
		return;
	}

	//  이건 사실상 도달하지 않을듯? 일단 
	pBoss->Set_Animation("1_Idle");
}


void CHellBoss_IdleState::Exit(CHellBoss* pBoss)
{

}
