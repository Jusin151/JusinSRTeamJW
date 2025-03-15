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
