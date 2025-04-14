#pragma once

#include "Client_Defines.h"
#include "Player.h"
#include "Camera.h"

BEGIN(Engine)
class CSound_Listener;
END

BEGIN(Client)

class CTestCamera final : public CCamera
{
public:

	typedef enum class ECameraShakeState
	{
		Idle,
		Move,
		Hit,
		Attack,
		Explosion,
		Earthquake,
		End
	}CAMERA_STATE;

	typedef struct tagCameraFreeDesc : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensor;
	} CAMERA_FREE_DESC;
private:
	CTestCamera(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTestCamera(const CTestCamera& Prototype);
	virtual ~CTestCamera() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	void HandleMouseInput(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

public:
	void Camera_Lock(_bool bIsLock)
	{
		m_bCameraLocked = bIsLock;
	}

	void Shaking(_float fTimeDelta);


private:
	HRESULT Ready_Components();

private:
	CSound_Listener* m_pListenerCom = { nullptr };
private:
	// �÷��̾� ��ġ�� ����ȭ�ϱ� ����..
	CGameObject* m_pPlayer = { nullptr };
	// �÷��̾� ũ�� �ȹٲٵ���
	_float3				m_vScale = {};

	// ���콺 ��
	POINT				m_ptOldMousePos = {};
	_float				m_fMouseSensor = {};
	_float				m_fSensitivity = 0.001f;
	_float				m_fYaw = 0.0f;
	_float				m_fPitch = 0.0f;
	_bool				m_bCameraLocked = { false };
	
	// ����ŷ ��
	_float				m_fShakeTime = 0.f;
	_float m_fShakeAmount = 0.f;    // ����ũ ���� 
	_float m_fShakeDuration = 0.f;    // ����ũ ���� �ð�
	
public:
	static CTestCamera* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};




END

