#include "Collider_Sphere.h"

CCollider_Sphere::CCollider_Sphere(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CCollider(pGraphic_Device)
{
}

CCollider_Sphere::CCollider_Sphere(const CCollider_Sphere& Prototype)
	:CCollider(Prototype)
{
}

HRESULT CCollider_Sphere::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CCollider_Sphere::Initialize(void* pArg)
{
	return S_OK;
}

void CCollider_Sphere::Calc_Radius()
{
	// 한 방향 길이를 구해서 곱해서 넘겨준다
	_float3 fRadiusVec = Get_State(CTransform::STATE_RIGHT);

	

	m_fRadius = fRadiusVec.Length() ;

}

_float CCollider_Sphere::Get_Radius()
{
	if (1.f != m_fRadius)
		Calc_Radius();

	return m_fRadius;
}

CCollider_Sphere* CCollider_Sphere::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollider_Sphere* pInstance = new CCollider_Sphere(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCollider_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider_Sphere::Clone(void* pArg)
{
	CCollider_Sphere* pInstance = new CCollider_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCollider_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Sphere::Free()
{
	__super::Free();
}
