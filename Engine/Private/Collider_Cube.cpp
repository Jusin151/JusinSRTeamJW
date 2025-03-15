#include "Collider_Cube.h"

CCollider_Cube::CCollider_Cube(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CCollider(pGraphic_Device)
{
}

CCollider_Cube::CCollider_Cube(const CCollider_Cube& Prototype)
	:CCollider(Prototype)
{
}

HRESULT CCollider_Cube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollider_Cube::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CCollider_Cube::Render()
{
	return S_OK;
}

void CCollider_Cube::Update_Desc()
{
	// 축과 점 위치를 월드 행렬에 맞게 고친다

	m_tDesc.fAxisX = Get_State(CTransform::STATE_RIGHT);
	m_tDesc.fAxisY = Get_State(CTransform::STATE_UP);
	m_tDesc.fAxisZ = Get_State(CTransform::STATE_LOOK);
	m_tDesc.fPos = Get_State(CTransform::STATE_POSITION);

	_float3 axis[3] = { m_tDesc.fAxisX, m_tDesc.fAxisY, m_tDesc.fAxisZ };
	for (int i = 0; i < 8; ++i) 
	{
		_float3 offset =
			((i & 1) ? axis[0] : -axis[0]) * 0.5f +
			((i & 2) ? axis[1] : -axis[1]) * 0.5f +
			((i & 4) ? axis[2] : -axis[2]) * 0.5f;
		m_tDesc.vecIndices.push_back(m_fPos + offset);
	}

	m_tDesc.fRadius = sqrt(m_tDesc.fAxisX.LengthSq() + m_tDesc.fAxisY.LengthSq() + m_tDesc.fAxisZ.LengthSq()) * 0.5f;
}

CCollider_Cube* CCollider_Cube::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollider_Cube* pInstance = new CCollider_Cube(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCollider_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider_Cube::Clone(void* pArg)
{
	CCollider_Cube* pInstance = new CCollider_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCollider_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Cube::Free()
{
	__super::Free();
}
