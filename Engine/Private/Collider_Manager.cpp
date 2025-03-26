#include "Collider_Manager.h"
#include "Collider.h"
#include "Collider_Sphere.h"
#include "Collider_Cube.h"
#include "GameObject.h"
#include "CollisionObject.h"



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
	//Collison_Sphere_To_Sphere(m_pColliders[CG_PLAYER], m_pColliders[CG_MONSTER]);
	Update_Collision_Structure();
	Collison_Cube_To_Cube(m_pColliders[CG_PLAYER], m_pColliders[CG_MONSTER]);
	Collison_Cube_To_Cube(m_pColliders[CG_PLAYER], m_pColliders[CG_MONSTER_PROJECTILE_CUBE]);

 	Collison_Cube_To_Cube(m_pColliders[CG_WEAPON], m_pColliders[CG_MONSTER]);

	Clear();
}

void CCollider_Manager::Update_Collision_Structure()
{
	for (int i = 0; i < CG_STRUCTURE_WALL; ++i)
	{
		if (i == CG_PLAYER_PROJECTILE_SPHERE || i == CG_MONSTER_PROJECTILE_SPHERE)
		{
			
		}
		else
		{
			for (auto& srcEntry : m_pColliders[i])
			{
				// 처리 전에 거리 짧은 순으로 정렬
				m_pColliders[CG_STRUCTURE_WALL].sort([srcEntry](const CCollider* a, const CCollider* b) {
					_float3 distA = (a->Get_State(CTransform::STATE_POSITION) -srcEntry->Get_State(CTransform::STATE_POSITION));
					_float3 distB = (b->Get_State(CTransform::STATE_POSITION) - srcEntry->Get_State(CTransform::STATE_POSITION));
					return distA.LengthSq() < distB.LengthSq();
					});

				for (auto& dstEntry : m_pColliders[CG_STRUCTURE_WALL])
				{
					// 일정 거리 넘어가면 다음으로
					if (Check_Cube_Distance(srcEntry, dstEntry))
						break;

					// 일단 가장 가까운 콜라이더만 충돌처리하고 넘어가기
					if (Calc_Cube_To_Cube(srcEntry, dstEntry))
					{
						
						//srcEntry->Set_State(CTransform::STATE_POSITION, srcEntry->Get_MTV());
						srcEntry->Get_Owner()->On_Collision(dstEntry->Get_Owner());
						dstEntry->Get_Owner()->On_Collision(srcEntry->Get_Owner());
						srcEntry->Update_Collider(TEXT("Com_Transform"), srcEntry->Get_Scale());
						
					}

				}
			}
		}
			
	}
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
				srcEntry->Get_Owner()->On_Collision(dstEntry->Get_Owner());
				dstEntry->Get_Owner()->On_Collision(srcEntry->Get_Owner());
				
			}
		}
	}
}

void CCollider_Manager::Collison_Cube_To_Cube(list<CCollider*> src, list<CCollider*> dst)
{
	if (src.empty() || dst.empty())
		return;

	for (auto& srcEntry : src)
	{
		for (auto& dstEntry : dst)
		{
			if (Calc_Cube_To_Cube(srcEntry, dstEntry))
			{
				srcEntry->Get_Owner()->On_Collision(dstEntry->Get_Owner());
				dstEntry->Get_Owner()->On_Collision(srcEntry->Get_Owner());
			}
		}
	}
}

_bool CCollider_Manager::Calc_Sphere_To_Sphere(CCollider* src, CCollider* dst)
{
	_float3 fDir = src->Get_State(CTransform::STATE_POSITION) - dst->Get_State(CTransform::STATE_POSITION);


	// 반지름 계산 or 가져와서 이제 처리
	_float fRadiusSrc = static_cast<CCollider_Sphere*>(src)->Get_Radius();
	_float fRadiusDst = static_cast<CCollider_Sphere*>(dst)->Get_Radius();

	_float fLengthSq = fDir.LengthSq();

	
	// 반지름 합보다 길이 합이 짧으면 충돌
	if ( (fRadiusSrc + fRadiusDst) * (fRadiusSrc + fRadiusDst) >= fLengthSq)
	{
		
		// 충돌 시 MTV 계산
		fDir.Normalize();
		_float fOverlap = fRadiusSrc + fRadiusDst - sqrtf(fLengthSq);  // 겹친 정도 (겹친 길이)

		// MTV 계산 (두 구가 겹치는 정도만큼 이동)
		src->Set_MTV(fDir * fOverlap * 0.5f);
		dst->Set_MTV(-fDir * fOverlap * 0.5f);

		return true;
	}
	else
		return false;

	
	
}

