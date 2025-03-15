#pragma once
#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CCustomFont() = default;

public:
	HRESULT Initialize(const wstring& strFontFilePath);
	HRESULT Render(const wstring& strText, const _float2& vPosition, _float3 vColor);

private:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = { nullptr };
	LPD3DXFONT					m_pFont = { nullptr };
	D3DXFONT_DESC				m_FontDesc;

public:
	static CCustomFont* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const wstring& strFontFilePath);
	virtual void Free() override;
};

END