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
#include "Pattern_Morph.h"
#include "Pattern_Warp.h"
#include "HellBoss_FloatState.h"
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
	m_iHp = 25000;
	m_iPrevHpDiv100 = m_iHp / 100;
	m_fSpeed = 7.f;
	m_fOffset = 3.6f;
	m_pColliderCom->Set_Scale(_float3(7.0f, 10.f, 10.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-40.f, m_fOffset, -10.f));
	m_pTransformCom->Set_Scale(20.f, 30.f, 10.f);

	m_AnimationManager.AddAnimation("1_Idle", 0, 0);
	m_AnimationManager.AddAnimation("2_Walk",1,8,0.1f);
	m_AnimationManager.AddAnimation("3_EyeBlast", 10, 29);
	m_AnimationManager.AddAnimation("4_Shoot", 30, 55);




	m_AnimationManager.AddAnimation("5_Morph", 56, 86);  //////////////////////////////2페이즈 진입


	m_AnimationManager.AddAnimation("6_Phase2_Idle", 86, 86);
	m_AnimationManager.AddAnimation("7_Phase2_Walk", 87, 94,0.1f);
	m_AnimationManager.AddAnimation("8_Phase2_Charge", 95, 99); //사실상 안씀
	m_AnimationManager.AddAnimation("9_Phase2_Spin", 100, 103);//사실상 안씀
	m_AnimationManager.AddAnimation("8_Phase2_Charge", 99, 103,0.04f); // 2페이즈_차징
	m_AnimationManager.AddAnimation("0_Phase2_Shoot", 104, 107,0.04f); // 2페이즈_발사 	//104부터 쏨


	m_AnimationManager.AddAnimation("T_Phase2_End", 108, 111,0.15f); // 워프 모션


	m_AnimationManager.AddAnimation("Y_ArmCut", 112, 117);        //////////////////////////////3페이즈 진입, 한팔 잘리는 모션



	m_AnimationManager.AddAnimation("U_ArmCut_Idle", 117, 117);  // 한팔 대기상태
	m_AnimationManager.AddAnimation("I_ArmCut_Walk", 118, 124, 0.1f);  // 한팔 Walk상태
	m_AnimationManager.AddAnimation("O_ArmCut_Attack", 125, 138);// 한팔 Attack상태


	m_AnimationManager.AddAnimation("P_ArmCut_End", 139, 203,0.08f);   //////////////////////////// 4페이즈 진입


	m_AnimationManager.AddAnimation("G_Phase3_Idle", 203, 203);
	m_AnimationManager.AddAnimation("H_Phase3_Walk", 204, 212, 0.04f);
	m_AnimationManager.AddAnimation("J_Phase3_TripleEye", 213, 223,0.07f);  // 삼눈깔빔
	m_AnimationManager.AddAnimation("K_Phase3_Nova", 224, 234);
	m_AnimationManager.AddAnimation("L_Phase3_Spawn", 235, 246);

	m_AnimationManager.AddAnimation("B_Phase3_End", 247, 289,0.1f); //////////////////////////// 5페이즈 진입


	m_AnimationManager.AddAnimation("N_Phase4_Idle", 290, 311);
	m_AnimationManager.AddAnimation("M_Phase4_Death", 312, 337);

	m_AnimationManager.AddAnimation("Start", 0, 337,0.02f);

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


	// 오브젝트풀링
 	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_HONG,
		TEXT("Prototype_GameObject_HellBoss_Bullet"), TEXT("Layer_HellBoss_PHASE1_HandBullet"), 100))) 
		return E_FAIL;

	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_HONG,
		TEXT("Prototype_GameObject_HellBoss_Bullet"), TEXT("Layer_HellBoss_PHASE1_EyeBullet"), 100)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_HONG,
		TEXT("Prototype_GameObject_HellBoss_Bullet"), TEXT("Layer_HellBoss_PHASE2_HandBullet"), 100)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_HONG,
		TEXT("Prototype_GameObject_HellBoss_Bullet"), TEXT("Layer_HellBoss_PowerBlast_Bullet"), 100))) // 주변에서 회전하는녀석
		return E_FAIL;


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

