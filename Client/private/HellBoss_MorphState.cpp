﻿#include "HellBoss_MorphState.h"
#include "HellBoss_CircleState.h"
#include "HellBoss.h"
#include "Camera_CutScene.h"
#include "HellBoss_IdleState.h"


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
            pCutCam->Set_CameraDisableDelay(3.f);          // 3초 후 자동으로꺼지게
        }
    }
    if (pBoss->Get_Phase() == PHASE2)
    {
        pBoss->m_pSoundCom->Play_Event(L"event:/Monsters/Satan/satan_transform_1to2")->SetVolume(0.7f);
    }
    if (pBoss->Get_Phase() == PHASE3)
    {
        pBoss->m_pSoundCom->Play_Event(L"event:/Monsters/Satan/satan_transform_2to3")->SetVolume(0.7f);
    }
    if (pBoss->Get_Phase() == PHASE4)
    {
        pBoss->m_pSoundCom->Play_Event(L"event:/Monsters/Satan/satan_transform_3to4")->SetVolume(0.7f);

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
    if (pBoss->Get_Phase() == PHASE5)
    {
        pBoss->m_pSoundCom->Play_Event(L"event:/Monsters/Satan/satan_transform_3to4")->SetVolume(0.7f);

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
            pCutCam->Set_CameraDisableDelay(4.5f); // 몇초뒤에 카메라끌껀지
        }

        pBoss->m_pSoundCom->Play_Event(L"event:/Monsters/Satan/satan_transform_2to3")->SetVolume(0.7f);
    }

    upwardSpeed = 3.f;
   // upwardSpeed = 0.f;
}

void CHellBoss_MorphState::Update(CHellBoss* pBoss, float fDeltaTime)
{
    static _float fFallSpeed = 0.f;
    static const _float fGravity = 98.f;
    static _bool bLandingEffectPlayed = false;
    static _float fLandingEffectTimer = 0.f;
    static _bool bStartFalling = false; //  낙흐앙!!!

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

    // 5페이즈: 변신 애니메이션 끝나면 낙하 시작!!!!
    if (pBoss->Get_Phase() == PHASE5)
    {
        if (!bStartFalling)
        {
            if (pBoss->Get_AnimationFinished())
            {
                bStartFalling = true; 
                fFallSpeed = 0.f;
            }
            else
            {
                return; 
            }
        }

        _float3 pos = pBoss->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        fFallSpeed += fGravity * fDeltaTime;
        pos.y -= fFallSpeed * fDeltaTime;

        if (pos.y <= pBoss->Get_FloorOffset() + 0.5f)
        {
            pos.y = pBoss->Get_FloorOffset();
            pBoss->Get_Transform()->Set_State(CTransform::STATE_POSITION, pos);

            if (!bLandingEffectPlayed)
            {
                bLandingEffectPlayed = true;
                fLandingEffectTimer = 0.f;

                BossDESC desc{};
                desc.vRight = pBoss->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
                desc.vUp = pBoss->Get_Transform()->Get_State(CTransform::STATE_UP);
                desc.vLook = pBoss->Get_Transform()->Get_State(CTransform::STATE_LOOK);
                desc.vPos = pos;
                desc.strState = "Down";

                pBoss->Get_GameInstance()->Add_GameObject(
                    LEVEL_HONG,
                    TEXT("Prototype_GameObject_HellBoss_Skill_Landing"),
                    LEVEL_HONG,
                    TEXT("Layer_HellBoss_Skill_Landing"),
                    &desc);

                pBoss->m_pSoundCom->Play_Event(L"event:/Monsters/Down")->SetVolume(1.f);
            }
            if (pBoss->Get_AnimationManager()->GetCurrentFrame() >= 280)
            {
                pBoss->Change_State(new CHellBoss_IdleState());
            }
            fLandingEffectTimer += fDeltaTime;
            if (fLandingEffectTimer >= 2.f)
            {
                bLandingEffectPlayed = false;
                fLandingEffectTimer = 0.f;
                bStartFalling = false;
                fFallSpeed = 0.f;
             
             
            }
        }
        else
        {
            pBoss->Get_Transform()->Set_State(CTransform::STATE_POSITION, pos);
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
