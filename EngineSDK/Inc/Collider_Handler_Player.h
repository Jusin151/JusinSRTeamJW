#pragma once

#include "Collider_Handler.h"

BEGIN(Engine)

class CCollider_Handler_Player : public CCollider_Handler
{
private:
	CCollider_Handler_Player(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Handler_Player(const CCollider_Handler_Player& Prototype);
	virtual ~CCollider_Handler_Player() = default;


public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT On_Collision(CCollider_Handler* other) override;

	

public:
	static CCollider_Handler_Player* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

