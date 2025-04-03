#pragma once
#include <Monster_Base.h>
#include <CBehaviorTree.h>
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
    virtual HRESULT On_Collision(CCollisionObject* other) { return S_OK; }
    void Billboarding(_float fTimeDelta);
private:
    CBehaviorTree* m_pBehaviorTree = nullptr;

public:
    virtual CGameObject* Clone(void* pArg) PURE;
    virtual void Free();
};
END

