#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);

	/* 마우스의 윈도우상의 위치를 구하고 */
	/* 뷰스페이스 상에서 마우스 레이로 변형하여 */
	/* 월드까지 변환시켜서 보관하자. */
	void Update();

	_bool Picking_InWorld(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC);
	_bool Picking_InLocal(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC);

	void Transform_ToLocalSpace(const _float4x4& WorldMatrixInverse);

private:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = { nullptr };

private:
	HWND			m_hWnd = {};
	_uint			m_iWinSizeX{}, m_iWinSizeY{};
	
	_float3			m_vMouseRay = {};
	_float3			m_vMousePos = {};

	_float3			m_vLocalMouseRay = {};
	_float3			m_vLocalMousePos = {};

public:
	static CPicking* Create(LPDIRECT3DDEVICE9 pGraphic_Device, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;
};

END