void CHellBoss::Process_Input()
{
	//if (GetAsyncKeyState('0') & 0x8000)		
	//	m_AnimationManager.SetCurrentAnimation("Start");

	if (GetAsyncKeyState('Z') & 0x8000)
	{
		if (!m_bZKeyPressed)
		{
			m_bZKeyPressed = true;

			m_bDarkHole_EffectActive = !m_bDarkHole_EffectActive;

			if (m_bDarkHole_EffectActive)
			{
				Generate_Warp_Positions();
				m_fDarkHole_SpawnTimer = 0.f;
			}
		}
	}
	else
	{
		m_bZKeyPressed = false; // 키를 뗐을 때 false로 초기화
	}


	if (GetAsyncKeyState('C') & 0x8000 && !m_bJumping && !m_bFalling)
	{
		m_bJumping = true;
		m_fJumpTime = 0.f;
		m_vJumpStartPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float3 vPos = m_vJumpStartPos;
		vPos.y += 80.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		BossDESC desc{};
		desc.vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		desc.vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
		desc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		desc.vPos = m_vJumpStartPos;
		desc.strState = "Up";

		m_pGameInstance->Add_GameObject(LEVEL_HONG,
			TEXT("Prototype_GameObject_HellBoss_Skill_Landing"),
			LEVEL_HONG, TEXT("Layer_HellBoss_Skill_Landing"), &desc);

		if (m_pTarget)
		{
			_float3 vPlayerPos = static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION);
			_float3 vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			_float angleRad = ((rand() % 360)) * D3DX_PI / 180.f;

			_float distance = 40.f;

			_float3 vOffset;
			vOffset.x = cosf(angleRad) * distance;
			vOffset.z = sinf(angleRad) * distance;
			vOffset.y = 0.f;

			_float3 vTargetPos = vPlayerPos + vOffset;
			m_vTargetDir = (vTargetPos - vCurrentPos);
			m_vTargetDir.Normalize();
			m_bFalling = true;
			m_bJumping = false;
		}



	}

}
void CHellBoss::Update(_float fTimeDelta)
{
	if (!m_pTarget)
		return;
	if (m_pCurState)
		m_pCurState->Update(this, fTimeDelta);

	Process_Input();
	if (m_bDarkHole_EffectActive)
		Spawn_Warp_Effect(fTimeDelta);


	if (m_bFalling)
	{
		_float3 vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		vCurPos += m_vTargetDir * m_fFallSpeed * fTimeDelta;
		vCurPos.y -= m_fFallSpeed * fTimeDelta;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurPos);
		if (vCurPos.y <= m_fOffset + 0.5f)
		{
			m_bFalling = false;
			_float3 landedPos = vCurPos;
			landedPos.y = m_fOffset;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, landedPos);

			// Down 애니메이션 생성
			BossDESC desc{};
			desc.vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			desc.vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
			desc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			desc.vPos = landedPos;
			desc.strState = "Down";

			m_pGameInstance->Add_GameObject(LEVEL_HONG,
				TEXT("Prototype_GameObject_HellBoss_Skill_Landing"),
				LEVEL_HONG, TEXT("Layer_HellBoss_Skill_Landing"), &desc);
		}
	}







	m_AnimationManager.Update(fTimeDelta);

	if (m_eCurState != MS_DEATH)
	{
		m_pColliderCom->Update_Collider_Boss(TEXT("Com_Transform")); 

		m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
	}

	if (m_iHp <= 20000 && !m_bDidPhase2Morph) // <<< 2페이즈 돌입!
	{
		m_bDidPhase2Morph = true;
		m_ePhase = PHASE2;
		Set_AttackPattern(new CPattern_Morph());
		Change_State(new CHellBoss_MorphState());
		return;
	}
	if (m_iHp <= 15000 && !m_bDidPhase3Morph)  // <<< 3페이즈 돌입! , 한팔 절단
	{
		m_bDidPhase3Morph = true;
		m_ePhase = PHASE3;
		Set_AttackPattern(new CPattern_Morph());
		Change_State(new CHellBoss_MorphState());
		return;
	}
	if (m_iHp <= 10000 && !m_bDidPhase4Morph) // 4페이즈 돌입!
	{
		m_bDidPhase4Morph = true;
		m_ePhase = PHASE4;
		Set_AttackPattern(new CPattern_Morph());
		Change_State(new CHellBoss_FloatState());
		return;
	}

	if (m_iHp <= 5000 && !m_bDidPhase5Morph)  // <<< 5페이즈 돌입! 
	{
		m_bDidPhase5Morph = true;
		m_ePhase = PHASE5;
		Set_AttackPattern(new CPattern_Morph());  
		Change_State(new CHellBoss_MorphState()); 
		return;
	}



	int a{};
	switch (m_ePhase) // 피 깎이는 로직의 대해서만 여기서 패턴상태로 관리
	{
	case PHASE1: 
		a = 5;
		break;
	case PHASE2:
		break;
	case PHASE3:
		a = 5;
		break;
	case PHASE4:
		a = 5;
		break;
	case PHASE5:
		a = 5;
		break;
	default:
		break;
	}
	Power_Blast_Patter(); // 피가 100 달때마다 생기는 파워블라스트같은 경우에는 공용

	__super::Update(fTimeDelta);
}

