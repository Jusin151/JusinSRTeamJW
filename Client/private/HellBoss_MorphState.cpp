#include "HellBoss_MorphState.h"
#include "HellBoss_CircleState.h"
#include "HellBoss.h"
#include "Camera_CutScene.h"


_float upwardSpeed = {};

void CHellBoss_MorphState::Enter(CHellBoss* pBoss)
{

    if (pBoss->Get_Phase() == PHASE1)
    {
        _float3 vBossPos = pBoss->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        _float3 vLook = pBoss->Get_Transform()->Get_State(CTransform::STATE_LOOK);
        _float3 vUp = pBoss->Get_Transform()->Get_State(CTransform::STATE_UP);

        vLook.Normalize();
        vUp.Normalize();
        vLook = _float3(-5.f, -0.3f, 0.f);

        _float3 vStart = vBossPos - vLook * 30.f + vUp * -5.f;
        _float3 vEnd = vBossPos + vLook * 30.f = vUp * 1.f;

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
            pCutCam->Set_LookTarget(pBoss);                 // 카메라는 보스를 계속 바라보게
            pCutCam->Set_CutSceneMove(vStart, vEnd, 0.5f);  // 카메라 이동
            pCutCam->Set_CameraDisableDelay(3.f);          // 4초 후 자동으로꺼지게
        }
    }
    //if (pBoss->Get_Phase() == PHASE3)
    //{
    //    _float3 vBossPos = pBoss->Get_CutScene_AnchorPos();

    //    _float3 vLookDir = pBoss->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    //    vLookDir.Normalize();

    //    _float3 vUp = _float3(0.f, 1.f, 0.f);
    //    _float3 vRight = _float3(3.f, 1.f, 10.f);
    //    _float radius = 30.f;

    //    _float3 vStart = vBossPos - vLookDir * radius + vUp * 10.f;
    //    _float3 vMid = vBossPos - vLookDir * (radius * 1.5f) + vUp;
    //    _float3 vEnd = vBossPos - vLookDir * (radius * 1.5f) + vUp;

    //    vector<_float3> vecPath = { vStart, vMid, vEnd };

    //    pBoss->Get_GameInstance()->Add_GameObject(
    //        LEVEL_STATIC,
    //        TEXT("Prototype_GameObject_Camera_CutScene"),
    //        LEVEL_HONG,
    //        TEXT("Layer_Camera"));

    //    CGameObject* pObj = pBoss->Get_GameInstance()->Find_Last_Object(LEVEL_HONG, TEXT("Layer_Camera"));
    //    CCamera_CutScene* pCutCam = dynamic_cast<CCamera_CutScene*>(pObj);
    //    if (pCutCam)
    //    {
    //        pCutCam->Set_LookTarget(pBoss); // 컷씬 카메라가 뭘 보게 할껀지
    //        pCutCam->Set_CutScenePath(vecPath, 0.4f); // 경로 넣기
    //        pCutCam->Set_CameraDisableDelay(0.6f); // 몇초뒤에 카메라끌껀지
    //    }
    //}
    if (pBoss->Get_Phase() == PHASE4)
    {
        _float3 vBossPos = pBoss->Get_CutScene_AnchorPos();

        _float3 vLookDir = pBoss->Get_Transform()->Get_State(CTransform::STATE_LOOK);
        vLookDir.Normalize();

        _float3 vUp = _float3(0.f, 1.f, 0.f);
        _float3 vRight = _float3(3.f, 1.f, 10.f);
        _float radius = 30.f;

        _float3 vStart = vBossPos - vLookDir * radius + vUp * 10.f;
        _float3 vMid = vBossPos - vLookDir * (radius * 1.5f) + vUp * 15.f;
        _float3 vEnd = vBossPos - vLookDir * (radius * 1.5f) + vUp * 25.f + vRight;

        vector<_float3> vecPath = { vStart, vMid, vEnd };

        pBoss->Get_GameInstance()->Add_GameObject(
            LEVEL_STATIC,
            TEXT("Prototype_GameObject_Camera_CutScene"),
            LEVEL_HONG,
            TEXT("Layer_Camera"));

        CGameObject* pObj = pBoss->Get_GameInstance()->Find_Last_Object(LEVEL_HONG, TEXT("Layer_Camera"));
        CCamera_CutScene* pCutCam = dynamic_cast<CCamera_CutScene*>(pObj);
        if (pCutCam)
        {
            pCutCam->Set_LookTarget(pBoss); // 컷씬 카메라가 뭘 보게 할껀지
            pCutCam->Set_CutScenePath(vecPath, 0.4f); // 경로 넣기
            pCutCam->Set_CameraDisableDelay(4.2f); // 몇초뒤에 카메라끌껀지
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

        CCamera_CutScene* pCam = dynamic_cast<CCamera_CutScene*>(
            pBoss->Get_GameInstance()->Find_Object(LEVEL_HONG, TEXT("Layer_Camera")));
        if (pCam)
        {
            _float3 vCamPos = pCam->Get_Transform()->Get_State(CTransform::STATE_POSITION);
            pBoss->Get_Transform()->LookAt(vCamPos);
        }
    }


   


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
