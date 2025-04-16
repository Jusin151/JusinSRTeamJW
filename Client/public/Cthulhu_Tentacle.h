#pragma once
#include <Monster_Base.h>
BEGIN(Client)

class CCthulhu_Tentacle final : public CMonster_Base
{
public:
    enum class Tentacle_STATE
    {
        IDLE,
        APPEAR,
		DISAPPEAR,
        ATTACK,
        DEAD
    };

private:
    CCthulhu_Tentacle(LPDIRECT3DDEVICE9 pGraphic_Device);
    CCthulhu_Tentacle(const CCthulhu_Tentacle& Prototype);
    virtual ~CCthulhu_Tentacle() = default;

public:
    virtual HRESULT Initialize_Prototype()override;
    virtual HRESULT Initialize(void* pArg)override;
    virtual void Priority_Update(_float fTimeDelta)override;
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    virtual HRESULT Render()override;

	virtual void Reset() override;  
    virtual void Set_Hp(_int iHp);

public:
    virtual HRESULT On_Collision(CCollisionObject* other);
    void Billboarding(_float fTimeDelta);
    void Select_Pattern(_float fTimeDelta) override;
	auto Get_State() const { return m_eState; }
    void Set_State(Tentacle_STATE eState)
    {
        m_eState = eState;
        m_fFrame = 0.f;
        m_iCurrentFrame = m_mapStateTextures[m_eState][(_uint)m_fFrame];
    }

    void Set_Position(const _float3& vPos);
    _bool IsAnimationFinished() const;
    void Set_Parent(CGameObject* pParent) {m_pOwner = pParent; }
private:
    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();

    HRESULT Ready_Components();
    void Init_Textures();
    void Update_Animation(_float fTimeDelta);
    _bool IsPlayerVisible();
    void Attack();
private:
    unordered_map<Tentacle_STATE, vector<_uint>> m_mapStateTextures;
    _float m_fFrame = 0.f;
    _float m_fAnimationSpeed = 7.f;
    Tentacle_STATE  m_eState = Tentacle_STATE::IDLE;

    _uint m_iMaxHp{ 0u };
    _bool  m_bUpdateAnimation{ true };
	_bool m_bCanHit{ false };
	_bool m_bCanAttack{ false };
    _bool m_bAnimFinished { false };
    CGameObject* m_pOwner{ nullptr };
    class CHP_WorldUI* m_pHpBar{ nullptr };
public:
    static CCthulhu_Tentacle* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
