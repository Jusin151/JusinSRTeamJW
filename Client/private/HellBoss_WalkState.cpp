#include "HellBoss_WalkState.h"
#include "HellBoss.h"



void CHellBoss_WalkState::Enter(CHellBoss* pBoss)
{
	if (pBoss->Get_Phase() == PHASE1)
		pBoss->Set_Animation("2_Walk");
	else if (pBoss->Get_Phase() == PHASE2)
		pBoss->Set_Animation("7_Phase2_Walk");
	else if (pBoss->Get_Phase() == PHASE3)
		pBoss->Set_Animation("I_ArmCut_Walk");
	else if (pBoss->Get_Phase() == PHASE4)
		pBoss->Set_Animation("H_Phase3_Walk");
	else if (pBoss->Get_Phase() == PHASE5)
		pBoss->Set_Animation("N_Phase4_Idle");
}


void CHellBoss_WalkState::Update(CHellBoss* pBoss, float fDeltaTime)
{
	_float3 vToPlayer = pBoss->Get_PlayerPos() - pBoss->Get_Pos();
	float fDist = D3DXVec3Length(&vToPlayer);

	D3DXVec3Normalize(&vToPlayer, &vToPlayer);


	if (pBoss->Get_Phase() == PHASE3)
		vToPlayer *= -1.f;

	_float3 vNewPos = pBoss->Get_Pos() + vToPlayer * pBoss->Get_Speed() * fDeltaTime;
	pBoss->Get_Transform()->Set_State(CTransform::STATE_POSITION, vNewPos);

	if (fDist < 20.f)
	{
		pBoss->Change_State(new CHellBoss_IdleState());
		return;
	}
}



void CHellBoss_WalkState::Exit(CHellBoss* pBoss)
{
}

