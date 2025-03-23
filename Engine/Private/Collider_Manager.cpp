#include "Collider_Manager.h"
#include "Collider.h"
#include "Collider_Sphere.h"
#include "Collider_Cube.h"



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
	Collison_Cube_To_Cube(m_pColliders[CG_PLAYER], m_pColliders[CG_MONSTER]);
	Collison_Cube_To_Cube(m_pColliders[CG_PLAYER], m_pColliders[CG_MONSTER_PROJECTILE_CUBE]);

 	Collison_Cube_To_Cube(m_pColliders[CG_WEAPON], m_pColliders[CG_MONSTER]);

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
	if (src.empty() || dst.empty())
		return;

	for (auto& srcEntry : src)
	{
		for (auto& dstEntry : dst)
		{
			if (Calc_Cube_To_Cube(srcEntry, dstEntry))
			{
				// 충돌한 collider의 타입을 기록함
				// 이제 object lateupdate에서 판정해서 타입별로 충돌 처리 하도록
				srcEntry->Set_Other_Type(dstEntry->Get_Type());
				dstEntry->Set_Other_Type(srcEntry->Get_Type());

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
	
	if (Check_Cube_Distance(src, dst))
		return false;

	
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

	// 범위 계산
	setAxes.insert(srcDesc.fAxisX.GetNormalized());
	setAxes.insert(srcDesc.fAxisY.GetNormalized());
	setAxes.insert(srcDesc.fAxisZ.GetNormalized());
	setAxes.insert(dstDesc.fAxisX.GetNormalized());
	setAxes.insert(dstDesc.fAxisY.GetNormalized());
	setAxes.insert(dstDesc.fAxisZ.GetNormalized());
	

	for (auto& axis : setAxes)
	{
		_float fSrcMin{ FLT_MAX }, fDstMin{FLT_MAX}, fSrcMax{ -FLT_MAX }, fDstMax{ -FLT_MAX };
		

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
		if (axis.LengthSq() != 0.f && setAxes.find(axis) == setAxes.end() && setAxes.find(-axis) == setAxes.end())
			setAxes.insert(axis);
	}

}

_bool CCollider_Manager::Calc_AddOn_Axes_Dot(CCollider* src, CCollider* dst)
{
	Calc_Cross_Axes(src, dst);

	CCollider_Cube::COL_CUBE_DESC srcDesc = static_cast<CCollider_Cube*>(src)->Get_Desc();
	CCollider_Cube::COL_CUBE_DESC dstDesc = static_cast<CCollider_Cube*>(dst)->Get_Desc();

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
	}

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
