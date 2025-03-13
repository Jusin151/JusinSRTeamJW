#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CCollider_Sphere : public CCollider
{
private:
	CCollider_Sphere(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Sphere(const CCollider_Sphere& Prototype);
	virtual ~CCollider_Sphere() = default;



public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	void Calc_Radius();
	// 반지름이 계산 안되있으면 계산 후 출력
	_float Get_Radius();


private:
	// 반지름
	_float m_fRadius = {-1.f};
	

public:

	static CCollider_Sphere* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
