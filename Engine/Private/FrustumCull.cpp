#include "FrustumCull.h"

CFrustumCull::CFrustumCull(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}



void CFrustumCull::Update()
{
	ExtractPlanes();
}

_bool CFrustumCull::IsPointInFrustum(const _float3& point)
{
	for (int i = 0; i < 6; i++)
	{
		if (ClassifyPoint(m_Planes[i], point) == eHalfSpace::NEGATIVE)
		{
			return false;
		}
	}
	return true;
}

_bool CFrustumCull::IsSphereInFrustum(const _float3& point, _float fRadius)
{
	for (int i = 0; i < 6; i++)
	{
		// 평면에서 구체 중심까지의 거리 계산
		_float d = D3DXPlaneDotCoord(&m_Planes[i], &point);

		// 구체가 완전히 평면 바깥에 있는 경우
		if (d < -fRadius)
			return false;
	}
	return true;
}

_bool CFrustumCull::IsAABBInFrustum(const _float3& point, const _float3& scale)
{
	for (int i = 0; i < 6; i++)
	{
		// 평면에서 박스 중심
		_float d = D3DXPlaneDotCoord(&m_Planes[i], &point);

		//평면 법선 벡터
		_float3 normal = _float3(m_Planes[i].a, m_Planes[i].b, m_Planes[i].c);

		_float r = scale.x * abs(normal.x) + scale.y * abs(normal.y) + scale.z * abs(normal.z);

		// 박스가 완전히 평면 바깥에 있는 경우
		if (d < -r)
			return false;
	}
	return true;
}

CFrustumCull::eHalfSpace CFrustumCull::ClassifyPoint(const D3DXPLANE& plane, const _float3 pt)
{
	_float d = D3DXPlaneDotCoord(&plane, &pt);
	if (d < 0) return eHalfSpace::NEGATIVE;
	if (d > 0) return eHalfSpace::POSITIVE;
	return eHalfSpace::ON_PLANE;
}

void CFrustumCull::ExtractPlanes()
{
	_float4x4 matViewProj, matView, matProj;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixMultiply(&matViewProj, &matView, &matProj); // 뷰와 프로젝션 행렬 곱하기

	m_Planes[0].a = matViewProj._14 + matViewProj._11;
	m_Planes[0].b = matViewProj._24 + matViewProj._21;
	m_Planes[0].c = matViewProj._34 + matViewProj._31;
	m_Planes[0].d = matViewProj._44 + matViewProj._41;
	// Right clipping plane
	m_Planes[1].a = matViewProj._14 - matViewProj._11;
	m_Planes[1].b = matViewProj._24 - matViewProj._21;
	m_Planes[1].c = matViewProj._34 - matViewProj._31;
	m_Planes[1].d = matViewProj._44 - matViewProj._41;
	// Top clipping plane
	m_Planes[2].a = matViewProj._14 - matViewProj._12;
	m_Planes[2].b = matViewProj._24 - matViewProj._22;
	m_Planes[2].c = matViewProj._34 - matViewProj._32;
	m_Planes[2].d = matViewProj._44 - matViewProj._42;
	// Bottom clipping plane
	m_Planes[3].a = matViewProj._14 + matViewProj._12;
	m_Planes[3].b = matViewProj._24 + matViewProj._22;
	m_Planes[3].c = matViewProj._34 + matViewProj._32;
	m_Planes[3].d = matViewProj._44 + matViewProj._42;
	// Near clipping plane
	m_Planes[4].a = matViewProj._13;
	m_Planes[4].b = matViewProj._23;
	m_Planes[4].c = matViewProj._33;
	m_Planes[4].d = matViewProj._43;
	// Far clipping plane
	m_Planes[5].a = matViewProj._14 - matViewProj._13;
	m_Planes[5].b = matViewProj._24 - matViewProj._23;
	m_Planes[5].c = matViewProj._34 - matViewProj._33;
	m_Planes[5].d = matViewProj._44 - matViewProj._43;

	for (int i = 0; i < 6; i++)
	{
		NormalizePlane(m_Planes[i]);
	}
}

void CFrustumCull::NormalizePlane(D3DXPLANE& plane)
{
	_float fMag;

	fMag = sqrt(plane.a * plane.a + plane.b * plane.b +
		plane.c * plane.c + plane.d * plane.d);

	plane.a /= fMag;
	plane.b /= fMag;
	plane.c /= fMag;
	plane.d /= fMag;
}

CFrustumCull* CFrustumCull::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFrustumCull* pInstance = new CFrustumCull(pGraphic_Device);

	if (!pInstance)
	{
		MSG_BOX("Failed to Created : CFrustumCull");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustumCull::Free()
{
	__super::Free();
	Safe_Release(m_pGraphic_Device);
}
