#pragma once
#include "Client_Defines.h"
#include "CollisionObject.h"

BEGIN(Engine)
class CShader;
class CTransform;
class CCollider;
class CTexture;
class CVIBuffer;
class CMaterial;
END

BEGIN(Client)
class CDoor : public CCollisionObject
{
public:
    enum class DOOR_STATE : uint8_t
    {
        CLOSED,     // 닫힌 상태
        OPENING,    // 열리는 중
        OPEN,       // 열린 상태
        CLOSING     // 닫히는 중
    };

    enum class DOOR_COLOR : uint8_t
    {
        BLUE,    // 파란색 문
        RED,     // 빨간색 문
        YELLOW,  // 노란색 문
        NORMAL   // 색상 없는 일반 문 (열쇠 없이 열림)
    };

    enum class DOOR_TYPE : uint8_t
    {
        NORMAL,     // 일반 문 (바로 열림)
        KEY,         // 열쇠가 필요한 문
        TRIGGER
    };

    typedef struct tagDoorDesc : public OBJECT_DESC
    {
        DOOR_TYPE eType;          // 문 유형
        DOOR_COLOR eColor;        // 문 색상
        _float fSlideDistance;    // 문이 내려가는 거리
        _wstring stKeyItemTag;    // 필요한 열쇠 아이템 태그 (KEY 타입인 경우)
        bool bStartsActive;       // 시작시 활성화 여부
    }DOOR_DESC;

private:
    CDoor(LPDIRECT3DDEVICE9 pGraphic_Device);
    CDoor(const CDoor& Prototype);
    virtual ~CDoor() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
private:
    HRESULT Ready_Components();
public:
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
public:
    virtual HRESULT Render() override;
    virtual HRESULT On_Collision(CCollisionObject* other) override;
public:
    // 문 열기/닫기 함수
    void Open_Door();
    void Close_Door();

    // 문 상태 확인
    _bool IsOpen() const { return m_eDoorState == DOOR_STATE::OPEN; }
    _bool IsClosed() const { return m_eDoorState == DOOR_STATE::CLOSED; }
    _bool IsMoving() const { return m_eDoorState == DOOR_STATE::OPENING || m_eDoorState == DOOR_STATE::CLOSING; }

    // 열쇠 확인 함수
    void TryOpen(CCollisionObject* pPlayer);
    void SetKeyRequired(const _wstring& keyItemTag) { m_stKeyItemTag = keyItemTag; }

private:
    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();
    void UpdateDoorTransform();

private:
    CTransform* m_pTransformCom = { nullptr };
    CCollider*  m_pColliderCom = { nullptr };
    CTexture*   m_pTextureCom = { nullptr };
    CVIBuffer*  m_pVIBufferCom = { nullptr };
    CMaterial*  m_pMaterialCom = { nullptr };
    CShader*    m_pShaderCom = { nullptr };
private:

    DOOR_STATE m_eDoorState = DOOR_STATE::CLOSED;
    DOOR_TYPE m_eDoorType = DOOR_TYPE::NORMAL;

    // 문 애니메이션 관련 변수
    _float m_fDoorOpenAmount = 0.0f;    // 0.0f (닫힘) ~ 1.0f (완전히 열림)
    _float m_fDoorOpenSpeed = 2.0f;     // 초당 열리는 속도
    _float3 m_vRotationAxis = { 0.0f, 1.0f, 0.0f };  // 회전축(기본값은 Y축)
    _float m_fOpenAngle = 90.0f;        // 문이 열리는 각도(기본값 90도)

    DOOR_COLOR m_eDoorColor = DOOR_COLOR::NORMAL;
    _float m_fSlideDistance = 2.0f;     // 기본 슬라이드 거리
    _float3 m_vOriginalPosition;        // 원래 위치
    _float3 m_vSlidePosition;           // 최종 슬라이드 위치

    // 열쇠 관련 변수
    _wstring m_stKeyItemTag = L"";      // 필요한 열쇠 아이템 태그

    // 원래 위치와 회전 저장
    _float3 m_vOriginalRotation;
    _float3 m_vOpenRotation;

public:
    static CDoor* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
    virtual json Serialize() override;
    virtual void Deserialize(const json& j) override;
};
END