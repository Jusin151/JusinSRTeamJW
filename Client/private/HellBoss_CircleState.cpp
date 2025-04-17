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

    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(
        CGameInstance::Get_Instance()->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));

    if (pPlayerTransform)
    {
        m_vCenterPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

        m_vCenterPos.y = pBoss->Get_Transform()->Get_State(CTransform::STATE_POSITION).y;
    }

    pBoss->m_fAngle = 0.f;
    pBoss->Set_Pattern(new CPattern_Shoot());
}




void CHellBoss_CircleState::Update(CHellBoss* pBoss, float fTimeDelta)
{
   const float radius = 50.f;
   // const float radius = 0.f;
    const float angularSpeed = D3DX_PI / 4.f;

    pBoss->m_fAngle += fTimeDelta * angularSpeed;

    _float3 orbitPos;
    orbitPos.x = m_vCenterPos.x + cosf(pBoss->m_fAngle) * radius;
    orbitPos.z = m_vCenterPos.z + sinf(pBoss->m_fAngle) * radius;
    orbitPos.y = m_vCenterPos.y; // 고정된 지상 높이

    pBoss->Get_Transform()->Set_State(CTransform::STATE_POSITION, orbitPos);

    if (pBoss->Get_AttackPattern())
        pBoss->Use_Attack(fTimeDelta);
}





void CHellBoss_CircleState::Exit(CHellBoss* pBoss)
{

}
