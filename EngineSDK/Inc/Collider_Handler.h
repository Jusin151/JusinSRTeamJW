#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider_Handler abstract : public CComponent
{	

protected:
	CCollider_Handler(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Handler(const CCollider_Handler& Prototype);
	virtual ~CCollider_Handler() = default;


public:
	virtual HRESULT On_Collision(CCollider_Handler* other) = 0;

	ColliderGroup Get_Type()
	{
		return m_eType;
	}

protected:
	ColliderGroup m_eType = {CG_END};

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
