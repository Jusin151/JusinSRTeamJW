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
	virtual HRESULT Render();

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

protected:
	_float4x4				m_WorldMatrix = {};
	// 큐브의 중점 저장용
	_float3					m_fPos = {};


	// �ּ� �̵� ����, ũ��
	_float3					m_fMTV = {0.f, 0.f, 0.f};
	_float					m_fDepth = {0.f};

	// �ڱ� Ÿ�� ���� �ϰ�
	// �浹�ϸ� �ڱ� Ÿ���� ���뿡�� ����, ���� Ÿ���� ������ ����
	// �� �� object���� ���� Ÿ�� ���� üũ
	COLLIDERGROUP			m_eType = { CG_END };
	COLLIDERGROUP			m_eOtherType = { CG_END };
public:

	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;

	
};

END

