#include "HellBoss.h"
#include "HellBoss_AttackState.h"
#include "HellBoss_IdleState.h"
#include "HellBoss_Bullet.h"
#include "Pattern_Shoot.h"
#include "Pattern_Warp.h"
#include "HellBoss_WalkState.h"
#include "GameInstance.h" 

void CHellBoss_AttackState::Enter(CHellBoss* pBoss)
{
    m_bAttackFinished = false;
    m_fDelayTimer = 0.f;
    m_eAttackSubState = ATTACK_ENTER;

    if (pBoss->Get_Phase() == PHASE1)
    {
        pBoss->Set_Animation("4_Shoot");
        m_fDelayDuration = 1.2f;
    }
    else if (pBoss->Get_Phase() == PHASE2)
    {
        pBoss->Set_Animation("0_Phase2_Shoot");
        m_fDelayDuration = 0.9f;
    }
    else if (pBoss->Get_Phase() == PHASE3)
    {
        pBoss->Set_Animation("O_ArmCut_Attack");
        m_fDelayDuration = 1.0f;
    }
    else if (pBoss->Get_Phase() == PHASE4)
    {
        pBoss->Set_Animation("J_Phase3_TripleEye");
        m_fDelayDuration = 1.3f;
    }
    else if (pBoss->Get_Phase() == PHASE5)
    {
        pBoss->Set_Animation("N_Phase4_Idle");
        m_fDelayDuration = 1.5f;
    }
}


void CHellBoss_AttackState::Update(CHellBoss* pBoss, float fDeltaTime)
{
    if (m_eAttackSubState == ATTACK_ENTER)
    {
        if (!m_bAttackFinished)
        {
            pBoss->Use_Attack(fDeltaTime);
            auto pPattern = pBoss->Get_AttackPattern();
            if (pPattern && pPattern->Is_Finished())
            {
                m_bAttackFinished = true;
                m_fDelayTimer = 0.f;
            }
            return;
        }

        m_fDelayTimer += fDeltaTime;
        if (m_fDelayTimer < m_fDelayDuration)
            return;

        // 진입 끝나고 루프로 전환
        m_eAttackSubState = ATTACK_LOOP;
        m_bAttackFinished = false;
        m_fDelayTimer = 0.f;

        if (pBoss->Get_Phase() == PHASE3)
        {
            pBoss->Set_Animation("O_ArmCut_Attack_Roof");
            m_fDelayDuration = 1.0f; // 루프용 딜레이
        }
        else
        {
            // 다른 페이즈는 공격 1회 후 바로 Idle이나 Walk로 전환
            DecideNextAction(pBoss);
        }

        return;
    }

    // 반복 공격 루프
    else if (m_eAttackSubState == ATTACK_LOOP)
    {
        if (!m_bAttackFinished)
        {
            pBoss->Use_Attack(fDeltaTime);
            auto pPattern = pBoss->Get_AttackPattern();
            if (pPattern && pPattern->Is_Finished())
            {
                m_bAttackFinished = true;
                m_fDelayTimer = 0.f;
            }
            return;
        }

        m_fDelayTimer += fDeltaTime;
        if (m_fDelayTimer < m_fDelayDuration)
            return;

        // 다음 루프 공격 시작
        m_bAttackFinished = false;
        m_fDelayTimer = 0.f;

        pBoss->Set_Animation("O_ArmCut_Attack_Roof");
        pBoss->Set_Pattern(new CPattern_Shoot());
    }
}


void CHellBoss_AttackState::DecideNextAction(CHellBoss* pBoss)
{
    _float3 vToPlayer = pBoss->Get_PlayerPos() - pBoss->Get_Pos();
    float fDist = D3DXVec3Length(&vToPlayer);

    if (fDist < 30.f)
        pBoss->Set_Pattern(new CPattern_Shoot());
    else
        pBoss->Change_State(new CHellBoss_WalkState());
}



void CHellBoss_AttackState::Exit(CHellBoss* pBoss)
{
	
}
