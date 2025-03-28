#pragma once
#include "Base.h"

BEGIN(Engine)
class CFrustumCull final : public CBase
{
	enum class eHalfSpace
	{
		ON_PLANE = 0,
		NEGATIVE = -1,
		POSITIVE = 1
	};
private:
	CFrustumCull(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CFrustumCull() = default;

public:
	void Update();

public:
	_bool IsPointInFrustum(const _float3& point);
	_bool IsSphereInFrustum(const _float3& point,_float fRadius = 0.f);
	_bool IsAABBInFrustum(const _float3& point, const _float3& scale);

private:
	eHalfSpace ClassifyPoint(const D3DXPLANE& plane, const _float3 pt);
	void ExtractPlanes();
	void NormalizePlane(D3DXPLANE& plane);

private:
	D3DXPLANE m_Planes[6];
	LPDIRECT3DDEVICE9 m_pGraphic_Device = { nullptr };

public:
	static CFrustumCull* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END