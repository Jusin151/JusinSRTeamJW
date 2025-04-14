#include "HellBoss.h"
#include "HellBoss_CircleState.h"
#include "HellBoss_IdleState.h"
#include "HellBoss_Bullet.h"
#include "Pattern_Shoot.h"
#include "Pattern_Warp.h"
#include "HellBoss_WalkState.h"
#include "GameInstance.h" 
#include "HellBoss_JumpLoopState.h"
#include "HellBoss_AttackState.h" 


void CHellBoss_JumpLoopState::Enter(CHellBoss* pBoss)
{
    m_eStep = STEP_JUMP;
    m_iJumpCount = 0;
    m_fWaitTimer = 0.f;
    m_fJumpIntervalTimer = 0.f;
}

void CHellBoss_JumpLoopState::Update(CHellBoss* pBoss, float fDeltaTime)
{
    switch (m_eStep)
    {
    case STEP_JUMP:
        if (!pBoss->Is_Jumping() && !pBoss->Is_Falling())
        {
            pBoss->Force_Jump();
            ++m_iJumpCount;

            if (m_iJumpCount < 3)
            {
                m_fJumpIntervalTimer = 0.f;
                m_eStep = STEP_WAIT_BETWEEN_JUMPS;
            }
            else
            {
                m_fWaitTimer = 0.f;
                m_eStep = STEP_WAIT;
            }
        }
        break;

    case STEP_WAIT_BETWEEN_JUMPS:
        m_fJumpIntervalTimer += fDeltaTime;
        if (m_fJumpIntervalTimer >= 1.0f) // 점프 간 간격
        {
            m_eStep = STEP_JUMP;
        }
        break;

    case STEP_WAIT:
        m_fWaitTimer += fDeltaTime;
        if (m_fWaitTimer >= 3.0f)
        {
            pBoss->Set_Pattern(new CPattern_Shoot());
            pBoss->Change_State(new CHellBoss_AttackState());
            m_eStep = STEP_ATTACK;
        }
        break;

    case STEP_ATTACK:
        // 패턴 끝나고 JumpLoopState로 돌아감 (AttackState에서 처리됨)
        break;
    }
}

void CHellBoss_JumpLoopState::Exit(CHellBoss* pBoss)
{
}
