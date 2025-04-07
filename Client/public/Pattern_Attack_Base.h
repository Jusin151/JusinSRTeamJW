#pragma once

#include "Client_Defines.h"

class CHellBoss;

class CPattern_Attack_Base
{
public:
    virtual ~CPattern_Attack_Base() {}
    virtual void Execute(CHellBoss* pBoss, float fDeltaTime) = 0;
    virtual void Update(CHellBoss* pBoss, float fDeltaTime) = 0;
    virtual bool Is_Finished() const { return false; } 
protected:

};
