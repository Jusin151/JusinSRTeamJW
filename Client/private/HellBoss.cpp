#include "HellBoss.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Transform.h"
#include "Player.h"
#include "GameInstance.h"
#include "HellBoss_IdleState.h"
#include "HellBoss_WalkState.h"
#include "HellBoss_MorphState.h"
#include "HellBoss_AttackState.h"
#include "HellBoss_DeadState.h"

CHellBoss::CHellBoss(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster_Base(pGraphic_Device) {
}

CHellBoss::CHellBoss(const CHellBoss& Prototype)
	: CMonster_Base(Prototype) {
}

HRESULT CHellBoss::Initialize_Prototype() { return S_OK; }

HRESULT CHellBoss::Initialize(void* pArg)
{
	if (FAILED(Ready_Components())) return E_FAIL;
	srand(static_cast<_uint>(time(nullptr)));
	m_eType = CG_MONSTER;
	m_iAp = 5;
	m_iHp = 3000;
	m_fSpeed = 3.f;

	m_pColliderCom->Set_Scale(_float3(7.0f, 10.f, 10.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-40.f, 0.f, -10.f));
	m_pTransformCom->Set_Scale(20.f, 30.f, 10.f);

	m_AnimationManager.AddAnimation("1_Idle", 0, 0);
	m_AnimationManager.AddAnimation("2_Walk",1,8,0.1f);
	m_AnimationManager.AddAnimation("3_EyeBlast", 10, 29);
	m_AnimationManager.AddAnimation("4_Shoot", 30, 55);
	m_AnimationManager.AddAnimation("5_Morph", 56, 86);


	m_AnimationManager.AddAnimation("6_Phase2_Idle", 86, 86);
	m_AnimationManager.AddAnimation("7_Phase2_Walk", 87, 94);
	m_AnimationManager.AddAnimation("8_Phase2_Charge", 95, 99);
	m_AnimationManager.AddAnimation("9_Phase2_Spin", 100, 103);
	m_AnimationManager.AddAnimation("0_Phase2_Shoot", 104, 107);
	m_AnimationManager.AddAnimation("T_Phase2_End", 108, 111);


	m_AnimationManager.AddAnimation("Y_ArmCut", 112, 117);
	m_AnimationManager.AddAnimation("U_ArmCut_Idle", 117, 117);
	m_AnimationManager.AddAnimation("I_ArmCut_Walk", 118, 124);
	m_AnimationManager.AddAnimation("O_ArmCut_Attack", 125, 138);
	m_AnimationManager.AddAnimation("P_ArmCut_End", 139, 203);
	m_AnimationManager.AddAnimation("G_Phase3_Idle", 203, 203);
	m_AnimationManager.AddAnimation("H_Phase3_Walk", 204, 212);
	m_AnimationManager.AddAnimation("J_Phase3_TripleEye", 213, 223);
	m_AnimationManager.AddAnimation("K_Phase3_Nova", 224, 234);
	m_AnimationManager.AddAnimation("L_Phase3_Spawn", 235, 246);
	m_AnimationManager.AddAnimation("B_Phase3_End", 247, 289);
	m_AnimationManager.AddAnimation("N_Phase4_Idle", 290, 311);
	m_AnimationManager.AddAnimation("M_Phase4_Death", 312, 337);

	m_AnimationManager.SetCurrentAnimation("1_Idle");



     //0     Idle //대기모션
     //1~8   Walk // 걷는모션
     //9~29  EyeBlast // 눈깔빔
     //30~55 Shoot // 발사
     //56~86 Morph // 진화
     //
     //86 //   두번째 모습/ IDLE 대기모션
     //87~94   두번째 모습 / 걷는모션
     //95~99   두번째 모습 / 발사준비
     //100~103 두번째 모습 / 스핀(예열)
     //104~107 두번째 모습 / 발사
     //108~111 두번째 모습 / 발사 끝
     //
     //112~117 두번째 모습 / 팔 절단
     //
     //117     두번째 모습 / 팔 절단 IDLE
     //118~124 두번째 모습 / 팔 절단 걷는모션
     //125~138 두번째 모습 / 팔 절단 공격모션
     //
     //139~203 // 진화
     //
     //203       // 세번째 모습 IDLE
     //204~212   // 세번째 모습 걷는보션
     //213~223   // 세번째 모습 삼눈깔빔
     //224~234   // 세번째 모습 노바(주위폭발)
     //234~246   // 세번째 모습 몬스터 소환 ( 또는 창작)
     //
     //247~289   // 진화
     //
     //290~311 // 네번째 모습 Idle 모션
     //312~337 // 네번째 모습 드디어 죽는 모션


	m_pCurState = new CHellBoss_IdleState();
	m_pCurState->Enter(this);


	return S_OK;
}


void CHellBoss::Priority_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
	{
		CGameObject* pTarget = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
		if (nullptr == pTarget) return;
		SetTarget(pTarget);
		Safe_AddRef(pTarget);
	}
	if (m_iHp <= 0) 
		m_eCurState = MS_DEATH;

	m_bIsActive = true;
}

