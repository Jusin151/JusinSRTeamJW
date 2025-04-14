#pragma once

#include "HellBoss_State.h"
#include "Client_Defines.h"

enum STEP 
{
    STEP_JUMP,
    STEP_WAIT_BETWEEN_JUMPS,
    STEP_WAIT,
    STEP_ATTACK
};


class CHellBoss_JumpLoopState : public CHellBoss_State
{

    void Enter(CHellBoss* pBoss) override;

    void Update(CHellBoss* pBoss, float fDeltaTime) override;



private:
    STEP m_eStep = STEP_JUMP;
    _int m_iJumpCount = {};
    _float m_fWaitTimer = { 0.f };
    _float m_fJumpIntervalTimer{};

       
    void Exit(CHellBoss* pBoss) override;

};


