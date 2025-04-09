#pragma once

#include "Monster_Base.h"
#include "AnimationManager.h" 
#include "HellBoss_State.h"
#include "Pattern_Base.h"
#include "Player.h"
#include "HellBoss_Bullet.h"

enum PHASE_STATE { PHASE1, PHASE2, PHASE3, PHASE4, PHASE5};

typedef struct BossINFO
{
	_float3 vRight{};
	_float3 vUp{};
	_float3 vLook{};
	_float3 vPos{};

}BossDESC;

class CHellBoss : public CMonster_Base
{
public:
	PHASE_STATE m_ePhase = PHASE1; // 초기엔 1페이즈 


private:
	CHellBoss(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHellBoss(const CHellBoss& Prototype);
	virtual ~CHellBoss() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	void Process_Input();
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

public:
	virtual HRESULT On_Collision(CCollisionObject* other) override;
private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();
public:
	static CHellBoss* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();
public:
	_float3 Get_PlayerPos() const { return static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION); }
	_float3 Get_Pos() const { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	bool HasTarget() const { return m_pTarget != nullptr; }
	void Change_State(CHellBoss_State* pNewState);
public: // 패턴관련
	void Set_AttackPattern(CPattern_Base* pPattern);
	void Use_Attack(_float fDeltaTime);
	void Power_Blast_Patter();
	void Launch_PowerBlast_Bullets();
	virtual void Select_Pattern(_float fTimeDelta) override;
	CPattern_Base* Get_AttackPattern() const { return m_pCurAttackPattern; }
private:
	CPattern_Base* m_pCurAttackPattern = { nullptr };

public://애니메이션관련
	void Set_Animation(const string& strAnimKey) { m_AnimationManager.SetCurrentAnimation(strAnimKey); }
	bool Get_AnimationFinished() const { return m_AnimationManager.IsFinished(); }
	_uint Get_CurAnimationFrame() { return m_AnimationManager.GetCurrentFrame(); }
	LPDIRECT3DDEVICE9 Get_Graphic_Device() { return m_pGraphic_Device; }
	CGameInstance* Get_GameInstance() { return m_pGameInstance; }
	CAnimationManager* Get_AnimationManager() { return &m_AnimationManager; }
	CAnimationManager m_AnimationManager = {};
public:
	PHASE_STATE Get_Phase() const { return m_ePhase; }
private: //콜라이더
	CCollider_Cube* m_pAttackCollider = { nullptr };
private: // 텍스쳐 관련
	map<string, pair<int, int>> m_TextureRanges{};
private:
	CHellBoss_State* m_pCurState = { nullptr };
	CHellBoss_State* m_pNextState = { nullptr };
public:
	CTransform* Get_Transform() const { return m_pTransformCom; }
	_float Get_Speed() const { return m_fSpeed; }
	void Set_CurPos(const _float3& vPos) { m_vCurPos = vPos; }
	_bool m_bInitializedCurPos = { false };
	list<CGameObject*> bullets = {};
	_int m_iPrevHpDiv100 = {};
	_int m_iPowerBlastCount = {};
	list<CHellBoss_Bullet*> m_vecPowerBlasts;
	_bool m_bDidPhase2Morph = { false };  //2번째 페이즈 진입 , 통통이에서 날씬이로
	_bool m_bDidPhase3Morph = { false }; // 3번째 페이즈 진입, 날씬이에서 한팔잘린상태
	_bool m_bDidPhase4Morph = { false }; // 4번째 페이즈 진입, 한팔잘린상태에서 뚱뚱이로
	_bool m_bDidPhase5Morph = { false }; // 5번째 페이즈 진입, 뚱뚱이에서 -> 괴물처럼
	_float m_fOffset = {}; // 지면고정

	BossDESC m_BossInfo{};
};

//
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
//204~212   // 세번째 모습 탐색
//213~223   // 세번째 모습 삼눈깔빔
//224~234   // 세번째 모습 노바(주위폭발)
//234~246   // 세번째 모습 몬스터 소환 ( 또는 창작)
//
//247~289   // 진화
//
//290~311 // 네번째 모습 Idle 모션
//312~337 // 네번째 모습 드디어 죽는 모션

//IdleState
//└ 플레이어 근처에 있음 ? -> 랜덤 패턴 선택 -> Set_AttackPattern()
//└ AttackState로 전환
//
//AttackState
//└Use_Attack() 실행
//└패턴이 끝나면 IdleState로 돌아감



