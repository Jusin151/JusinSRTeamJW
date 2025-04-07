#pragma once

#include "Component.h"

/* 월드변환을 위한 월드 행렬을 보관한다. */
/* 월드행렬의 변환을 수행한다. == 월드공간에서의 상태를 변환한다 == 모델의 월드공간에서의 움직임을 제어한다. */


BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum TRANSFORMSTATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION};

	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
		_float3     vPos;
	}TRANSFORM_DESC;
private:
	CTransform(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTransform(const CTransform& Prototype);
	virtual ~CTransform() = default;

public:
	_float3 Get_State(TRANSFORMSTATE eState) const {
		return *reinterpret_cast<const _float3*>(&m_WorldMatrix.m[eState][0]);
	}

	const _float4x4* Get_WorldMatrix() {

		return &m_WorldMatrix;
	}

	void Set_State(TRANSFORMSTATE eState, const _float3& vState) {
		memcpy(&m_WorldMatrix.m[eState][0], &vState, sizeof vState);
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	ImGuiDisplay() override;

public:
	HRESULT Bind_Resource();

public:
	_float3 Compute_Scaled() const;
	void Set_Scale(_float fX, _float fY, _float fZ);
	void Go(_float3 dir, _float fTimeDelta);
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Up(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);

	void LookAt(const _float3& vTargetPos);
	void Chase(const _float3& vTargetPos, _float fTimeDelta, _float fMinDistance = 0.1f);
	void Turn(const _float3& vAxis, _float fTimeDelta);
	void Rotation(const _float3& vAxis, _float fRadian);
	void Rotate_EulerAngles(const _float3& vEulerAngles);

	void Rotation_Axis(const _float3& vAxis, float fAngle);

public:

	const _float4x4* Get_WorldMatrix_Inverse() {

		return D3DXMatrixInverse(&m_WorldMatrixInverse, nullptr, &m_WorldMatrix);
	}

	_float4x4 Get_WorldMat()
	{
		return m_WorldMatrix;
	}

	_float3 Get_EulerAngles() const{return m_vEulerAngles;}

private:
	_float4x4				m_WorldMatrix = {};
	_float4x4				m_WorldMatrixInverse = {};

	_float					m_fSpeedPerSec = {};
	_float					m_fRotationPerSec = {};
	_float3					m_vEulerAngles{};

public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END