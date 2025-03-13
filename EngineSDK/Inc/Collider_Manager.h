#pragma once

#include "Base.h"
#include <queue>

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


	// update 후에 등록된 모든 collider를 삭제
	void Clear();

	// enum으로 원하는 타입만 충돌 처리 하도록
	void Update_Collison();

	// 거리로만
	void Collison_Sphere_To_Sphere( list<CCollider*> src, list<CCollider*> dst);
	// SAT 써서
	void Collison_Cube_To_Cube(list<CCollider*> src, list<CCollider*> dst);

	// 중점 거리가 반지름 합보다 작은지 판단
	bool Calc_Sphere_To_Sphere(CCollider* src, CCollider* dst);

	// 구로 충돌 체크 끝내고
	bool Calc_Cube_To_Cube(CCollider* src, CCollider* dst);

	// 위에꺼 끝내고.
	bool Calc_Cube_To_Sphere(CCollider* src, CCollider* dst);

private:

	vector<list<CCollider*>> m_pColliders{ static_cast<size_t>(CG_END) };



	//// 축 계산할때 넣어두는 용도로, key가 dir, value는 depth
	//map<_float3, _float> m_Axes;
	//// 거리 작은 순서대로 넣어주기, 충돌 후 이동시킬때 사용
	//priority_queue<_float, vector<_float>, greater<_float>> m_Depths;
	

public:
	static CCollider_Manager* Create();
	virtual void Free() override;
};

END
