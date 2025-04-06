#pragma once
#include <Monster_Base.h>
#include <BehaviorTree.h>
BEGIN(Client)
class CCthulhu final : public CMonster_Base
{
public:
	enum class STATE
	{
		IDLE,
		ATTACK,
		MULTI_ATTACK,
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
    // 마지막에 플레이어 방향으로 바라보도록 함
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    virtual HRESULT Render()override;
 

public:
    virtual HRESULT On_Collision(CCollisionObject* other);
    void Billboarding(_float fTimeDelta);
    void Select_Pattern(_float fTimeDelta) override;

    virtual json Serialize() override;
    virtual void Deserialize(const json& j) override;

private:
    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();

    HRESULT Ready_Components();
    void Init_Textures();
	void Update_Animation(_float fTimeDelta);
	void Set_State(STATE eState) { m_eState = eState; }
	STATE Get_State() { return m_eState; }
    NodeStatus Attack();
    NodeStatus UpdateAttack();
    NodeStatus MultiMissileAttack();
	NodeStatus Update_Appear();

	void Create_BehaviorTree();
    _bool RayCubeIntersection(const _float3& rayOrigin,  _float3& rayDir, CCollider_Cube* pCollider,_float fMaxDistance);
    _bool IsPlayerVisible();

private:
    CBehaviorTree* m_pBehaviorTree = nullptr;
    unordered_map<STATE, vector<_uint>> m_mapStateTextures;
    _float m_fFrame = 0.f;
	_float m_fAnimationSpeed = 5.f;
	STATE  m_eState = STATE::IDLE;

    // 공격 관련
    _float m_fAttackCoolTime = 4.f;  
    _float m_fAttackCoolDown = 4.f;   
    _bool  m_bIsAttacking{ false };
    _int   m_iMissilesToFire{ 3 };
    _float m_fMissileTimer{ 0.f };

    _float m_fPhaseThreshold{ 1000*0.5f };  
    _bool  m_bIsMultiAttack{ false };     
    _float m_fMultiAttackCoolTime = 8.f;
    _float m_fMultiAttackCoolDown = 8.f;
    _int   m_iMultiMissilesToFire{ 7 };// 멀티 공격 시 발사할 미사일 수
    _float m_fMultiMissileTimer{ 0.f };  // 멀티 공격 타이머
	_bool  m_bUpdateAnimation{ true };

    // IDLE 관련
	_bool m_bIsAppeared{ false };
    _float m_fAppearTime{ 2.f };
	_float m_fAppearAmount{ 0.f };
    _float m_fDelta{ 0.f };
public:
    static CCthulhu* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();


};
END