_bool CCollider_Manager::Calc_Cube_To_Cube(CCollider* src, CCollider* dst)
{
	CCollider_Cube::COL_CUBE_DESC srcDesc = static_cast<CCollider_Cube*>(src)->Get_Desc();
	CCollider_Cube::COL_CUBE_DESC dstDesc = static_cast<CCollider_Cube*>(dst)->Get_Desc();
	

	
	if (!Calc_Basic_Axes_Dot(src, dst))
		return false;


	if (!Calc_AddOn_Axes_Dot(src, dst))
		return false;


	// 모든 계산이 끝나면 다시 사용하기 위해 set을 초기화
	setAxes.clear();

	return true;
}

_bool CCollider_Manager::Calc_Cube_To_Sphere(CCollider* src, CCollider* dst)
{
	return false;
}

_bool CCollider_Manager::Check_Cube_Distance(CCollider* src, CCollider* dst)
{

	setAxes.clear();
	// 큐브 큐브 사이 거리가, 큐브와 큐브를 외접하는 두 구의 반지름 합보다 길면 true
	// 아니면 false

	CCollider_Cube::COL_CUBE_DESC srcDesc = static_cast<CCollider_Cube*>(src)->Get_Desc();
	CCollider_Cube::COL_CUBE_DESC dstDesc = static_cast<CCollider_Cube*>(dst)->Get_Desc();

	// 경래야 내가 distance 만들어놨어...
	//_float3 distance = srcDesc.fPos - dstDesc.fPos;

	return srcDesc.fRadius + dstDesc.fRadius < _float3::Distance(srcDesc.fPos, dstDesc.fPos);
}

_bool CCollider_Manager::Calc_Basic_Axes_Dot(CCollider* src, CCollider* dst)
{
	CCollider_Cube::COL_CUBE_DESC srcDesc = static_cast<CCollider_Cube*>(src)->Get_Desc();
	CCollider_Cube::COL_CUBE_DESC dstDesc = static_cast<CCollider_Cube*>(dst)->Get_Desc();

	_float fDepth = FLT_MAX;
	_float3 bestAxis = { 0.f, 0.f, 0.f }; // 최적의 MTV

	// 범위 계산
	setAxes.insert(srcDesc.fAxisX);
	setAxes.insert(srcDesc.fAxisY);
	setAxes.insert(srcDesc.fAxisZ);
	setAxes.insert(dstDesc.fAxisX);
	setAxes.insert(dstDesc.fAxisY);
	setAxes.insert(dstDesc.fAxisZ);
	

	for (auto& axis : setAxes)
	{
		_float fSrcMin{ FLT_MAX }, fDstMin{FLT_MAX}, fSrcMax{ -FLT_MAX }, fDstMax{ -FLT_MAX };
		
		_float fDepth = FLT_MAX;
		_float3 bestAxis = { 0.f, 0.f, 0.f }; // 최적의 MTV

		for (auto& p : srcDesc.vecIndices)
		{
			_float proj = p.Dot(axis);
			fSrcMin = min(fSrcMin, proj);
			fSrcMax = max(fSrcMax, proj);
		}

		for (auto& p : dstDesc.vecIndices)
		{
			_float proj = p.Dot(axis);
			fDstMin = min(fDstMin, proj);
			fDstMax = max(fDstMax, proj);
		}

		if ((fSrcMax < fDstMin || fDstMax < fSrcMin)) // 범위가 안겹치면 충돌 안함
			return false;

		_float penetration = min(fSrcMax - fDstMin, fDstMax - fSrcMin);

		if (penetration < fDepth)  // 가장 작은 penetration을 저장
		{
			fDepth = penetration;
			bestAxis = axis;
			src->Set_MTV(-bestAxis);
			dst->Set_MTV(bestAxis);
			src->Set_Depth(fDepth);
			dst->Set_Depth(fDepth);
		}
	}
	
	return true;
}


