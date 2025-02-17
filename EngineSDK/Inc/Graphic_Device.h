#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)

private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	HRESULT		Initialize(HWND hWnd, _bool isWindowed, _uint iWinSizeX, _uint iWinSizeY);
	void		Render_Begin();
	void		Render_End(HWND hWnd = nullptr);	

private:
	void		Set_Parameters(D3DPRESENT_PARAMETERS& d3dpp,
		HWND hWnd, _bool isWindowed, _uint iWinSizeX, _uint iWinSizeY);

private:
	LPDIRECT3D9			m_pSDK;		// 1번에 해당하는 객체
	LPDIRECT3DDEVICE9	m_pDevice;	// 3번에 해당하는 객체

	LPD3DXSPRITE		m_pSprite;	// 2d 이미지를 출력하기 위한 용도
	LPD3DXFONT			m_pFont;

public:
	virtual void Free() override;
};

END