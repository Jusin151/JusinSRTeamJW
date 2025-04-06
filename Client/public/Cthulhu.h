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


    // 텍스처 추가 
    HRESULT Ready_Components();
    void Init_Textures();
	void Update_Animation(_float fTimeDelta);
	void Set_State(STATE eState) { m_eState = eState; }
	STATE Get_State() { return m_eState; }
    NodeStatus Attack();
	void Create_BehaviorTree();

private:
    CBehaviorTree* m_pBehaviorTree = nullptr;
    unordered_map<STATE, vector<_uint>> m_mapStateTextures;
    _float m_fFrame = 0.f;
	_float m_fAnimationSpeed = 5.f;
    _float m_fAttackCoolTime = 3.f;   // 누적된 공격 쿨타임 시간
    _float m_fAttackCoolDown = 3.f;   
	STATE m_eState = STATE::ATTACK;
public:
    static CCthulhu* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();


};
END

