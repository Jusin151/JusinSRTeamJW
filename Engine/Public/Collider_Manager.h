﻿#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollider;
class CCollider_Handler;

class  CCollider_Manager final : public CBase
{
public:

	


private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;


public:

	// 추가. gameinstance에 등록해서 오브젝트에서 추가. collider에서 하면 오브젝트가 삭제되면 좀 그럴듯?
	HRESULT Add_Collider(COLLIDERGROUP eGroup, CCollider* Collider);
	vector<list<CCollider*>> Get_Colliders()  { return m_pColliders; }

	// update 후에 등록된 모든 collider를 삭제
	void Clear();

	// enum으로 원하는 타입만 충돌 처리 하도록
	void Update_Collison();

	//Helper Function
	
	
	// 벽 충돌할거 먼저 충돌 처리
	void Update_Collision_Structure();

	void Update_Collision_Floor();

	void Update_Collision_Trigger();

	// 거리로만
	void Collison_Sphere_To_Sphere(list<CCollider*> src, list<CCollider*> dst);
	// OBB 충돌
	void Collison_Cube_To_Cube(list<CCollider*> src, list<CCollider*> dst);

	void Collison_Sphere_To_Cube(list<CCollider*> src, list<CCollider*> dst);

	// 중점 거리가 반지름 합보다 작은지 판단
	_bool Calc_Sphere_To_Sphere(CCollider* src, CCollider* dst);

	// 그냥 하면 연산량이 많아서 단계적으로 진행...
	_bool Calc_Cube_To_Cube(CCollider* src, CCollider* dst);

	// 
	_bool Calc_Sphere_To_Cube(CCollider* src, CCollider* dst);

	// 큐브 큐브 사이 거리로 먼저
	_bool Check_Cube_Distance(CCollider* src, CCollider* dst);

	// 각 기본 축으로 먼저 내적하면서 확인
	_bool Calc_Basic_Axes_Dot(CCollider* src, CCollider* dst);


	
	// 추가 계산이 필요하면 이제 외적해서 축을 추가로 구하기
	void Calc_Cross_Axes(CCollider* src, CCollider* dst);

	// 추가한 축으로 내적 계산해서 값 비교
	_bool Calc_AddOn_Axes_Dot(CCollider* src, CCollider* dst);

	// aabb 구현, 벽 충돌에서만 사용
	_bool Calc_AABB(CCollider* src, CCollider* dst);

	// 바닥 체크
	_bool Check_Floor_Ray(CCollider* src, _float& fY);

private:
	inline bool checkCollisionWithGroup(COLLIDERGROUP group, CCollider* srcEntry);

private:

	vector<list<CCollider*>> m_pColliders{ static_cast<size_t>(CG_END) };



	//// 축 계산할때 넣어두는 용도로
	set<_float3> setAxes;
	

public:
	static CCollider_Manager* Create();
	virtual void Free() override;
};

END
