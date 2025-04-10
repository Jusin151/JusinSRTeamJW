#include "HellBoss_MorphState.h"
#include "HellBoss.h"


_float upwardSpeed = {};

void CHellBoss_MorphState::Enter(CHellBoss* pBoss)
{
    upwardSpeed = 5.f;
}

void CHellBoss_MorphState::Update(CHellBoss* pBoss, float fDeltaTime)
{
 
    if (pBoss->Get_Phase() == PHASE4)
    {
        // 기존 위치에서 수직 상승
        _float3 pos = pBoss->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        pos.y += fDeltaTime * upwardSpeed; 
        pBoss->Get_Transform()->Set_State(CTransform::STATE_POSITION, pos);
    }

	if (pBoss)
		pBoss->Use_Attack(fDeltaTime);

}

void CHellBoss_MorphState::Exit(CHellBoss* pBoss)
{
}
