#pragma once

#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
private:
	CFont_Manager(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Add_Font(const _wstring& strFontTag, const _wstring& strFontFilePath);
	HRESULT Render_Font(const _wstring& strFontTag, const _wstring & strText, const _float2 & vPosition, _float3 vColor);

	HRESULT Render_Font_size(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, const _float2& vSize, _float3 vColor);

private:
	LPDIRECT3DDEVICE9 m_pGraphic_Device	= { nullptr };
	map<const _wstring, class CCustomFont*>			m_Fonts;

private:
	class CCustomFont* Find_Font(const wstring& strFontTag);

public:
	static CFont_Manager* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END