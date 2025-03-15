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
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	POINT				m_ptOldMousePos = {};
	_float				m_fMouseSensor = {};
	// 플레이어 위치와 동기화하기 위해..
	CGameObject*		m_pPlayer = { nullptr };
private:
	HRESULT Ready_Components();



public:
	static CCamera_FirstPerson* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};




END