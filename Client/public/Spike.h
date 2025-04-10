#pragma once
#include <Monster_Base.h>
BEGIN(Client)

class CSpike final : public CMonster_Base
{
public:
    enum class SPIKE_STATE
    {
        ATTACK,
        DISAPPEAR
    };
    enum class SPIKE_TYPE
    {
        CENTER,
        SIDE
    };
private:
    CSpike(LPDIRECT3DDEVICE9 pGraphic_Device);
    CSpike(const CSpike& Prototype);
    virtual ~CSpike() = default;

public:
    virtual HRESULT Initialize_Prototype()override;
    virtual HRESULT Initialize(void* pArg)override;
    virtual void Priority_Update(_float fTimeDelta)override;
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    virtual HRESULT Render()override;

    virtual void Reset() override;

public:
    virtual HRESULT On_Collision(CCollisionObject* other);
    void Billboarding(_float fTimeDelta);
    void Select_Pattern(_float fTimeDelta) override;
    auto Get_State() const { return m_eState; }
    void Set_SpikeType(SPIKE_TYPE eType)
    {
        m_eSpikeType = eType;
        m_fFrame = 0.f;
        m_iCurrentFrame = m_mapStateTextures[m_eSpikeType][m_eState][(_uint)m_fFrame];
    }
    void Active_Animaiton(_bool bFlag) { m_bUpdateAnimation = bFlag; }
    void Flip(_bool bIsRight);

    void Set_Position(const _float3& vPos);
    _bool IsAnimationFinished() const;
private:
    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();

    HRESULT Ready_Components();
    void Init_Textures();
    void Update_Animation(_float fTimeDelta);
    void Attack();
private:
    unordered_map<SPIKE_TYPE,unordered_map<SPIKE_STATE, vector<_uint>>> m_mapStateTextures;
    _float m_fFrame = 0.f;
    _float m_fAnimationSpeed = 12.f;
    SPIKE_STATE  m_eState = SPIKE_STATE::ATTACK;
    SPIKE_TYPE  m_eSpikeType = SPIKE_TYPE::SIDE;

    _bool  m_bUpdateAnimation{ true };
    _bool m_bCanHit{ false };
    _bool m_bCanAttack{ false };
    _bool m_bAnimFinished{ false };
    _bool m_bIsRight{ false };

public:
    static CSpike* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
