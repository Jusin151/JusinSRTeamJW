#pragma once
#include "Client_Defines.h"
#include "CollisionObject.h"
#include "Serializable.h"

BEGIN(Engine)
class CTransform;
class CCollider;
class CTexture;
END

BEGIN(Client)
class CTrigger : public CCollisionObject
{
public:
    enum class TRIGGER_TYPE : uint8_t
    {
        BUTTON,           // 누를 수 있는 버튼
        INTERACTION     // 상호작용
    };

    typedef struct tagTriggerDesc : public OBJECT_DESC
    {
        TRIGGER_TYPE eType;
        _float3 vPosition;         // 트리거 위치
        _float fActivationRange;   // 근접 트리거의 활성화 범위
        _wstring stTargetTag;        // 영향을 받는 객체의 태그
        bool bStartsActive;        // 시작시 활성화 여부
    }TRIGGER_DESC;

private:
    CTrigger(LPDIRECT3DDEVICE9 pGraphic_Device);
    CTrigger(const CTrigger& Prototype);
    virtual ~CTrigger() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual HRESULT On_Collision(CCollisionObject* other) override;

    void Activate();
    void Deactivate();

    void AddTargetObject(CGameObject* pTarget);

    // 특정 트리거 동작을 구현하기 위한 오버라이드
    void OnTrigger_Activated();
    void OnTrigger_Deactivated();

private:
    CTransform* m_pTransformCom = { nullptr };
    CCollider* m_pColliderCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };

    TRIGGER_TYPE m_eTriggerType = TRIGGER_TYPE::BUTTON;
    bool m_bWasTriggered = false;

    CGameObject* m_TargetObject = { nullptr };

private:
    HRESULT Ready_Components();

public:
    static CTrigger* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
    virtual json Serialize() override;
    virtual void Deserialize(const json& j) override;
};
END