void CHellBoss::Update(_float fTimeDelta)
{
	if (!m_pTarget)
		return;


	if (m_pCurState)
		m_pCurState->Update(this, fTimeDelta);

	Process_Input();
	m_AnimationManager.Update(fTimeDelta);

	if (m_eCurState != MS_DEATH)
	{
		m_pColliderCom->Update_Collider_Boss(TEXT("Com_Transform")); 

		m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
	}

	__super::Update(fTimeDelta);
}



void CHellBoss::Late_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;


	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
			return;

}
void CHellBoss::Change_State(CHellBoss_State* pNewState)
{
	if (m_pCurState)
	{
		m_pCurState->Exit(this);
		delete m_pCurState; 
	}

	// 상태 enum 
	if (dynamic_cast<CHellBoss_IdleState*>(pNewState))
		m_eCurState = MS_IDLE;
	else if (dynamic_cast<CHellBoss_WalkState*>(pNewState))
		m_eCurState = MS_WALK;
	else if (dynamic_cast<CHellBoss_AttackState*>(pNewState))
		m_eCurState = MS_ATTACK;
	else if (dynamic_cast<CHellBoss_DeadState*>(pNewState))
		m_eCurState = MS_DEATH;
	else if (dynamic_cast<CHellBoss_MorphState*>(pNewState))
		m_eCurState = MS_MORPH;

	m_pCurState = pNewState;

	if (m_pCurState)
		m_pCurState->Enter(this);
}




void CHellBoss::Process_Input()
{
	if (GetAsyncKeyState('1') & 0x8000) m_AnimationManager.SetCurrentAnimation("1_Idle");
	else if (GetAsyncKeyState('2') & 0x8000) m_AnimationManager.SetCurrentAnimation("2_Walk");
	else if (GetAsyncKeyState('3') & 0x8000) m_AnimationManager.SetCurrentAnimation("3_EyeBlast");
	else if (GetAsyncKeyState('4') & 0x8000) m_AnimationManager.SetCurrentAnimation("4_Shoot");
	else if (GetAsyncKeyState('5') & 0x8000) m_AnimationManager.SetCurrentAnimation("5_Morph");
	else if (GetAsyncKeyState('6') & 0x8000) m_AnimationManager.SetCurrentAnimation("6_Phase2_Idle");
	else if (GetAsyncKeyState('7') & 0x8000) m_AnimationManager.SetCurrentAnimation("7_Phase2_Walk");
	else if (GetAsyncKeyState('8') & 0x8000) m_AnimationManager.SetCurrentAnimation("8_Phase2_Charge");
	else if (GetAsyncKeyState('9') & 0x8000) m_AnimationManager.SetCurrentAnimation("9_Phase2_Spin");
	else if (GetAsyncKeyState('0') & 0x8000) m_AnimationManager.SetCurrentAnimation("0_Phase2_Shoot");
	else if (GetAsyncKeyState('T') & 0x8000) m_AnimationManager.SetCurrentAnimation("T_Phase2_End");
	else if (GetAsyncKeyState('Y') & 0x8000) m_AnimationManager.SetCurrentAnimation("Y_ArmCut");
	else if (GetAsyncKeyState('U') & 0x8000) m_AnimationManager.SetCurrentAnimation("U_ArmCut_Idle");
	else if (GetAsyncKeyState('I') & 0x8000) m_AnimationManager.SetCurrentAnimation("I_ArmCut_Walk");
	else if (GetAsyncKeyState('O') & 0x8000) m_AnimationManager.SetCurrentAnimation("O_ArmCut_Attack");
	else if (GetAsyncKeyState('P') & 0x8000) m_AnimationManager.SetCurrentAnimation("P_ArmCut_End");
	else if (GetAsyncKeyState('G') & 0x8000) m_AnimationManager.SetCurrentAnimation("G_Phase3_Idle");
	else if (GetAsyncKeyState('H') & 0x8000) m_AnimationManager.SetCurrentAnimation("H_Phase3_Walk");
	else if (GetAsyncKeyState('J') & 0x8000) m_AnimationManager.SetCurrentAnimation("J_Phase3_TripleEye");
	else if (GetAsyncKeyState('K') & 0x8000) m_AnimationManager.SetCurrentAnimation("K_Phase3_Nova");
	else if (GetAsyncKeyState('B') & 0x8000) m_AnimationManager.SetCurrentAnimation("B_Phase3_End");
	else if (GetAsyncKeyState('L') & 0x8000) m_AnimationManager.SetCurrentAnimation("N_Phase4_Idle");
	else if (GetAsyncKeyState('M') & 0x8000) m_AnimationManager.SetCurrentAnimation("M_Phase4_Death");
}
HRESULT CHellBoss::Render()
{
	int iCurFrame = m_AnimationManager.GetCurrentFrame();
	if (FAILED(m_pTextureCom->Bind_Resource(iCurFrame)))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_Resource())) 
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers())) 
		return E_FAIL;

	SetUp_RenderState();
	if (FAILED(m_pVIBufferCom->Render())) 
		return E_FAIL;

	//if (g_bDebugCollider) 
		
		m_pColliderCom->Render();

	Release_RenderState();

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("보스 체력 :") + to_wstring(m_iHp),
		_float2(-100.f, 300.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	return S_OK;
}
void CHellBoss::Set_AttackPattern(CPattern_Attack_Base* pPattern)
{
	if (m_pCurAttackPattern)
	{
		delete m_pCurAttackPattern;
		m_pCurAttackPattern = nullptr;
	}

	m_pCurAttackPattern = pPattern;
}

