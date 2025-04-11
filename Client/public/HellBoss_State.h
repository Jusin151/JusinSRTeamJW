#pragma once

#include "Client_Defines.h"

class CHellBoss;

class CHellBoss_State
{
public:
    virtual ~CHellBoss_State()PURE;

    virtual void Enter(CHellBoss* pBoss) = 0;
    virtual void Update(CHellBoss* pBoss, float fTimeDelta) = 0;
    virtual void Exit(CHellBoss* pBoss) = 0;


};