void CHellBoss::Launch_PowerBlast_Bullets()
{
	for (auto& pBullet : m_vecPowerBlasts)
	{
		if (pBullet && pBullet->Get_BulletType() == L"Power_Blast")
			pBullet->Launch_Toward_Player(); // 한번에 발사!
	}



	m_vecPowerBlasts.clear();
	m_iPowerBlastCount = 0;
}

void CHellBoss::Power_Blast_Patter()
{
	_int iCurHpDiv100 = m_iHp / 100;
	if (iCurHpDiv100 < m_iPrevHpDiv100)
	{
		m_iPrevHpDiv100 = iCurHpDiv100;


		static _float3 axisList[15] = {
			_float3(1.f, 0.f, 0.f),     // X축
			_float3(0.f, 1.f, 0.f),     // Y축
			_float3(0.f, 0.f, 1.f),     // Z축
			_float3(1.f, 1.f, 0.f),     // XY축
			_float3(1.f, 0.f, 1.f),     // XZ축
			_float3(0.f, 1.f, 1.f),     // YZ축
			_float3(-1.f, 1.f, 0.f),    // 대각선
			_float3(0.f, -1.f, 1.f),
			_float3(1.f, 1.f, 1.f),     // 삼축 회전
			_float3(-1.f, -1.f, -1.f),  // 반대 회전
			_float3(1.f, 0.f, 0.f),     // X축
			_float3(0.f, 1.f, 0.f),     // Y축
			_float3(0.f, 0.f, 1.f),     // Z축
			_float3(1.f, 1.f, 0.f),     // XY축
			_float3(1.f, 0.f, 1.f),     // XZ축
		};

		// 총알 1개 생성
		CHellBoss_Bullet::PowerBlastDesc pDesc{};
		pDesc.wBulletType = L"Power_Blast";
		pDesc.iIndex = m_iPowerBlastCount;       // 현재 몇 번째 총알인지
		pDesc.iTotalCount = 15;                  // 총 10개
		pDesc.bIsPowerBlast = true;              // 회전 총알 여부
		pDesc.vAxis = axisList[m_iPowerBlastCount % 10]; // 축 설정

		if (!m_pGameInstance->Add_GameObject_FromPool(LEVEL_HONG, LEVEL_HONG, TEXT("Layer_HellBoss_PowerBlast_Bullet"), &pDesc))
		{
			MSG_BOX("HellBoss_Bullet 생성 실패");
		}

		CGameObject* pLastObj = m_pGameInstance->Find_Last_Object(LEVEL_HONG, TEXT("Layer_HellBoss_PowerBlast_Bullet"));
		CHellBoss_Bullet* pBullet = dynamic_cast<CHellBoss_Bullet*>(pLastObj);
		if (pBullet)
			m_vecPowerBlasts.push_back(pBullet);

		m_iPowerBlastCount++;

		if (m_iPowerBlastCount == 15)
		{
			Launch_PowerBlast_Bullets();  // 나선 발사
			m_iPowerBlastCount = 0;
		}
	}
}
void CHellBoss::Generate_Warp_Positions()
{
	m_vecWarpGrid.clear();

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float3 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_float3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);

	vLook.Normalize();
	vRight.Normalize();
	vUp.Normalize();

    // 멤버변수로 빼도 되기는 하는데 , 여기서 설정하는게 더 편하지 않을까??
	_float distanceFromBoss = 10.f;
	_float width = 20.f;
	_float height = 10.f;
	_int cols = 5;
	_int rows = 3;

	_float3 planeCenter = vPos + vLook * distanceFromBoss;

	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			if (row == rows / 2 && col == cols / 2)
				continue; // 중앙 칸 제외

			_float xOffset = (col - (cols - 1) / 2.0f) * (width / cols);
			_float yOffset = (row - (rows - 1) / 2.0f) * (height / rows);

			_float3 finalPos = planeCenter + vRight * xOffset + vUp * yOffset;
			m_vecWarpGrid.push_back(finalPos);
		}
	}

	random_shuffle(m_vecWarpGrid.begin(), m_vecWarpGrid.end()); // 랜덤화
}
void CHellBoss::Spawn_Warp_Effect(_float fDeltaTime)
{
	m_fDarkHole_SpawnTimer += fDeltaTime;

	if (m_fDarkHole_SpawnTimer >= 1.f)
	{
		m_fDarkHole_SpawnTimer = 0.f;

		//  매번 방향 새로 가져오기
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_float3 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_float3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);

		vLook.Normalize();
		vRight.Normalize();
		vUp.Normalize();

		_float distanceFromBoss = 10.f;
		_float width = 20.f;
		_float height = 10.f;
		_int cols = 5;
		_int rows = 3;

		_float3 planeCenter = vPos + vLook * distanceFromBoss;

		//  제외하고 랜덤 위치 생성
		vector<_float3> newGrid;

		for (int row = 0; row < rows; ++row)
		{
			for (int col = 0; col < cols; ++col)
			{
				if (row == rows / 2 && col == cols / 2)
					continue;

				_float xOffset = (col - (cols - 1) / 2.0f) * (width / cols);
				_float yOffset = (row - (rows - 1) / 2.0f) * (height / rows); // 세로 위치 조절하는곳

				//_float3 finalPos = planeCenter + vRight * xOffset + vUp * yOffset;
				_float3 finalPos = planeCenter + vRight * xOffset + vUp * (yOffset + 5.0f); //   (yOffset + 1.0f); 여기에서 숫자 조절하면 위로 감

				newGrid.push_back(finalPos);
			}
		}

		random_shuffle(newGrid.begin(), newGrid.end());

		// 하나 꺼내서  이펙트 생성
		if (!newGrid.empty())
		{
			_float3 warpPos = newGrid.back();

			BossDESC desc{};
			desc.vRight = vRight;
			desc.vUp = vUp;
			desc.vLook = vLook;
			desc.vPos = warpPos;

			m_pGameInstance->Add_GameObject(LEVEL_HONG,
				TEXT("Prototype_GameObject_HellBoss_Skill_DarkHole"),
				LEVEL_HONG, TEXT("Layer_HellBoss_Skill_DarkHole"), &desc);
		}
	}
}



