#pragma once

#include "Client_Defines.h"
#include "Player.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_FirstPerson final : public CCamera
{
public:
	typedef struct tagCameraFreeDesc : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensor;
	} CAMERA_FREE_DESC;
private:
	CCamera_FirstPerson(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCamera_FirstPerson(const CCamera_FirstPerson& Prototype);
	virtual ~CCamera_FirstPerson() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	void HandleMouseInput(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

public:
	_float Get_Yaw() const { return m_fYaw; }
	void Set_Yaw(_float fYaw) { m_fYaw = fYaw; }

	void ApplyRecoil(float fAmount)
	{
		m_fRecoil += fAmount;  // 반동에 누적
	}

	void UpdateRecoil(_float fTimeDelta);

	void TriggerShake(_float shakeAmount, _float duration);
	void TriggerShake_HellBoss(_float shakeAmount, _float duration);
	void Shaking_HellBoss(_float fTimeDelta);
	_float3 m_vShakeOffset_HellBoss = {}; // 보스 쉐이크 전용
	_float3 m_vShakeOffset = {};
	void Shaking(_float fTimeDelta);
public:
	void Camera_Lock(_bool bIsLock)
	{
		m_bCameraLocked = bIsLock;
 	}

private:
	POINT				m_ptOldMousePos = {};
	_float				m_fMouseSensor = {};
	// 플레이어 위치와 동기화하기 위해..
	CGameObject*		m_pPlayer = { nullptr };

private:
	_float m_fRecoil = 0.f;       // 반동 강도
	_float m_fRecoilDecay = 10.f;  // 반동 감쇠 속도

private:
	HRESULT Ready_Components();
private:
	_bool				m_bIsRotating;       // 카메라 회전 활성화 상태 (우클릭 시)
	_float				m_fDamping;          // 움직임 감속 계수
	_float3				m_vCurrentVelocity;  // 현재 속도 벡터
	_int				m_iWinCenterX;       // 윈도우 중앙 X 좌표
	_int				m_iWinCenterY;       // 윈도우 중앙 Y 좌표
	_float				m_fYaw = 0.0f;
	_float				m_fPitch = 0.0f;
	_float				m_fSensitivity = 0.001f;
	_float				m_fShakeTime = 0.f;
	_bool				m_bCameraLocked = { false };
	_float3				m_vScale = {};
	_bool m_bSmoothReturn = { false };
	_float m_fShakeAmount = 0.f;    // 쉐이크 강도 
	_float m_fShakeDuration = 0.f;    // 쉐이크 지속 시간
	_bool m_bTriggerShake = false; // 쉐이크 트리거 상태
	_float3 m_vOriginalCameraPosition; // 원래 카메라 위치
	_bool m_bSmoothReturn_HellBoss = false; // 부드럽게 복귀할지 여부
	_float3 m_vShakeBasePosition = {}; 
public:
	static CCamera_FirstPerson* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};




END

