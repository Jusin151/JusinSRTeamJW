#include "HellBoss_MorphState.h"
#include "HellBoss_CircleState.h"
#include "HellBoss.h"
#include "Camera_CutScene.h"


_float upwardSpeed = {};

void CHellBoss_MorphState::Enter(CHellBoss* pBoss)
{

    if (pBoss->Get_Phase() == PHASE4)
    {
        _float3 vBossPos = pBoss->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        _float3 vLook = pBoss->Get_Transform()->Get_State(CTransform::STATE_LOOK);
        _float3 vUp = pBoss->Get_Transform()->Get_State(CTransform::STATE_UP);

        vLook.Normalize();
        vUp.Normalize();

        _float3 vStart = vBossPos - vLook * 30.f + vUp * -5.f;  // 보스 뒤 + 위에서 시작
        _float3 vEnd = vBossPos + vUp * 50.f;                // 보스 위로 올라가며

        _float3 vArgs[2] = { vStart, vEnd };


        pBoss->Get_GameInstance()->Add_GameObject(
            LEVEL_STATIC,
            TEXT("Prototype_GameObject_Camera_CutScene"),
            LEVEL_HONG,
            TEXT("Layer_Camera"),
            vArgs);

        CGameObject* pObj = pBoss->Get_GameInstance()->Find_Last_Object(LEVEL_HONG, TEXT("Layer_Camera"));
        CCamera_CutScene* pCutCam = dynamic_cast<CCamera_CutScene*>(pObj);
        if (pCutCam) 
        {
            pCutCam->Set_LookTarget(pBoss);
            pCutCam->Set_CutSceneMove(vStart, vEnd, 0.5f);
            pCutCam->Set_CameraDisableDelay(4.f);

        }

    }
    upwardSpeed = 5.f;
}

void CHellBoss_MorphState::Update(CHellBoss* pBoss, float fDeltaTime)
{
    if (pBoss->Get_Phase() == PHASE4)
    {
        _float3 pos = pBoss->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        pos.y += fDeltaTime * upwardSpeed;
        pBoss->Get_Transform()->Set_State(CTransform::STATE_POSITION, pos);

    
    }


  /*  if (pBoss->Get_AttackPattern() && pBoss->Get_AttackPattern()->Is_Finished()&&pBoss->Get_Phase() == PHASE4)
    {

        _float3 finalPos = pBoss->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        pBoss->Set_CircleBasePos(finalPos);

        pBoss->Set_Pattern(nullptr);

        pBoss->Change_State(new CHellBoss_CircleState());
    }*/


    if (pBoss->Get_AttackPattern())
        pBoss->Use_Attack(fDeltaTime);

   
}


void CHellBoss_MorphState::Exit(CHellBoss* pBoss)
{
    /*if (pBoss->Get_Phase() == PHASE4)
    {
        _float3 finalPos = pBoss->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        pBoss->Set_CircleBasePos(finalPos);
    }*/
}