void CCollider_Manager::Calc_Cross_Axes(CCollider* src, CCollider* dst)
{
	// 일단 추가만 해놓고 로직 좀 깔끔하게 수정 예정...
	CCollider_Cube::COL_CUBE_DESC srcDesc = static_cast<CCollider_Cube*>(src)->Get_Desc();
	CCollider_Cube::COL_CUBE_DESC dstDesc = static_cast<CCollider_Cube*>(dst)->Get_Desc();

	_float3 srcAxisX = srcDesc.fAxisX.GetNormalized();
	_float3 srcAxisY = srcDesc.fAxisY.GetNormalized();
	_float3 srcAxisZ = srcDesc.fAxisZ.GetNormalized();

	_float3 dstAxisX = dstDesc.fAxisX.GetNormalized();
	_float3 dstAxisY = dstDesc.fAxisY.GetNormalized();
	_float3 dstAxisZ = dstDesc.fAxisZ.GetNormalized();

	vector<_float3> tempAxes;

	// 외적 값 일단 저장
	tempAxes.push_back(srcAxisX.Cross(dstAxisX));
	tempAxes.push_back(srcAxisX.Cross(dstAxisY));
	tempAxes.push_back(srcAxisX.Cross(dstAxisZ));

	tempAxes.push_back(srcAxisY.Cross(dstAxisX));
	tempAxes.push_back(srcAxisY.Cross(dstAxisY));
	tempAxes.push_back(srcAxisY.Cross(dstAxisZ));

	tempAxes.push_back(srcAxisZ.Cross(dstAxisX));
	tempAxes.push_back(srcAxisZ.Cross(dstAxisY));
	tempAxes.push_back(srcAxisZ.Cross(dstAxisZ));

	
	
	
	for (auto& axis : tempAxes)
	{
		// 크기가 0(평행)이거나 같은 방향이 있거나, 반대 방향이 있으면 추가 안하고 없으면 추가

		if (axis.LengthSq() == 0.f)
			continue; // 크기가 0이면 스킵

		bool bDuplicate = false;
		for (const auto& existingAxis : setAxes)
		{
			if (axis.IsParallel(axis)) // 기존 축과 같은 방향이면 추가 안 함
			{
				bDuplicate = true;
				break;
			}
		}
		if (!bDuplicate)
			setAxes.insert(axis);
	}

}

_bool CCollider_Manager::Calc_AddOn_Axes_Dot(CCollider* src, CCollider* dst)
{
	
	Calc_Cross_Axes(src, dst);
	

	CCollider_Cube::COL_CUBE_DESC srcDesc = static_cast<CCollider_Cube*>(src)->Get_Desc();
	CCollider_Cube::COL_CUBE_DESC dstDesc = static_cast<CCollider_Cube*>(dst)->Get_Desc();

	_float fDepth = FLT_MAX;
	_float3 bestAxis = { 0.f, 0.f, 0.f }; // 최적의 MTV

	for (auto& axis : setAxes)
	{
		_float fSrcMin{ FLT_MAX }, fDstMin{ FLT_MAX }, fSrcMax{ -FLT_MAX }, fDstMax{ -FLT_MAX };


		for (auto& p : srcDesc.vecIndices)
		{
			_float proj = p.Dot(axis);
			fSrcMin = min(fSrcMin, proj);
			fSrcMax = max(fSrcMax, proj);
		}

		for (auto& p : dstDesc.vecIndices)
		{
			_float proj = p.Dot(axis);
			fDstMin = min(fDstMin, proj);
			fDstMax = max(fDstMax, proj);
		}

		if ((fSrcMax < fDstMin || fDstMax < fSrcMin)) // 범위가 안겹치면 충돌 안함
			return false;

		// 겹치는 범위를 계산해서 가장 작게 겹치는 걸 이제 mtv로
		_float penetration = min(fSrcMax - fDstMin, fDstMax - fSrcMin);

		if (penetration < fDepth)  // 가장 작은 penetration을 저장
		{
			fDepth = penetration;
			bestAxis = axis;
			src->Set_MTV(-bestAxis);
			dst->Set_MTV(bestAxis);
			src->Set_Depth(fDepth);
			dst->Set_Depth(fDepth);
		}
	}

	return true;
}

