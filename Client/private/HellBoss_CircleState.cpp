#include "HellBoss.h"
#include "HellBoss_CircleState.h"
#include "HellBoss_IdleState.h"
#include "HellBoss_Bullet.h"
#include "Pattern_Shoot.h"
#include "Pattern_Warp.h"
#include "HellBoss_WalkState.h"
#include "GameInstance.h" 

void CHellBoss_CircleState::Enter(CHellBoss* pBoss)
{

    if (pBoss->Get_Phase() == PHASE4)
        pBoss->Set_Animation("G_Phase3_Idle");


}

void CHellBoss_CircleState::Update(CHellBoss* pBoss, float fTimeDelta)
{
    _float3 playerPos = static_cast<CPlayer*>(pBoss->Get_Target())->Get_TransForm()->Get_State(CTransform::STATE_POSITION);
    const float radius = 30.f;
    const float angularSpeed = D3DX_PI / 4; // 45도/초
    pBoss->_angle += fTimeDelta * angularSpeed;
    _float3 orbitPos;
    orbitPos.x = playerPos.x + cos(pBoss->_angle) * radius;
    orbitPos.z = playerPos.z + sin(pBoss->_angle) * radius;
    // 고정된 높이 (예: 플레이어 높이에서 +20.f)
    orbitPos.y = playerPos.y + 20.f;
    pBoss->Get_Transform()->Set_State(CTransform::STATE_POSITION, orbitPos);
}



void CHellBoss_CircleState::Exit(CHellBoss* pBoss)
{
	
}
