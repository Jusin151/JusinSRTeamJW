#include "Pattern_Morph.h"
#include "HellBoss_IdleState.h"
#include "HellBoss_CircleState.h"
#include "HellBoss_JumpLoopState.h"

CPattern_Morph::CPattern_Morph()
{
    m_bStarted = false;
    m_fAccTime = 0.f;
    m_fDuration = 1.5f; //  대기
}

void CPattern_Morph::Update(CHellBoss* pBoss, float fDeltaTime)
{
}

void CPattern_Morph::Execute(CHellBoss* pBoss, float fDeltaTime)
{
    if (!m_bStarted)
    {
        m_bStarted = true;
        m_fAccTime = 0.f;
  
        if (pBoss->Get_Phase() == PHASE2)
        {
            pBoss->Set_Animation("5_Morph"); //2번째 페이즈 진입 , 통통이에서 날씬이로
            m_fDuration = 1.5f; //  대기
        }
        else if (pBoss->Get_Phase() == PHASE3)
        {
            pBoss->Set_Animation("Y_ArmCut"); // 3번째 페이즈 진입, 날씬이에서 한팔잘린상태
            m_fDuration = 0.3f; //  대기
        }
        else if (pBoss->Get_Phase() == PHASE4)
        {
            pBoss->Set_Animation("P_ArmCut_End");// 4번째 페이즈 진입, 한팔잘린상태에서 뚱뚱이로
            m_fDuration = 4.3f; //  대기
        }
        else if (pBoss->Get_Phase() == PHASE5)
        {
            pBoss->Set_Animation("B_Phase3_End");// 5번째 페이즈 진입, 뚱뚱이에서 -> 괴물처럼
            m_fDuration = 7.3f; //  대기
        }

        // 사운드 재생 
        //  pBoss->Play_Sound("진화 재생.wav 같은느낌");

        //  이펙트 생성 
        // pBoss->Fire_Morph(); 같은거 
    }

    //  시간
    m_fAccTime += fDeltaTime;

    if (m_fAccTime >= m_fDuration)
    {
        m_bStarted = false;
        m_fAccTime = 0.f;

      
        /*if (pBoss->Get_Phase() == PHASE3 )
        {
            pBoss->Change_State(new CHellBoss_JumpLoopState());
            return;
        }*/

        if (pBoss->Get_Phase() == PHASE4)
        {
            pBoss->Change_State(new CHellBoss_CircleState());
            return;
        }

        pBoss->Change_State(new CHellBoss_IdleState());
    }

}
