#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CCollider_Cube : public CCollider
{
private:
	CCollider_Cube(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Cube(const CCollider_Cube& Prototype);
	virtual ~CCollider_Cube() = default;



public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;


private:

public:

	static CCollider_Cube* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
