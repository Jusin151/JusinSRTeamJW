#include "HellBoss_WalkState.h"
#include "HellBoss.h"
#include "HellBoss_IdleState.h"

void CHellBoss_WalkState::Enter(CHellBoss* pBoss)
{
	pBoss->Set_Animation("2_Walk");
}

void CHellBoss_WalkState::Update(CHellBoss* pBoss, float fDeltaTime)
{
	_float3 vToPlayer = pBoss->Get_PlayerPos() - pBoss->Get_Pos();
	float fDist = D3DXVec3Length(&vToPlayer);

	D3DXVec3Normalize(&vToPlayer, &vToPlayer);

	_float3 vNewPos = pBoss->Get_Pos() + vToPlayer * pBoss->Get_Speed() * fDeltaTime;
	pBoss->Get_Transform()->Set_State(CTransform::STATE_POSITION, vNewPos);

	pBoss->Set_CurPos(vNewPos);

	
	if (fDist < 8.f) // 플레이어랑 거리가 8보다 가까우면 Idle 
	{
		pBoss->Change_State(new CHellBoss_IdleState());
		return;
	}
}

void CHellBoss_WalkState::Exit(CHellBoss* pBoss)
{
}
