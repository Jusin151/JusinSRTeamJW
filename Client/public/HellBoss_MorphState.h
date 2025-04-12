#pragma once

#include "HellBoss_State.h"
#include "Client_Defines.h"

class CHellBoss_MorphState : public CHellBoss_State
{
public:
    void Enter(CHellBoss* pBoss) override;
    void Update(CHellBoss* pBoss, float fTimeDelta) override;
    void Exit(CHellBoss* pBoss) override;

};