_float3 CHellBoss::Get_RandomWarpPos_InFront()
{
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float3 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_float3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);

	vLook.Normalize();
	vRight.Normalize();
	vUp.Normalize();

	_float distanceFromBoss = 10.f;
	_float width = 20.f;    // X축 방향  길이
	_float height = 10.f;   // Y축 방향  길이
	_int cols = 5;
	_int rows = 3;


	_float3 planeCenter = vPos + vLook * distanceFromBoss;

	_int colIndex = rand() % cols;
	_int rowIndex = rand() % rows;


	_float xOffset = (colIndex - (cols - 1) / 2.0f) * (width / cols);
	_float yOffset = (rowIndex - (rows - 1) / 2.0f) * (height / rows);

	_float3 finalPos = planeCenter + vRight * xOffset + vUp * yOffset;

	return finalPos;
}
void CHellBoss::Set_AttackPattern(CPattern_Base* pPattern)
{
	if (m_pCurAttackPattern)
	{
		delete m_pCurAttackPattern;
		m_pCurAttackPattern = nullptr;
	}

	m_pCurAttackPattern = pPattern;
}

void CHellBoss::Late_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;


	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
			return;

	if (!m_bJumping && !m_bFalling)
	{
		//_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		//fPos.y = m_fOffset;
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
	}


	if (m_fParryTextTimer > 0.f)
		m_fParryTextTimer -= fTimeDelta;


}
void CHellBoss::Use_Attack(_float fDeltaTime)
{
	if (m_pCurAttackPattern)
	{
		m_pCurAttackPattern->Execute(this, fDeltaTime);
		if (m_pCurAttackPattern->Is_Finished())
		{
			// Morph 패턴인 경우 혹은 1페이즈라면 
			if (dynamic_cast<CPattern_Morph*>(m_pCurAttackPattern) != nullptr || m_ePhase == PHASE1)
			{
				delete m_pCurAttackPattern;
				m_pCurAttackPattern = nullptr;
				Change_State(new CHellBoss_IdleState());
			}
		}
	}
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

HRESULT CHellBoss::Render()
{
	_int iCurFrame = m_AnimationManager.GetCurrentFrame();
	if (FAILED(m_pTextureCom->Bind_Resource(iCurFrame)))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_Resource())) 
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers())) 
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, iCurFrame)))
		return E_FAIL;

	SetUp_RenderState();
	m_pShaderCom->Begin(1);
	if (FAILED(m_pVIBufferCom->Render())) 
		return E_FAIL;

	//if (g_bDebugCollider) 
		
		m_pColliderCom->Render();
	m_pShaderCom->End();
	Release_RenderState();

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("보스 체력 :") + to_wstring(m_iHp),
		_float2(-100.f, 300.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("보스 위치 X:") + to_wstring(m_pTransformCom->Get_WorldMat()._41),
	_float2(-300.f, -207.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("보스 위치 Y:") + to_wstring(m_pTransformCom->Get_WorldMat()._42),
	_float2(-100.f, -207.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("보스 위치 Z:") + to_wstring(m_pTransformCom->Get_WorldMat()._43),
	_float2(100.f, -207.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

if (m_fParryTextTimer > 0.f)
{
	if (m_bParrySuccess)
	{
		m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("패링 성공"),
			_float2(-100.f, -100.f), _float2(8.f, 0.f), _float3(0.f, 1.f, 0.f));
	}
	else
	{
		m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("패링 실패"),
			_float2(-100.f, -100.f), _float2(8.f, 0.f), _float3(1.f, 0.f, 0.f));
	}
}




	return S_OK;
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
	D3DXVECTOR2 vScaleFactor(1.f, 1.f);
	D3DXVECTOR2 vOffsetFactor(0.0f, 0.0f); // Y축 반전을 위한 오프셋 조정
	m_pShaderCom->Set_UVScaleFactor(&vScaleFactor);
	m_pShaderCom->Set_UVOffsetFactor(&vOffsetFactor);

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


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
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
	m_AnimationManager.ClearAnimations(); 
}


