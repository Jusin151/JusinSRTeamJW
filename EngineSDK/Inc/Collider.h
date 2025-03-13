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


	COLLIDERGROUP	Get_Type()
	{
		return m_eType;
	}
	void	Set_Other_Type(COLLIDERGROUP eOtherType)
	{
		m_eOtherType = eOtherType;
	}

	COLLIDERGROUP	Get_Other_Type()
	{
		return m_eOtherType;
	}

protected:
	_float4x4				m_WorldMatrix = {};
	// 큐브의 중점 저장용
	_float3					m_fPos = {};

	// 자기 타입 저장 하고
	// 충돌하면 자기 타입을 상대에게 저장, 상대 타입은 나에게 저장
	// 그 후 object에서 상대 타입 보고 체크
	COLLIDERGROUP			m_eType = { CG_END };
	COLLIDERGROUP			m_eOtherType = { CG_END };
public:

	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;

	
};

END

