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

	m_tDesc.vecIndices.clear();

	m_tDesc.fAxisX = Get_State(CTransform::STATE_RIGHT);
	m_tDesc.fAxisY = Get_State(CTransform::STATE_UP);
	m_tDesc.fAxisZ = Get_State(CTransform::STATE_LOOK);
	m_tDesc.fPos = Get_State(CTransform::STATE_POSITION);

	/// 픽킹 테스트///////////////////////
	// 각 축 벡터의 길이 계산 (스케일 값)
	//_float fScaleX = m_tDesc.fAxisX.Length();
	//_float fScaleY = m_tDesc.fAxisY.Length();
	//_float fScaleZ = m_tDesc.fAxisZ.Length();

	//// 방향 벡터 정규화
	//_float3 vDirX = m_tDesc.fAxisX.GetNormalized(); 
	//_float3 vDirY = m_tDesc.fAxisY.GetNormalized(); 
	//_float3 vDirZ = m_tDesc.fAxisZ.GetNormalized(); 

	//// 스케일이 적용된 축 벡터 생성
	//_float3 axis[3] = {
	//	vDirX * fScaleX * 0.5f,  // 0.5f는 중심에서 가장자리까지의 거리
	//	vDirY * fScaleY * 0.5f,
	//	vDirZ * fScaleZ * 0.5f
	//};
	/////////////////////////////////////
	_float3 axis[3] = { m_tDesc.fAxisX, m_tDesc.fAxisY, m_tDesc.fAxisZ };
	for (int i = 0; i < 8; ++i) 
	{
		_float3 offset =
			((i & 1) ? axis[0] : -axis[0]) * 0.5f +
			((i & 2) ? axis[1] : -axis[1]) * 0.5f +
			((i & 4) ? axis[2] : -axis[2]) * 0.5f;
		m_tDesc.vecIndices.push_back(m_tDesc.fPos + offset);
	}

	_float fDiagonal = (axis[0].LengthSq() * 0.25f + axis[1].LengthSq() * 0.25f + axis[2].LengthSq() * 0.25f);
	m_tDesc.fRadius = sqrtf(fDiagonal);
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
