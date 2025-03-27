#pragma once

#include "Component.h"
#include "Transform.h"
#include "VIBuffer.h"

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
		class CCollisionObject*      pOwner = { nullptr };

	}COL_DESC;

protected:
	CCollider(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() = 0;

	virtual HRESULT Update_Collider(const _wstring& strLayerTag, _float3 fScale) = 0;

public:
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

	_float Get_Radius() const 
	{
		return m_fRadius;
	}

	_float3 Get_MTV() { return m_fMTV; }

	void Set_MTV(_float3 fMTV) { m_fMTV = fMTV; }

	_float Get_Depth() { return m_fDepth; }
	// mtv의 길이 저장해놓음
	void Set_Depth(_float fDepth) { m_fDepth = fDepth; }

	_float3 Get_Scale() { return m_fScale; }
	void	Set_Scale(_float3 fScale) { m_fScale = fScale; }

	void Set_Owner(class CCollisionObject* pOwner) { m_pOwner = pOwner; }
	class CCollisionObject* Get_Owner() const { return m_pOwner; }

	_bool Get_bCollision() { return m_bIsCollision; }
	void Set_bCollsion() { m_bIsCollision = true; }

protected:
	_float4x4				m_WorldMatrix = {};
	// 큐브의 중점 저장용
	_float3					m_fLocalPos = {};
	_float3					m_fPos = {};
	// 콜라이더 크기 저장. 
	_float3					m_fScale = {};

	_float					m_fRadius;

	// 최소 이동 벡터랑 길이
	// 충돌 시에만 사용
	_float3					m_fMTV = {0.f, 0.f, 0.f};
	_float					m_fDepth = {0.f};

	class CCollisionObject*		m_pOwner = { nullptr };

	// 충돌 여러번 안하기 위해 사용할 변수
	_bool					m_bIsCollision = { false };

protected:
	//경계 출력을 위한 변수
	LPDIRECT3DINDEXBUFFER9				m_pIB = { nullptr };
	LPDIRECT3DVERTEXBUFFER9				m_pVB = { nullptr };

	_uint								m_iNumVertices = { };
	_uint								m_iVertexStride = { };
	_uint								m_iFVF = {};

	_uint								m_iNumLine = {};
	_uint								m_iNumIndices = {};
	_uint								m_iIndexStride = {};
	D3DFORMAT							m_eIndexFormat = {};

protected:
	HRESULT Create_VertexBuffer();
	HRESULT Create_IndexBuffer();

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

