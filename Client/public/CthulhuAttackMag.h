#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "CompositeNode.h"

BEGIN(Client)
class CCthulhu;

class CCthulhuAttackMag
{
public:

    CCthulhuAttackMag(CCthulhu* pOwner);
    ~CCthulhuAttackMag() = default;

    // 각 공격 실행 함수
    NodeStatus Execute_MissileAttack();
    NodeStatus Execute_MultiMissileAttack();
    NodeStatus Execute_SpikeAttack();
    NodeStatus Execute_TentacleDeploy();
    NodeStatus Execute_BigTentacleDeploy();

private:
	CCthulhu* m_pOwner = nullptr;
};
END