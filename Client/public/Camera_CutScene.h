#pragma once

#include "Camera.h"

class CCamera_CutScene : public CCamera
{
public:
    CCamera_CutScene(LPDIRECT3DDEVICE9 pGraphic_Device);
    CCamera_CutScene(const CCamera_CutScene& Prototype);
    virtual ~CCamera_CutScene() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    void Set_CutSceneMove(const _float3& vStart, const _float3& vEnd, _float fSpeed);
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    _float3 Get_CameraPosition() const { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }

public:
    static CCamera_CutScene* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free();
private:
    HRESULT Ready_Components();
    void Shaking(_float fTimeDelta);
public:
    void Set_LookTarget(CGameObject* pTarget); // 보스 등 바라볼 대상 설정
    void Set_FixedTransform(const _float3& vEye, const _float3& vAt);
    void Set_CameraDisableDelay(_float fDelay);
    void Set_CutScenePath(const vector<_float3>& vecPoints, float fSpeed);
    CTransform* Get_Transform() { return m_pTransformCom; }
    _bool Get_DelayDisable() { return m_bDelayDisable; } 
    void  Set_DelayDisable(_bool bType) { m_bDelayDisable = bType; }
    void TriggerShake(_float shakeAmount, _float duration);
private:
    _bool				m_bIsRotating{};       // 카메라 회전 활성화 상태 (우클릭 시)
    _float				m_fDamping{};          // 움직임 감속 계수
    _float3				m_vCurrentVelocity{};  // 현재 속도 벡터
    _int				m_iWinCenterX{};       // 윈도우  X 좌표
    _int				m_iWinCenterY{};       // 윈도우  Y 좌표
    _float				m_fYaw = { 0.0f };
    _float				m_fPitch = { 0.0f };
    _float				m_fSensitivity = { 0.001f };
    _float				m_fShakeTime = { 0.f };
    _bool				m_bCameraLocked = { false };
    _float3				m_vScale = {};

    // 회전 카메라 관련
    _bool m_bRotating = false;
    _float m_fRotateTime = 0.f;
    _float m_fRotateDuration = 0.f;
    _float m_fRotateSpeed = 1.5f;
    _float m_fRotateRadius = 30.f;
    _float m_fRotateHeight = 10.f;
    _float3 m_vRotateCenter = {};

private:
    _bool m_bPhase2Move = false;
    _float3 m_vMid;
    _float m_fWaitTime = 0.f;
    _float3 m_vEye{};
    _float3 m_vAt{};
    _float m_fMoveSpeed2 = { 1.f };
    CGameObject* m_pTarget = nullptr;
    _float3 m_vStart{};
    _float3 m_vEnd{};
    _float3 m_vCurEye{};

    _float m_fLerpT = 0.f;         // 보간 시간
    _float m_fMoveSpeed = 1.f;     // 보간 속도
    _bool m_bMoving = false;       // 이동 중 여부

    _bool  m_bDelayDisable = { false };
    _float m_fDisableTimer = 0.f;
    _float m_fDisableDelay = 0.f;
    CGameObject* m_pPlayer_Camera{};
    CGameObject* m_pPlayer{};
    CGameObject* m_pHellBoss{};


    _float m_fShakeAmount = 0.f;    // 쉐이크 강도 
    _float m_fShakeDuration = 0.f;    // 쉐이크 지속 시간
    _bool m_bTriggerShake = false; // 쉐이크 트리거 상태
    _float3 m_vOriginalCameraPosition; // 원래 카메라 위치

private:  // 
    vector<_float3> m_vecPathPoints;
    size_t m_iCurrentSegment = 0;
   _bool m_bUsePath = false; // 이걸로 경로 기반 체크


};
