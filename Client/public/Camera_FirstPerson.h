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

private:
	void Shaking(_float fTimeDelta);

private:
	POINT				m_ptOldMousePos = {};
	_float				m_fMouseSensor = {};
	// 플레이어 위치와 동기화하기 위해..
	CGameObject*		m_pPlayer = { nullptr };
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
	_bool				m_tmpState = { false };
	_float3				m_vScale = {};

public:
	static CCamera_FirstPerson* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};




END