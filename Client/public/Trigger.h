#pragma once
#include "Client_Defines.h"
#include "CollisionObject.h"
#include "Serializable.h"

BEGIN(Engine)
class CSound_Source;
class CShader;
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
        INTERACTION,     // 상호작용
        LEVEL_CHANGE      // 충돌 시 자동 활성화
    };

    typedef struct tagTriggerDesc : public OBJECT_DESC
    {
        TRIGGER_TYPE eType= TRIGGER_TYPE::BUTTON;
        _wstring stTargetTag;        // 영향을 받는 객체의 태그
        _bool bStartsActive;        // 시작시 활성화 여부
        LEVEL eLevel = LEVEL_END;
        _float3 vPos{};
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
    CSound_Source* m_pSoundCom = { nullptr };
    CShader* m_pShaderCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
    CVIBuffer* m_pVIBufferCom = { nullptr };
    CCollider* m_pColliderCom = { nullptr };
    CMaterial* m_pMaterialCom = { nullptr };
    _bool m_bWasTriggered = false;


    class CDoor* m_pTargetObject = { nullptr };
    _uint m_iPrevSize = { 0 };
    TRIGGER_DESC m_vTriggerDesc;
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