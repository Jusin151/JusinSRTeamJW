#pragma once

#include "HellBoss_State.h"
#include "Pattern_Shoot.h"
#include "HellBoss_AttackState.h"
#include "HellBoss_IdleState.h"
#include "HellBoss_AfterImage.h"

class CHellBoss_WalkState : public CHellBoss_State
{
public:
    void Enter(CHellBoss* pBoss) override;
    void Update(CHellBoss* pBoss, float fTimeDelta) override;
    void Exit(CHellBoss* pBoss) override;
};

class CHellBoss_DashState : public CHellBoss_State
{
public:
  /*  void Enter(CHellBoss* pBoss) override
    {
        m_fTime = 0.f;

        _float3 vDir = pBoss->Get_PlayerPos() - pBoss->Get_Pos();
        vDir.y = 0.f;
        vDir.Normalize();

        _float3 vUp(0.f, 1.f, 0.f);
        _float3 vRight;
        D3DXVec3Cross(&vRight, &vUp, &vDir);
        vRight.Normalize();

        bool bGoLeft = rand() % 2 == 0;

        _float3 vFinalDir;
        if (bGoLeft)
            vFinalDir = (-vDir + -vRight);
        else
            vFinalDir = (-vDir + vRight);

        vFinalDir.Normalize(); 
        m_vDashDir = vFinalDir; 

        pBoss->Set_Animation("I_ArmCut_Dash");
    }*/
    void Enter(CHellBoss* pBoss) override
    {
        m_fTime = 0.f;
        m_fAfterImageTimer = 0.f;

        m_vCircleCenter = pBoss->Get_PlayerPos(); // 플레이어 중심으로

        _float3 vStart = pBoss->Get_Pos();
        _float3 vToCenter = m_vCircleCenter - vStart;
        vToCenter.y = 0.f;
        m_fRadius = D3DXVec3Length(&vToCenter); // 현재 거리 그대로 유지
        m_fCurAngle = atan2f(vToCenter.z, vToCenter.x); // 현재 각도 저장

        pBoss->Set_Animation("I_ArmCut_Dash");
    }
    void Update(CHellBoss* pBoss, float fDeltaTime) override
    {
        m_fTime += fDeltaTime;
        if (m_fTime < 0.5f)
        {
            m_fCurAngle += m_fAngularSpeed * fDeltaTime; // 시계 방향 회전

            _float3 vNewPos;
            vNewPos.x = m_vCircleCenter.x + cosf(m_fCurAngle) * m_fRadius;
            vNewPos.y = pBoss->Get_Pos().y;
            vNewPos.z = m_vCircleCenter.z + sinf(m_fCurAngle) * m_fRadius;

            pBoss->Get_Transform()->Set_State(CTransform::STATE_POSITION, vNewPos);

            m_fAfterImageTimer += fDeltaTime;
            if (m_fAfterImageTimer >= m_fAfterImageInterval)
            {
                m_fAfterImageTimer = 0.f;

                CHellBoss_AfterImage::DESC desc{};
                desc.vPos = vNewPos;
                desc.vRight = pBoss->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
                desc.vUp = pBoss->Get_Transform()->Get_State(CTransform::STATE_UP);
                desc.vLook = pBoss->Get_Transform()->Get_State(CTransform::STATE_LOOK);
                desc.iAnimFrame = pBoss->Get_CurAnimationFrame();

                pBoss->Get_GameInstance()->Add_GameObject(
                    LEVEL_HONG,
                    TEXT("Prototype_GameObject_HellBoss_AfterImage"),
                    LEVEL_HONG,
                    TEXT("Layer_HellBoss_AfterImage"),
                    &desc);
            }
        }
        else
        {
            pBoss->Change_State(new CHellBoss_IdleState());
        }
    }



    //void Update(CHellBoss* pBoss, float fDeltaTime) override
    //{
    //    const _float fDashSpeed = 10.f;

    //    m_fTime += fDeltaTime;
    //    if (m_fTime < 0.3f)
    //    {
    //        _float3 newPos = pBoss->Get_Pos() + m_vDashDir * fDashSpeed * 4.f * fDeltaTime;
    //        pBoss->Get_Transform()->Set_State(CTransform::STATE_POSITION, newPos);
    //        m_fAfterImageTimer += fDeltaTime;

    //        if (m_fAfterImageTimer >= m_fAfterImageInterval)
    //        {
    //            m_fAfterImageTimer = 0.f;

    //            CHellBoss_AfterImage::DESC desc{}; 
    //            desc.vPos = pBoss->Get_Pos();
    //            desc.vRight = pBoss->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
    //            desc.vUp = pBoss->Get_Transform()->Get_State(CTransform::STATE_UP);
    //            desc.vLook = pBoss->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    //            desc.iAnimFrame = pBoss->Get_CurAnimationFrame();

    //            if (FAILED(pBoss->Get_GameInstance()->Add_GameObject(
    //                LEVEL_HONG,
    //                TEXT("Prototype_GameObject_HellBoss_AfterImage"),
    //                LEVEL_HONG,
    //                TEXT("Layer_HellBoss_AfterImage"),
    //                &desc)))
    //            {
    //                MSG_BOX("AfterImage 생성 실패!");
    //            }
    //        }

    //    }
    //    else
    //    {
    //         // 대쉬하고 Idle
    //        pBoss->Change_State(new CHellBoss_IdleState());
    //    }
    //}

    void Exit(CHellBoss* pBoss) override {}

private:
    _float m_fTime = 0.f;
    _float3 m_vDashDir;
    _float m_fAfterImageTimer = 0.f;
    const _float m_fAfterImageInterval = 0.01f; // 0.05초마다 잔상
    _float m_fCurAngle = 0.f;
    _float m_fAngularSpeed = 2.f * D3DX_PI; // 1초에 한 바퀴 도는 속도
    _float3 m_vCircleCenter;
    _float m_fRadius = 10.f; // 플레이어와의 거리


};
