#pragma once

#include "Collider.h"

class CPlayerCollider : public CCollider
{
private:
	CPlayerCollider(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CPlayerCollider() = default;

public:
	int On_Collision() override;
	CComponent* Clone(void* pArg) override;

public:
	static CPlayerCollider* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