void CHellBoss::Use_Attack(_float fDeltaTime)
{
	if (m_pCurAttackPattern)
	{
		m_pCurAttackPattern->Execute(this, fDeltaTime);

		if (m_pCurAttackPattern->Is_Finished())
		{
			delete m_pCurAttackPattern;
			m_pCurAttackPattern = nullptr;

			Change_State(new CHellBoss_IdleState());
		}
	}
}





HRESULT CHellBoss::On_Collision(CCollisionObject* other)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;
	if (nullptr == other)
		return S_OK;
	
	// 안바뀌면 충돌 안일어남
	if (other->Get_Type() == CG_END)
		return S_OK;

	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vMtv = m_pColliderCom->Get_MTV();
	_float3 vMove = { vMtv.x, 0.f, vMtv.z };

	switch (other->Get_Type())
	{
	case CG_PLAYER:

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
		//m_pTransformCom->Go_Backward(fTimeDelta);
	
		if (m_eCurState != MS_ATTACK)
		{
			Take_Damage(other);
			
			
		}
		else
		{
			m_iAp *= 3;
			Take_Damage(other);
			m_iAp /= 3;
		}
		
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vCurPos);
		break;

	case CG_WEAPON:

		
		m_eCurState = MS_HIT;

		break;

	case CG_MONSTER:
		m_vNextPos += vMove * 0.2f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNextPos);

		break;
	case CG_STRUCTURE_WALL:
		m_vNextPos += vMove;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNextPos);
		break;
	case CG_DOOR:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vCurPos);

		break;
	default:
		break;
	}

	

	return S_OK;
}

void CHellBoss::Select_Pattern(_float fTimeDelta)
{
	if (m_eCurState == MS_DEATH)
		return;

	_float3 vDist;
	vDist = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION);


	switch (m_eCurState) 
	{
	case 1:
	   
		break;
	default:
		break;
	}
}

HRESULT CHellBoss::SetUp_RenderState()
{
	

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

	return S_OK;
}

HRESULT CHellBoss::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CHellBoss::Ready_Components()
{




	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_HONG, TEXT("Prototype_Component_Texture_HellBoss"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;


	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 0.1f,0.5f,0.5f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	_float3 fPos = { 10.f, 0.5f, 10.f };

	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	Collider_AttackDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackCollider), &Collider_AttackDesc)))
		return E_FAIL;

	return S_OK;
}

CHellBoss* CHellBoss::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHellBoss* pInstance = new CHellBoss(pGraphic_Device);

 	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHellBoss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHellBoss::Clone(void* pArg)
{
	CHellBoss* pInstance = new CHellBoss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CHellBoss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBoss::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pAttackCollider);
	Safe_Release(m_pTarget);

	if (m_pCurState)
	{
		m_pCurState->Exit(this);
		delete m_pCurState;
		m_pCurState = nullptr;
	}

	if (m_pCurAttackPattern)
	{
		delete m_pCurAttackPattern;
		m_pCurAttackPattern = nullptr;
	}
}


