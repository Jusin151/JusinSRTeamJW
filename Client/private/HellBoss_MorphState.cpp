#include "HellBoss_MorphState.h"
#include "HellBoss.h"

void CHellBoss_MorphState::Enter(CHellBoss* pBoss)
{
}

void CHellBoss_MorphState::Update(CHellBoss* pBoss, float fDeltaTime)
{
	if (pBoss)
		pBoss->Use_Attack(fDeltaTime);

}

void CHellBoss_MorphState::Exit(CHellBoss* pBoss)
{
}
