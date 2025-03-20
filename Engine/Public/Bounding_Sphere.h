#pragma once
#include "Base.h"
BEGIN(Engine)
class Bounding_Sphere final : public CBase
{
private:
	Bounding_Sphere();
	virtual ~Bounding_Sphere() = default;
public:

private:
	D3DXVECTOR3 m_vCenter;
	_float		m_fDistance;

	virtual void Free() override;
};
END