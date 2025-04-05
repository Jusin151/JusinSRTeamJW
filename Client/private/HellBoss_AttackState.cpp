#include "HellBoss_AttackState.h"
#include "HellBoss.h"

void CHellBoss_AttackState::Enter(CHellBoss* pBoss)
{
}

void CHellBoss_AttackState::Update(CHellBoss* pBoss, float fDeltaTime)
{
	// ºö °ø°Ý
	pBoss->Use_Attack(fDeltaTime);
}


void CHellBoss_AttackState::Exit(CHellBoss* pBoss)
{
}
