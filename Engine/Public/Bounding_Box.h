#pragma once
#include "Base.h"
BEGIN(Engine)
class CBounding_Box final : public CBase
{
private:
	friend class CParticle_System;
	friend class CSnow_Particle_System;
	CBounding_Box();
	virtual ~CBounding_Box() = default;
public:
	bool Is_Point_Inside(D3DXVECTOR3& p);

private:
	D3DXVECTOR3 m_vMin;
	D3DXVECTOR3 m_vMax;

	virtual void Free() override;
};

END