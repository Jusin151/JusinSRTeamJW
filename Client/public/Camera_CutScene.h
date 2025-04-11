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
public:
    static CCamera_CutScene* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free();
private:
    HRESULT Ready_Components();
public:
    void Set_LookTarget(CGameObject* pTarget); // 보스 등 바라볼 대상 설정
    void Set_FixedTransform(const _float3& vEye, const _float3& vAt);
    void Set_CameraDisableDelay(_float fDelay);
private:
    _bool				m_bIsRotating{};       // 카메라 회전 활성화 상태 (우클릭 시)
    _float				m_fDamping{};          // 움직임 감속 계수
    _float3				m_vCurrentVelocity{};  // 현재 속도 벡터
    _int				m_iWinCenterX{};       // 윈도우 중앙 X 좌표
    _int				m_iWinCenterY{};       // 윈도우 중앙 Y 좌표
    _float				m_fYaw = { 0.0f };
    _float				m_fPitch = { 0.0f };
    _float				m_fSensitivity = { 0.001f };
    _float				m_fShakeTime = { 0.f };
    _bool				m_bCameraLocked = { false };
    _float3				m_vScale = {};
private:
    _float3 m_vEye{};
    _float3 m_vAt{};
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

};
