#include "HellBoss_AttackState.h"
#include "HellBoss.h"
#include "HellBoss_IdleState.h"

void CHellBoss_AttackState::Enter(CHellBoss* pBoss)
{
	// 보스가 애니메이션 안에 패턴명을 포함하고 있다면, 패턴 쪽에서 Set_Animation 할 수도 있음
	// 또는 여기서 기본 애니를 지정해줘도 됨
}

void CHellBoss_AttackState::Update(CHellBoss* pBoss, float fDeltaTime)
{

	pBoss->Use_Attack(fDeltaTime);

	auto pPattern = pBoss->Get_AttackPattern();
	if (pPattern && pPattern->Is_Finished())
	{
	
		delete pPattern;
		pBoss->Set_AttackPattern(nullptr);

		pBoss->Change_State(new CHellBoss_IdleState()); 
	}
}

void CHellBoss_AttackState::Exit(CHellBoss* pBoss)
{
	
}
