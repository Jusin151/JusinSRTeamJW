#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CCollider_Cube : public CCollider
{
public:
	typedef struct tagCol_Cube_Desc
	{
		// 점 계산을 위한 축과 중점
		_float3 fAxisX;
		_float3 fAxisY;
		_float3 fAxisZ;
		_float3 fPos;

		// 점들 저장
		vector<_float3> vecIndices;

		// 큐브에 외접하는 구의 반지름
		_float fRadius;

	}COL_CUBE_DESC;
private:
	CCollider_Cube(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Cube(const CCollider_Cube& Prototype);
	virtual ~CCollider_Cube() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;
	COL_CUBE_DESC& Get_Desc() { return m_tDesc; }

	void Update_Desc();

private:
	COL_CUBE_DESC		m_tDesc;

public:

	static CCollider_Cube* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
