#include "Picking.h"

CPicking::CPicking(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device { pGraphic_Device }
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{

	m_hWnd = hWnd;
	m_iWinSizeX = iWinSizeX;
	m_iWinSizeY = iWinSizeY;

	return S_OK;
}

void CPicking::Update()
{
	POINT		ptMouse = {};

	/*1. 뷰포트 상(윈도우공간)의 마우스 위치를 구하자. */
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	/*2. 투영스페이스로 옮기자. 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1/w */
	_float4			vPosition = { };

	vPosition.x = ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
	vPosition.y = ptMouse.y / (m_iWinSizeY * -0.5f) + 1.f;
	vPosition.z = 0.0f;
	vPosition.w = 1.f;

	/*3. 뷰스페이스로 옮기자. 로컬위치 * 월드행렬 * 뷰행렬 */
	_float4x4		ProjMatrix{};
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);
	D3DXMatrixInverse(&ProjMatrix, nullptr, &ProjMatrix);

	D3DXVec4Transform(&vPosition, &vPosition, &ProjMatrix);

	m_vMousePos = _float3(0.f, 0.f, 0.f);
	m_vMouseRay = _float3(vPosition.x, vPosition.y, vPosition.z);

	/*3. 월드 스페이스로 옮기자. 로컬위치 * 월드행렬*/
	_float4x4		ViewMatrix{};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	D3DXVec3TransformCoord(&m_vMousePos, &m_vMousePos, &ViewMatrix);
	D3DXVec3TransformNormal(&m_vMouseRay, &m_vMouseRay, &ViewMatrix);

	D3DXVec3Normalize(&m_vMouseRay, &m_vMouseRay);

}

_bool CPicking::Picking_InWorld(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC)
{
	_float		fU, fV, fDist;

	_bool		isPicked = D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vMousePos, &m_vMouseRay, &fU, &fV, &fDist);

	vPickedPos = m_vMousePos + m_vMouseRay * fDist;

	return isPicked;
}

_bool CPicking::Picking_InLocal(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC)
{
	_float		fU, fV, fDist;

	_bool		isPicked = D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vLocalMousePos, &m_vLocalMouseRay, &fU, &fV, &fDist);

	vPickedPos = m_vLocalMousePos + m_vLocalMouseRay * fDist;

	return isPicked;
}

void CPicking::Transform_ToLocalSpace(const _float4x4& WorldMatrixInverse)
{

	D3DXVec3TransformCoord(&m_vLocalMousePos, &m_vMousePos, &WorldMatrixInverse);
	D3DXVec3TransformNormal(&m_vLocalMouseRay, &m_vMouseRay, &WorldMatrixInverse);
}

CPicking* CPicking::Create(LPDIRECT3DDEVICE9 pGraphic_Device, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	CPicking* pInstance = new CPicking(pGraphic_Device);

	if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
	{
		MSG_BOX("Failed to Created : CPicking");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking::Free()
{
	__super::Free();

	Safe_Release(m_pGraphic_Device);
}
