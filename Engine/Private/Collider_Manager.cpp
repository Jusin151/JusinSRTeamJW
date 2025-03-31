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
	Update_Collision_Floor();
	Update_Collision_Structure();
	Collison_Cube_To_Cube(m_pColliders[CG_PLAYER], m_pColliders[CG_MONSTER]);
	Collison_Cube_To_Cube(m_pColliders[CG_PLAYER], m_pColliders[CG_MONSTER_PROJECTILE_CUBE]);

 	Collison_Cube_To_Cube(m_pColliders[CG_WEAPON], m_pColliders[CG_MONSTER]);
 	Collison_Cube_To_Cube(m_pColliders[CG_PLAYER], m_pColliders[CG_ITEM]);
 	Collison_Cube_To_Cube(m_pColliders[CG_PLAYER], m_pColliders[CG_TRIGGER]);

 	Collison_Cube_To_Cube(m_pColliders[CG_PLAYER], m_pColliders[CG_DOOR]);
	Collison_Cube_To_Cube(m_pColliders[CG_MONSTER_PROJECTILE_CUBE], m_pColliders[CG_DOOR]);
	Collison_Cube_To_Cube(m_pColliders[CG_MONSTER], m_pColliders[CG_DOOR]);

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
				for (auto& dstEntry : m_pColliders[CG_STRUCTURE_WALL])
				{
					if (!Calc_AABB(srcEntry, dstEntry))
						continue;
					if (Calc_Cube_To_Cube(srcEntry, dstEntry))
					{
						srcEntry->Get_Owner()->On_Collision(dstEntry->Get_Owner());
						dstEntry->Get_Owner()->On_Collision(srcEntry->Get_Owner());
						srcEntry->Update_Collider(TEXT("Com_Transform"), srcEntry->Get_Scale());
					}
					

				}
			}
		}

	}
}

