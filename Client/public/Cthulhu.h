#pragma once
#include <Monster_Base.h>
#include <BehaviorTree.h>
#include "Spike.h"
BEGIN(Client)

enum class MonsterType
{
    THINGY,
    LOOKER,
    TYPE_END
};

class CCthulhu final : public CMonster_Base
{
    friend  class CCthulhuAttackMag;
public:
	enum class STATE
	{
		IDLE,
		ATTACK,
		MULTI_ATTACK,
		DPELOY_TENTACLES,
		DEAD
	};
private:
    CCthulhu(LPDIRECT3DDEVICE9 pGraphic_Device);
    CCthulhu(const CCthulhu& Prototype);
    virtual ~CCthulhu() = default;

public:
    virtual HRESULT Initialize_Prototype()override;
    virtual HRESULT Initialize(void* pArg)override;
    virtual void Priority_Update(_float fTimeDelta)override;
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    virtual HRESULT Render()override;
 

public:
    virtual HRESULT On_Collision(CCollisionObject* other);
    void Billboarding(_float fTimeDelta);
    void Select_Pattern(_float fTimeDelta) override;
    _bool Get_CanHit() const { return m_bCanHit; }

    virtual void Set_Hp(_int iHp) override;

    virtual json Serialize() override;
    virtual void Deserialize(const json& j) override;
    static _bool RayCubeIntersection(const _float3& rayOrigin, _float3& rayDir, CCollider_Cube* pCollider, _float fMaxDistance);
private:
    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();

    HRESULT Ready_Components();
    void Init_Textures();
	void Update_Animation(_float fTimeDelta);
	void Set_State(STATE eState) { m_eState = eState; }
	STATE Get_State() { return m_eState; }


    // 비헤이비어 트리 관련 함수
	void Create_BehaviorTree();
	NodeStatus Update_Appear(); // 처음 등장
    NodeStatus Attack(); // 공격 트리거
    NodeStatus UpdateAttack(); // 공격 진행
    NodeStatus MultiMissileAttack(); // 두번째 패턴

    NodeStatus Deploy_Tentacles(); // 공격용 촉수 설치
    NodeStatus Deploy_BigTentacles(); // 데코

    NodeStatus Attack_Spike(); // 스파이크 공격

    NodeStatus Spawn_Monster(); // 스파이크 공격

    NodeStatus Dead();


    _bool IsPlayerVisible();
    void DropItems();
    void Remove_DeadTentacles();
    void Remove_DeadMonsters();
    void Create_Monster(MonsterType type, const _float3& spawnPos);
    void Update_CoolTimes(_float fTimeDelta);
    
private:
    CBehaviorTree* m_pBehaviorTree = nullptr;
    unordered_map<STATE, vector<_uint>> m_mapStateTextures;
    _float m_fFrame = 0.f;
	_float m_fAnimationSpeed = 5.f;
	STATE  m_eState = STATE::IDLE;
	_bool m_bIsColned{ false };
    _bool m_bCanHit = { false };

    // 공격 관련
    _float m_fAttackCoolTime = 0.f;  
    _float m_fAttackCoolDown = 3.f;   
    _bool  m_bIsAttacking{ false };
    _int   m_iMissilesToFire{ 3 };
    _float m_fMissileTimer{ 0.f };

    _float m_fPhaseThreshold{ 1000*0.5f };  
    _bool  m_bIsMultiAttack{ false };     
    _float m_fMultiAttackCoolTime = 0.f;
    _float m_fMultiAttackCoolDown = 8.f;
    _int   m_iMultiMissilesToFire{ 7 };// 멀티 공격 시 발사할 미사일 수
    _float m_fMultiMissileTimer{ 0.f };  // 멀티 공격 타이머
	_bool  m_bUpdateAnimation{ true };

    // IDLE 관련
	_bool m_bIsAppeared{ false };
    _float m_fAppearTime{ 2.f };
	_float m_fAppearAmount{ 0.f };
    _float m_fDelta{ 0.f };
	_bool m_bCameraShaken{ false };

    // 촉수 설치
	_float m_fTentacleCoolTime{ 0.f };
	_float m_fTentacleCoolDown{ 2.f };
	_bool  m_bIsTentacleInstalled{ false };
	_float m_fTentacleTimer{ 0.f };
	list<class CCthulhu_Tentacle*> m_listTentacles;
    _int m_iCountTentacle{ 0 };
    _bool m_bRemovedTentacle{ false };

    _float m_fBigTentacleCoolTime{ 0.f };
    _float m_fBigTentacleCoolDown{ 5.f };
    _bool  m_bIsBigTentacleInstalled{ false };
    _float m_fBigTentacleTimer{ 5.f };
	vector<class CCthulhu_Big_Tentacle*> m_vecBigTentacles;

    // 몬스터 배치 관련
    _float m_fSpawnCoolTime{ 0.f };
    _float m_fSpawnCoolDown{ 2.f };
    list<CGameObject*> m_MonsterList;
    const _int MAX_MONSTER_SPAWN_COUNT = 6;

    // 스파이크
    _float m_fSpikeCoolTime{ 0.f };
    _float m_fSpikeCoolDown{ 2.f };
    _float m_fSpikeTimer{ 0.f };
    _float m_fSpikeSpawnTimer{ 0.3f };
    _float m_fPhaseThreshold2{ 0.f };
    _bool  m_bSpikeAppeared{ false };
    vector<CSpike*> m_vecSpikes;


    // 플레이어 관련
    CTransform* m_pPlayerTransform { nullptr };

    // 사운드 관련
    _float m_fSoundTime{ 0.f };

public:
    static CCthulhu* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();


};
END

