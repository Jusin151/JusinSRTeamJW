#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Free final : public CCamera
{
public:
	typedef struct tagCameraFreeDesc : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensor;
	} CAMERA_FREE_DESC;
private:
	CCamera_Free(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCamera_Free(const CCamera_Free& Prototype);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;	

private:
	POINT				m_ptOldMousePos = {};
	_float				m_fMouseSensor = {};

	_bool				m_bIsRotating;       // 카메라 회전 활성화 상태 (우클릭 시)
	_float				m_fDamping;          // 움직임 감속 계수
	_float3				m_vCurrentVelocity;  // 현재 속도 벡터
	_int				m_iWinCenterX;       // 윈도우 중앙 X 좌표
	_int				m_iWinCenterY;       // 윈도우 중앙 Y 좌표
private:
	HRESULT Ready_Components();

public:
	static CCamera_Free* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};




END