void CCollider_Manager::Update_Collision_Floor()
{
	for (int i = 0; i < CG_STRUCTURE_WALL; ++i)
	{
		if (i == CG_WEAPON)
			continue;

		if (i == CG_MONSTER_PROJECTILE_CUBE || i == CG_MONSTER_PROJECTILE_SPHERE ||
			i == CG_PLAYER_PROJECTILE_CUBE || i == CG_PLAYER_PROJECTILE_SPHERE)
		{
			for (auto srcEntry : m_pColliders[i])
			{
				_float fY = 0.f;
				if (!Check_Floor_Ray(srcEntry, fY))
				{
					srcEntry->Get_Owner()->SetActive(false);
				}
			}
		}
		else
		{
			for (auto srcEntry : m_pColliders[i])
			{
				_float fY = 0.f;
				if (Check_Floor_Ray(srcEntry, fY))
				{
					CTransform* pTrans = static_cast<CTransform*>(srcEntry->Get_Owner()->Get_Component(TEXT("Com_Transform")));

					_float3 vPos = pTrans->Get_State(CTransform::STATE_POSITION);

					vPos.y = fY + pTrans->Compute_Scaled().y * 0.5f;

					pTrans->Set_State(CTransform::STATE_POSITION, vPos);

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


	//모든 계산이 끝나면 다시 사용하기 위해 set을 초기화
	setAxes.clear();

	return true;
}

_bool CCollider_Manager::Calc_Sphere_To_Cube(CCollider* src, CCollider* dst)
{

	_float srcRadius = src->Get_Radius();
	_float3 srcPos = src->Get_State(CTransform::STATE_POSITION);
	CCollider_Cube::COL_CUBE_DESC dstDesc = static_cast<CCollider_Cube*>(dst)->Get_Desc();

	_float3 xAxis = dstDesc.fAxisX;  // x 축
	_float3 yAxis = dstDesc.fAxisY;  // y 축
	_float3 zAxis = dstDesc.fAxisZ;  // z 축

	// 큐브의 중심
	_float3 cubeCenter = dstDesc.fPos;

	// 큐브의 8개의 꼭짓점 정의
	const vector<_float3>& corners = dstDesc.vecIndices;

	// 12개의 모서리 정의 (두 점으로 이루어짐)
	vector<pair<_float3, _float3>> edges;

	//x축에 평행한 모서리 4개
	edges.push_back({ corners[0], corners[1] });
	edges.push_back({ corners[2], corners[3] });
	edges.push_back({ corners[4], corners[5] });
	edges.push_back({ corners[6], corners[7] });

	// y축에 평행한 모서리 4개
	edges.push_back({ corners[0], corners[2] });
	edges.push_back({ corners[1], corners[3] });
	edges.push_back({ corners[4], corners[7] });
	edges.push_back({ corners[5], corners[6] });

	// z축에 평행한 모서리 4개
	edges.push_back({ corners[0], corners[4] });
	edges.push_back({ corners[1], corners[5] });
	edges.push_back({ corners[2], corners[7] });
	edges.push_back({ corners[3], corners[6] });

	_float3 fMin = dstDesc.fMin;
	_float3 fMax = dstDesc.fMax;
	

	if (srcPos.x < fMin.x - srcRadius || srcPos.x > fMax.x + srcRadius ||
		srcPos.y < fMin.y - srcRadius || srcPos.y > fMax.y + srcRadius ||
		srcPos.z < fMin.z - srcRadius || srcPos.z > fMax.z + srcRadius)
	{
		return false; // AABB 밖에 있으므로 충돌 없음
	}

	// MTV 벡터 초기화
	_float3 mtv = { 0.f, 0.f, 0.f };
	// 최소 충돌 길이 저장
	_float fDepth = FLT_MAX;

	// 2. 각 면에 대해 구와의 거리 계산
	struct Plane {
		_float3 normal; // 면의 법선 벡터
		_float3 point;  // 면 위의 점
	};

	// 큐브의 각 면 정의
	vector<Plane> planes = { {
		{ dstDesc.fAxisX, cubeCenter + dstDesc.fAxisX * 0.5f }, // x+ 면
		{ -dstDesc.fAxisX,cubeCenter - dstDesc.fAxisX * 0.5f }, // x- 면
		{ dstDesc.fAxisY, cubeCenter + dstDesc.fAxisY * 0.5f }, // y+ 면
		{ -dstDesc.fAxisY, cubeCenter - dstDesc.fAxisY * 0.5f }, // y- 면
		{ dstDesc.fAxisZ, cubeCenter + dstDesc.fAxisZ * 0.5f }, // z+ 면
		{ -dstDesc.fAxisZ, cubeCenter - dstDesc.fAxisZ * 0.5f }  // z- 면
	} };

	// 3. 면 충돌 검사
	for (const auto& plane : planes)
	{
		// 면의 법선 벡터를 정규화
		_float3 norVec = plane.normal;
		norVec.Normalize();

		// 구의 중심에서 면까지의 거리 계산
		_float3 diff = src->Get_State(CTransform::STATE_POSITION) - plane.point;
		_float distanceToPlane = diff.Dot(norVec); // 점과 평면 사이의 거리

		// 거리 절댓값이 반지름보다 작으면 충돌
		if (abs(distanceToPlane) <= srcRadius)
		{
			_float penetrationDepth = srcRadius - abs(distanceToPlane);

			if (penetrationDepth < fDepth) // 가장 깊이 충돌한 면만 반영
			{
				fDepth = penetrationDepth;
				mtv = (distanceToPlane > 0) ? norVec * penetrationDepth : -norVec * penetrationDepth;
			}
		}
	}

	// 4. 각 모서리에 대해 구와의 거리 계산
	for (const auto& edge : edges)
	{
		// 모서리의 두 점을 이용해 모서리 벡터 계산
		_float3 edgeStart = edge.first;
		_float3 edgeEnd = edge.second;

		_float3 edgeVec = edgeEnd - edgeStart;
		_float3 edgeDir = edgeVec.GetNormalized();
		_float3 diff = src->Get_State(CTransform::STATE_POSITION) - edgeStart;

		// 최소 거리 계산 (모서리와 구의 거리), diff를 edgeVec으로 투영후 크기 제곱으로 나눔
		_float t = diff.Dot(edgeVec) / edgeVec.LengthSq();
		t = max(0.f, min(1.f, t));  // 구가 모서리의 양 끝점 사이에 있을 때

		_float3 closestPoint = edgeStart + t * edgeVec;
		_float3 distVec = src->Get_State(CTransform::STATE_POSITION) - closestPoint;
		_float distSq = distVec.LengthSq();

		// 구와 모서리 간의 거리 계산
		if (distSq <= (srcRadius * srcRadius))
		{
			if (distSq > 0)  //  distVec이 0이 아닐 때만 정규화
			{
				_float3 direction = distVec.GetNormalized();
				_float distance = sqrt(distSq);
				_float mtvLength = srcRadius - distance;

				if (mtvLength > 0 && mtvLength < fDepth)  // 최소 MTV 선택
				{
					fDepth = mtvLength;
					mtv = direction * mtvLength;
				}

			}
			else  //  만약 distVec == 0이면 기본 방향 적용
			{
				mtv = edgeDir * srcRadius;
			}
		}

	}

	// MTV가 너무 작으면 충돌 처리하지 않음
	if (mtv.LengthSq() < 0.000001f)
	{
		return false; // 너무 작은 MTV는 충돌로 처리하지 않음
	}

	// MTV 벡터가 충분히 큰 경우에만 충돌로 처리
	src->Set_MTV(mtv);
	dst->Set_MTV(-mtv); // 반대 방향으로 밀어줘야 하므로
	return true; // 충돌 발생

}

_bool CCollider_Manager::Check_Cube_Distance(CCollider* src, CCollider* dst)
{

	// 큐브 큐브 사이 거리가, 큐브와 큐브를 외접하는 두 구의 반지름 합보다 길면 true
	// 아니면 false

	CCollider_Cube::COL_CUBE_DESC srcDesc = static_cast<CCollider_Cube*>(src)->Get_Desc();
	CCollider_Cube::COL_CUBE_DESC dstDesc = static_cast<CCollider_Cube*>(dst)->Get_Desc();



	_float3 fDist = srcDesc.fPos - dstDesc.fPos;

	// 연산 줄이기 위해 제곱으로
	return (srcDesc.fRadius + dstDesc.fRadius) * (srcDesc.fRadius + dstDesc.fRadius) < fDist.LengthSq();
}

_bool CCollider_Manager::Calc_Basic_Axes_Dot(CCollider* src, CCollider* dst)
{
	setAxes.clear();

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
			_float3 vMove = { bestAxis.x, 0.f, bestAxis.z }; // y축 이동을 제외한 MTV 벡터
			_float fDepthExcludeY = sqrtf(vMove.x * vMove.x + vMove.z * vMove.z);
			src->Set_MTV(-bestAxis);
			dst->Set_MTV(bestAxis);
			src->Set_Depth(fDepthExcludeY);
			dst->Set_Depth(fDepthExcludeY);
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


		// penetration 방향 결정
		_float penetration1 = fSrcMax - fDstMin;
		_float penetration2 = fDstMax - fSrcMin;
		_float penetration;
		_float3 axisDir;

		if (penetration1 < penetration2)
		{
			penetration = penetration1;
			axisDir = axis;  // 정방향
		}
		else
		{
			penetration = penetration2;
			axisDir = -axis;  // 반대방향
		}

		if (penetration < fDepth)  // 가장 작은 penetration을 저장
		{
			fDepth = penetration;
			bestAxis = axisDir;
		}
	}

	if (fDepth == FLT_MAX)
		return false;

	src->Set_MTV(-bestAxis.GetNormalized() * fDepth);
	dst->Set_MTV(bestAxis.GetNormalized() * fDepth);
	src->Set_Depth(fDepth);
	dst->Set_Depth(fDepth);


	return true;
}

_bool CCollider_Manager::Calc_AABB(CCollider* src, CCollider* dst)
{

	CCollider_Cube::COL_CUBE_DESC srcDesc = static_cast<CCollider_Cube*>(src)->Get_Desc();
	CCollider_Cube::COL_CUBE_DESC dstDesc = static_cast<CCollider_Cube*>(dst)->Get_Desc();

	_float3 srcMin = srcDesc.fMin;
	_float3 srcMax = srcDesc.fMax;

	_float3 dstMin = dstDesc.fMin;
	_float3 dstMax = dstDesc.fMax;
	

	if (srcMax.x < dstMin.x || dstMax.x < srcMin.x ||
		srcMax.y < dstMin.y || dstMax.y < srcMin.y ||
		srcMax.z < dstMin.z || dstMax.z < srcMin.z)
	{
		return false; // 충돌 없음
	}

	// 충돌 발생
	return true;
}

_bool CCollider_Manager::Check_Floor_Ray(CCollider* src, _float& fY)
{
	for (auto& entry : m_pColliders[CG_STRUCTURE_FLOOR])
	{
		CCollider_Cube::COL_CUBE_DESC entryDesc = static_cast<CCollider_Cube*>(entry)->Get_Desc();

		_float3 vPos = src->Get_State(CTransform::STATE_POSITION);
		_float3 vDir = { 0.f, -1.f, 0.f };


		_float3 vColliderPos = entry->Get_State(CTransform::STATE_POSITION);
		_float3 vNormal = { 0.f, 1.f, 0.f }; // 바닥의 법선벡터

		// 평면 방정식: Ax + By + Cz + D = 0 (여기서 법선 = (A, B, C))
		_float D = -(vNormal.x * vColliderPos.x + vNormal.y * vColliderPos.y + vNormal.z * vColliderPos.z);

		float denominator = vNormal.x * vDir.x + vNormal.y * vDir.y + vNormal.z * vDir.z;
		if (fabs(denominator) > 1e-6) // 0이 아니어야 함 (수직이 아닌 경우)
		{
			float t = -(vNormal.x * vPos.x + vNormal.y * vPos.y + vNormal.z * vPos.z + D) / denominator;
			if (t >= 0.f) // 교차 지점이 레이의 방향에 있음
			{
				_float3 vIntersection = { vPos.x + t * vDir.x, vPos.y + t * vDir.y, vPos.z + t * vDir.z };

				// aabb로 콜라이더가 안에 있는지 판단
				if (vIntersection.x >= entryDesc.fMin.x && vIntersection.x <= entryDesc.fMax.x &&
					vIntersection.z >= entryDesc.fMin.z && vIntersection.z <= entryDesc.fMax.z)
				{
					fY = vPos.y + t * vDir.y;
					return true;
				}
				
				
			}
		}

	}


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
