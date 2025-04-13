#pragma once

#include "HellBoss_State.h"

enum ATTACK_SUBSTATE
{
    ATTACK_ENTER,
    ATTACK_LOOP
};


class CHellBoss_AttackState : public CHellBoss_State
{
public:
    void Enter(CHellBoss* pBoss) override;
    void Update(CHellBoss* pBoss, float fDeltaTime) override;
    void DecideNextAction(CHellBoss* pBoss);
    void Exit(CHellBoss* pBoss) override;

    _bool m_bAttackFinished = false;
    _float m_fDelayTimer = 0.f;
    _float m_fDelayDuration = { 1.5f };

    ATTACK_SUBSTATE m_eAttackSubState = ATTACK_ENTER;

};
