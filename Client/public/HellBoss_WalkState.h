#pragma once

#include "HellBoss_State.h"
#include "Pattern_Shoot.h"
#include "HellBoss_AttackState.h"
#include "HellBoss_IdleState.h"

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
    void Enter(CHellBoss* pBoss) override {
        m_fTime = 0.f;
        _float3 vDir = pBoss->Get_Pos() - pBoss->Get_PlayerPos(); // 플레이어 반대 방향
        vDir.y = 0.f;
        m_vDashDir = vDir.GetNormalized();
        pBoss->Set_Animation("I_ArmCut_Dash"); // 빠르게 걷는 모션
    }

    void Update(CHellBoss* pBoss, float fDeltaTime) override {
        m_fTime += fDeltaTime;
        if (m_fTime < 0.5f)
        {
            _float3 newPos = pBoss->Get_Pos() + m_vDashDir * pBoss->Get_Speed() * 4.f * fDeltaTime;
            pBoss->Get_Transform()->Set_State(CTransform::STATE_POSITION, newPos);
        }
        else
        {
            // 도망쳤으면 다시 플레이어 쫓아가도록 WalkState로
            pBoss->Change_State(new CHellBoss_WalkState());
        }
    }

    void Exit(CHellBoss* pBoss) override {}

private:
    _float m_fTime = 0.f;
    _float3 m_vDashDir;
};
