#include "HellBoss.h"
#include "HellBoss_AttackState.h"
#include "HellBoss_IdleState.h"
#include "HellBoss_Bullet.h"
#include "GameInstance.h" 

void CHellBoss_AttackState::Enter(CHellBoss* pBoss)
{
    // 공격 애니메이션 설정
    pBoss->Set_Animation("4_Shoot");

    // 플레이어 방향 벡터 계산
    _float3 vDir = pBoss->Get_PlayerPos() - pBoss->Get_Pos();
    D3DXVec3Normalize(&vDir, &vDir);

    // 총알 생성
    //CHellBoss_Bullet* pBullet = CHellBoss_Bullet::Create(pBoss->Get_Graphic_Device());
    //if (pBullet)
    //{
    //    // 초기화
    //    pBullet->Initialize(nullptr);

    //    // 위치 지정
    //    pBullet->Get_Transform()->Set_State(CTransform::STATE_POSITION, pBoss->Get_Pos());

    //    // 방향 지정
    //    pBullet->Set_Direction(vDir);

      
        if (FAILED(pBoss->Get_GameInstance()->Add_GameObject(LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss_Bullet"),
            LEVEL_HONG, TEXT("Layer_HellBoss_Bullet"))))
        {
            MSG_BOX("HellBoss_Bullet 생성 실패");
        }

    //
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
