#include "Collider_Manager.h"
#include "Collider.h"
#include "Collider_Sphere.h"



CCollider_Manager::CCollider_Manager()
{
}

HRESULT CCollider_Manager::Add_Collider(COLLIDERGROUP eGroup, CCollider* Collider)
{
	if (eGroup >= CG_END ||
		nullptr == Collider)
		return E_FAIL;

	
	m_pColliders[eGroup].push_back(Collider);

	Safe_AddRef(Collider);

	return S_OK;
}


void CCollider_Manager::Clear()
{

	for (auto& list : m_pColliders)
	{
		for (auto& ptr : list)
		{
			Safe_Release(ptr);
			
		}
		list.clear();
	}

		
}

void CCollider_Manager::Update_Collison()
{
	Collison_Sphere_To_Sphere(m_pColliders[CG_PLAYER], m_pColliders[CG_MONSTER]);

	Clear();
}

void CCollider_Manager::Collison_Sphere_To_Sphere(list<CCollider*> src, list<CCollider*> dst)
{
	if (src.empty() || dst.empty())
		return;

	for (auto& srcEntry : src)
	{
		for (auto& dstEntry : dst)
		{
			if (Calc_Sphere_To_Sphere(srcEntry, dstEntry))
			{
				// 충돌한 collider의 타입을 기록함
				// 이제 object lateupdate에서 판정해서 타입별로 충돌 처리 하도록
				srcEntry->Set_Other_Type(dstEntry->Get_Type());
				dstEntry->Set_Other_Type(srcEntry->Get_Type());
				
			}
		}
	}
}

void CCollider_Manager::Collison_Cube_To_Cube(list<CCollider*> src, list<CCollider*> dst)
{
	
}

bool CCollider_Manager::Calc_Sphere_To_Sphere(CCollider* src, CCollider* dst)
{
	_float3 fDistance = src->Get_State(CTransform::STATE_POSITION) - dst->Get_State(CTransform::STATE_POSITION);


	// 반지름 계산 or 가져와서 이제 처리
	_float fRadiusSrc = static_cast<CCollider_Sphere*>(src)->Get_Radius();
	_float fRadiusDst = static_cast<CCollider_Sphere*>(dst)->Get_Radius();

	// 거리 길이
	_float fLength = fDistance.Length();

	// 중점 사이 거리가 반지름 합보다 작으면 충돌
	return fRadiusSrc * fRadiusSrc + fRadiusDst * fRadiusDst > fLength * fLength;
}

bool CCollider_Manager::Calc_Cube_To_Cube(CCollider* src, CCollider* dst)
{
	return false;
}

bool CCollider_Manager::Calc_Cube_To_Sphere(CCollider* src, CCollider* dst)
{
	return false;
}

CCollider_Manager* CCollider_Manager::Create()
{
	return new CCollider_Manager();
}

void CCollider_Manager::Free()
{
	__super::Free();

	Clear();

	m_pColliders.clear();
}
