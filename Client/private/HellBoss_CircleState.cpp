#include "HellBoss.h"
#include "HellBoss_CircleState.h"
#include "HellBoss_IdleState.h"
#include "HellBoss_Bullet.h"
#include "Pattern_Shoot.h"
#include "Pattern_Warp.h"
#include "HellBoss_WalkState.h"
#include "GameInstance.h" 

static _float3 m_vCenterPos{};

void CHellBoss_CircleState::Enter(CHellBoss* pBoss)
{
    if (pBoss->Get_Phase() == PHASE4)
        pBoss->Set_Animation("H_Phase3_Walk");

    _float3 pos = pBoss->Get_Transform()->Get_State(CTransform::STATE_POSITION);

    const float radius = 10.f;

    _float angle = 0.f; 
    pBoss->m_fAngle = angle;

    _float3 center;
    center.x = pos.x - cosf(angle) * radius;
    center.z = pos.z - sinf(angle) * radius;
    center.y = pos.y;

    m_vCenterPos = center;

    pBoss->Set_Pattern(new CPattern_Shoot());
}



void CHellBoss_CircleState::Update(CHellBoss* pBoss, float fTimeDelta)
{
    const float radius = 30.f;
    const float angularSpeed = D3DX_PI / 4.f; 

    pBoss->m_fAngle += fTimeDelta * angularSpeed;

    _float3 orbitPos;
    orbitPos.x = m_vCenterPos.x + cos(pBoss->m_fAngle) * radius;
    orbitPos.z = m_vCenterPos.z + sin(pBoss->m_fAngle) * radius;
    orbitPos.y = m_vCenterPos.y;

    pBoss->Get_Transform()->Set_State(CTransform::STATE_POSITION, orbitPos);


 

    if (pBoss->Get_AttackPattern())
        pBoss->Use_Attack(fTimeDelta);
}




void CHellBoss_CircleState::Exit(CHellBoss* pBoss)
{

}
