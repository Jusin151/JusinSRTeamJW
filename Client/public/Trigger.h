#pragma once
#include "Client_Defines.h"
#include "CollisionObject.h"
#include "Serializable.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer;
class CCollider;
class CMaterial;
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
        _wstring stTargetTag;        // 영향을 받는 객체의 태그
        _bool bStartsActive;        // 시작시 활성화 여부
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

    void AddTargetObject(class CDoor* pTarget);

    void OnTrigger_Activated();
    void OnTrigger_Deactivated();
private:
    void Find_Target();

private:
    CTexture* m_pTextureCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
    CVIBuffer* m_pVIBufferCom = { nullptr };
    CCollider* m_pColliderCom = { nullptr };
    CMaterial* m_pMaterialCom = { nullptr };

    TRIGGER_TYPE m_eTriggerType = TRIGGER_TYPE::BUTTON;
    _bool m_bWasTriggered = false;


    class CDoor* m_pTargetObject = { nullptr };
    _wstring m_stTargetTag;
    set<class CCollisionObject*> m_setMonsters;
    _bool m_bInitMonsters = { false };
    _uint m_iPrevSize = { 0 };

private:
    HRESULT Ready_Components();
    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();

public:
    static CTrigger* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
    virtual json Serialize() override;
    virtual void Deserialize(const json& j) override;
};
END