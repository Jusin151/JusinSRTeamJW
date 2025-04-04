#pragma once
#include <Monster_Base.h>
#include <BehaviorTree.h>
BEGIN(Client)
class CCthulhu final : public CMonster_Base
{
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

private:
    CBehaviorTree* m_pBehaviorTree = nullptr;

public:
    static CCthulhu* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();


};
END

