﻿#pragma once

#include "HellBoss_State.h"


class CHellBoss_CircleState : public CHellBoss_State
{
public:
    void Enter(CHellBoss* pBoss) override;
    void Update(CHellBoss* pBoss, float fTimeDelta) override;
    void Exit(CHellBoss* pBoss) override;

private:
};
