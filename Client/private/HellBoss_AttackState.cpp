#include "HellBoss.h"
#include "HellBoss_AttackState.h"
#include "HellBoss_IdleState.h"
#include "HellBoss_Bullet.h"
#include "Pattern_Shoot.h"
#include "HellBoss_WalkState.h"
#include "GameInstance.h" 

void CHellBoss_AttackState::Enter(CHellBoss* pBoss)
{
	if (pBoss->Get_Phase() == PHASE1)
		pBoss->Set_Animation("4_Shoot");
	else if (pBoss->Get_Phase() == PHASE2)
		pBoss->Set_Animation("0_Phase2_Shoot");
    else if (pBoss->Get_Phase() == PHASE3)
        pBoss->Set_Animation("O_ArmCut_Attack");
    else if (pBoss->Get_Phase() == PHASE4)
        pBoss->Set_Animation("J_Phase3_TripleEye");
    else if (pBoss->Get_Phase() == PHASE5)
        pBoss->Set_Animation("N_Phase4_Idle");

}

void CHellBoss_AttackState::Update(CHellBoss* pBoss, float fDeltaTime)
{
    pBoss->Use_Attack(fDeltaTime);

    auto pPattern = pBoss->Get_AttackPattern();
    if (pPattern && pPattern->Is_Finished())
    {
     
        if (pBoss->Get_Phase() == PHASE1)
        {
            pBoss->Change_State(new CHellBoss_IdleState());
        }
        else if (pBoss->Get_Phase() == PHASE2)
        {
            _float3 vToPlayer = pBoss->Get_PlayerPos() - pBoss->Get_Pos();
            float fDist = D3DXVec3Length(&vToPlayer);
            if (fDist < 20.f)
            {
                pBoss->Set_AttackPattern(new CPattern_Shoot());
            }
            else
            {
                pBoss->Change_State(new CHellBoss_WalkState());
            }
        }
        else if (pBoss->Get_Phase() == PHASE3) 
        {
            _float3 vToPlayer = pBoss->Get_PlayerPos() - pBoss->Get_Pos();
            float fDist = D3DXVec3Length(&vToPlayer);
            if (fDist < 20.f)
            {
                pBoss->Set_AttackPattern(new CPattern_Shoot());
            }
            else
            {
                pBoss->Change_State(new CHellBoss_WalkState());
            }
        }
        else if (pBoss->Get_Phase() == PHASE4)
        {
            _float3 vToPlayer = pBoss->Get_PlayerPos() - pBoss->Get_Pos(); 
            float fDist = D3DXVec3Length(&vToPlayer);
            if (fDist < 20.f)
            {
                pBoss->Set_AttackPattern(new CPattern_Shoot());
            }
            else
            {
                pBoss->Change_State(new CHellBoss_WalkState());
            }
        }
        else if (pBoss->Get_Phase() == PHASE5)
        {
            _float3 vToPlayer = pBoss->Get_PlayerPos() - pBoss->Get_Pos();
            float fDist = D3DXVec3Length(&vToPlayer);
            if (fDist < 20.f)
            {
                pBoss->Set_AttackPattern(new CPattern_Shoot());
            }
            else
            {
                pBoss->Change_State(new CHellBoss_WalkState());
            }
        }
    }
}



void CHellBoss_AttackState::Exit(CHellBoss* pBoss)
{
	
}
