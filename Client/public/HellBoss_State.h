#pragma once

class CHellBoss;

class CHellBoss_State
{
public:
    virtual ~CHellBoss_State() = default;

    virtual void Enter(CHellBoss* pBoss) = 0;
    virtual void Update(CHellBoss* pBoss, float fDeltaTime) = 0;
    virtual void Exit(CHellBoss* pBoss) = 0;
};
