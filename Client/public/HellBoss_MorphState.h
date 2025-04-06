#pragma once

#include "HellBoss_State.h"

class CHellBoss_MorphState : public CHellBoss_State
{
public:
    void Enter(CHellBoss* pBoss) override;
    void Update(CHellBoss* pBoss, float fDeltaTime) override;
    void Exit(CHellBoss* pBoss) override;
};
