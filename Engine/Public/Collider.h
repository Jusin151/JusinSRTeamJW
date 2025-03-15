#pragma once

#include "Component.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCollider abstract : public CComponent
{
protected:
	CCollider(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	// 오브젝트에서 설정해서 매니저로 넘기기
	_float3 Get_State(CTransform::TRANSFORMSTATE eState) const {
		return *reinterpret_cast<const _float3*>(&m_WorldMatrix.m[eState][0]);
	}

	void Set_State(CTransform::TRANSFORMSTATE eState, const _float3& vState) {
		memcpy(&m_WorldMatrix.m[eState][0], &vState, sizeof vState);
	}

	void Set_WorldMat(_float4x4 worldMat)
	{
		m_WorldMatrix = worldMat;
	}


	COLLIDERGROUP	Get_Type()
	{
		return m_eType;
	}

	void Set_Type(COLLIDERGROUP eType)
	{
		m_eType = eType;
	}
	

	COLLIDERGROUP	Get_Other_Type()
	{
		return m_eOtherType;
	}

	void	Set_Other_Type(COLLIDERGROUP eOtherType)
	{
		m_eOtherType = eOtherType;
	}

	_float3 Get_MTV() { return m_fMTV; }

	void Set_MTV(_float3 fMTV) { m_fMTV = fMTV; }

	_float3 Get_Scale() { return m_fScale; }
	void Set_Scale(_float3 fScale) { m_fScale = fScale; }

protected:
	_float4x4				m_WorldMatrix = {};
	// 큐브의 중점 저장용
	_float3					m_fPos = {};
	// 월드 행렬 스케일 값 저장용
	_float3					m_fScale = {};

	// 최소 이동 벡터랑 필요하면 길이까지. 길이는 필요 없을지도?
	_float3					m_fMTV = {0.f, 0.f, 0.f};
	_float					m_fDepth = {0.f};

	// 내 타입은 object에서 설정. 
	// 상대 타입은 충돌하면 collidermanager에서 수정해서 다르게 충돌 처리 하도록 
	// 나중에 수정 하면 좋을듯?
	COLLIDERGROUP			m_eType = { CG_END };
	COLLIDERGROUP			m_eOtherType = { CG_END };
public:

	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;

	
};

END

