#pragma once

#include "Component.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCollider abstract : public CComponent
{
public:
	// 필요하면 처음 초기화 용으로 사용하면 될듯?
	typedef struct tagCol_Desc
	{
		// 콜라이더 처음 위치. 오브젝트에서 얼마만큼 떨어져 있나 설정
		_float3					fLocalPos = {};
		// 크기
		_float3					fScale = {};

		// 누가 가지고 있는지
		class CGameObject*      pOwner = { nullptr };

		// Owner에 따라 바꿔주기
		COLLIDERGROUP			eType = { CG_END };
	}COL_DESC;

protected:
	CCollider(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() = 0;

	virtual HRESULT Update_Collider(const _wstring& strLayerTag) = 0;

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

	void Set_Owner(class CGameObject* pOwner) { m_pOwner = pOwner; }
	class CGameObject* Get_Owner() const { return m_pOwner; }

	void Set_bCollsion() { m_bIsCollision = true; }
	_bool Get_bCollision() { return m_bIsCollision; }

protected:
	_float4x4				m_WorldMatrix = {};
	// 큐브의 중점 저장용
	_float3					m_fLocalPos = {};
	_float3					m_fPos = {};
	// 콜라이더 크기 저장. 
	_float3					m_fScale = {};

	// 최소 이동 벡터랑 길이
	// 충돌 시에만 사용
	_float3					m_fMTV = {0.f, 0.f, 0.f};
	_float					m_fDepth = {0.f};

	// 내 타입은 object에서 설정. 
	// 상대 타입은 충돌하면 collidermanager에서 수정해서 다르게 충돌 처리 하도록 
	// 나중에 수정 하면 좋을듯?
	COLLIDERGROUP			m_eType = { CG_END };
	COLLIDERGROUP			m_eOtherType = { CG_END };
	class CGameObject*		m_pOwner = { nullptr };

	// 충돌 여러번 안하기 위해 사용할 변수
	_bool					m_bIsCollision = { false };
public:

	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;

	
};

END