_bool CCollider_Manager::Calc_AABB(CCollider* src, CCollider* dst)
{
	//_float3 fDir = src->Get_State(CTransform::STATE_POSITION) - dst->Get_State(CTransform::STATE_POSITION);

	//CCollider_Cube::COL_CUBE_DESC srcDesc = static_cast<CCollider_Cube*>(src)->Get_Desc();
	//CCollider_Cube::COL_CUBE_DESC dstDesc = static_cast<CCollider_Cube*>(dst)->Get_Desc();

	//// 반지름 계산 or 가져와서 이제 처리
	//_float fRadiusSrc = srcDesc.fRadius;
	//_float fRadiusDst = dstDesc.fRadius;

	//_float fLengthSq = fDir.LengthSq();


	//// 반지름 합보다 길이 합이 짧으면 충돌
	//if ((fRadiusSrc + fRadiusDst) * (fRadiusSrc + fRadiusDst) >= fLengthSq)
	//{

	//	// 충돌 시 MTV 계산
	//	fDir.Normalize();
	//	_float fOverlap = fRadiusSrc + fRadiusDst - sqrtf(fLengthSq);  // 겹친 정도 (겹친 길이)

	//	// MTV 계산 (두 구가 겹치는 정도만큼 이동)
	//	src->Set_MTV(fDir * fOverlap * 0.5f);
	//	dst->Set_MTV(-fDir * fOverlap * 0.5f);

	//	return true;
	//}
	//else
	//	return false;

	CCollider_Cube::COL_CUBE_DESC srcDesc = static_cast<CCollider_Cube*>(src)->Get_Desc();
	CCollider_Cube::COL_CUBE_DESC dstDesc = static_cast<CCollider_Cube*>(dst)->Get_Desc();

	// AABB의 최소값(min)과 최대값(max)을 계산
	_float3 srcMin = { FLT_MAX, FLT_MAX, FLT_MAX };
	_float3 srcMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	for (const auto& p : srcDesc.vecIndices)
	{
		srcMin = min(srcMin, p);
		srcMax = max(srcMax, p);
	}

	_float3 dstMin = { FLT_MAX, FLT_MAX, FLT_MAX };
	_float3 dstMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	for (const auto& p : dstDesc.vecIndices)
	{
		dstMin = min(dstMin, p);
		dstMax = max(dstMax, p);
	}

	if (srcMax.x < dstMin.x || dstMax.x < srcMin.x ||
		srcMax.y < dstMin.y || dstMax.y < srcMin.y ||
		srcMax.z < dstMin.z || dstMax.z < srcMin.z)
	{
		return false; // 충돌 없음
	}

	// 충돌 발생 시 MTV 계산
	_float3 mtv = { 0.f, 0.f, 0.f };
	_float depth = FLT_MAX;

	// x축에서 겹치는 정도 계산
	if (srcMax.x > dstMin.x && srcMin.x < dstMax.x)
	{
		_float xPenetration = min(srcMax.x - dstMin.x, dstMax.x - srcMin.x);
		if (xPenetration < depth)
		{
			depth = xPenetration;
			mtv.x = (srcDesc.fPos.x < dstDesc.fPos.x) ? -xPenetration : xPenetration;
		}
	}

	// y축에서 겹치는 정도 계산
	if (srcMax.y > dstMin.y && srcMin.y < dstMax.y)
	{
		_float yPenetration = min(srcMax.y - dstMin.y, dstMax.y - srcMin.y);
		if (yPenetration < depth)
		{
			depth = yPenetration;
			mtv.y = (srcDesc.fPos.y < dstDesc.fPos.y) ? -yPenetration : yPenetration;
		}
	}

	// z축에서 겹치는 정도 계산
	if (srcMax.z > dstMin.z && srcMin.z < dstMax.z)
	{
		_float zPenetration = min(srcMax.z - dstMin.z, dstMax.z - srcMin.z);
		if (zPenetration < depth)
		{
			depth = zPenetration;
			mtv.z = (srcDesc.fPos.z < dstDesc.fPos.z) ? -zPenetration : zPenetration;
		}
	}

	// MTV를 각각 src와 dst에 적용하여 객체를 밀어준다
	src->Set_MTV(mtv);
	dst->Set_MTV(-mtv); // dst는 반대 방향으로 밀기

	// 충돌 발생
	return true;
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
