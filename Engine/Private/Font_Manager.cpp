#include "Font_Manager.h"
#include "CustomFont.h"

CFont_Manager::CFont_Manager(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device{ pGraphic_Device }
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _wstring& strFontFilePath)
{
	if (nullptr != Find_Font(strFontTag))
		return E_FAIL;

	CCustomFont* pFont = CCustomFont::Create(m_pGraphic_Device, strFontFilePath);
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(strFontTag, pFont);

	return S_OK;
}

HRESULT CFont_Manager::Render_Font(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _float3 vColor)
{
	CCustomFont*		pFont = Find_Font(strFontTag);
	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(strText, vPosition, vColor);
}

HRESULT CFont_Manager::Render_Font_size(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition,const _float2& vSize, _float3 vColor)
{
	CCustomFont* pFont = Find_Font(strFontTag);
	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render_Size(strText, vPosition, vSize, vColor);
}

CCustomFont * CFont_Manager::Find_Font(const wstring & strFontTag)
{
	auto	iter = m_Fonts.find(strFontTag);

	if(iter == m_Fonts.end())		
		return nullptr;

	return iter->second;
}

CFont_Manager * CFont_Manager::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	return new CFont_Manager(pGraphic_Device);
}

void CFont_Manager::Free()
{
	for (auto& Pair : m_Fonts)
		Safe_Release(Pair.second);

	m_Fonts.clear();

	Safe_Release(m_pGraphic_Device);
}

