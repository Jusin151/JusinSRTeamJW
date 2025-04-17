#include "..\Public\CustomFont.h"

CCustomFont::CCustomFont(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device{ pGraphic_Device }
{
	Safe_AddRef(m_pGraphic_Device);
	ZeroMemory(&m_FontDesc, sizeof(D3DXFONT_DESC));
}

HRESULT CCustomFont::Initialize(const wstring& strFontFilePath)
{
	// 폰트 파일 경로에서 폰트 이름 추출 또는 기본 폰트 사용
	// DX9에서는 TTF 파일을 직접 로드하는 대신 시스템 폰트를 사용

	m_FontDesc.Height = 16;                  // 폰트 높이
	m_FontDesc.Width = 0;                    // 폰트 너비 (0은 자동)
	m_FontDesc.Weight = FW_NORMAL;           // 폰트 두께
	m_FontDesc.MipLevels = 1;
	m_FontDesc.Italic = FALSE;
	m_FontDesc.CharSet = DEFAULT_CHARSET;
	m_FontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	m_FontDesc.Quality = DEFAULT_QUALITY;
	m_FontDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	//wcscpy_s(m_FontDesc.FaceName, )
	wcscpy_s(m_FontDesc.FaceName, L"Wheaton Capitals");  // 기본 폰트 이름

	// 폰트 파일 이름이 지정된 경우 사용
	if (!strFontFilePath.empty())
	{
		// 간단한 예: 파일 이름에서 .ttf 확장자 제거
 		wstring strFontName = strFontFilePath;
		size_t pos = strFontName.rfind(L'\\');
		if (pos != wstring::npos)
			strFontName = strFontName.substr(pos + 1);

		pos = strFontName.rfind(L'.');
		if (pos != wstring::npos)
			strFontName = strFontName.substr(0, pos);

		size_t lastSlashPos = strFontName.find_last_of(L'/'); // 와이드 문자 L'/' 사용

		std::wstring fileName;

		// '/' 문자를 찾았는지 확인합니다.
		if (lastSlashPos != std::wstring::npos) {
			// 찾았다면, '/' 다음 위치부터 끝까지 부분 문자열을 추출합니다.
			strFontName = strFontName.substr(lastSlashPos + 1);
		}

		if (!strFontName.empty() && strFontName.length() < LF_FACESIZE)
			wcscpy_s(m_FontDesc.FaceName, strFontName.c_str());
	}

	// D3DX 폰트 생성
	if (FAILED(D3DXCreateFontIndirect(m_pGraphic_Device, &m_FontDesc, &m_pFont)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CCustomFont::Render(const wstring& strText, const _float2& vPosition, _float3 vColor)
{
	if (nullptr == m_pFont)
		return E_FAIL;

	int centerX = 1280 / 2;
	int centerY = 720 / 2;
	int drawX = static_cast<int>(vPosition.x + centerX);
	int drawY = static_cast<int>(vPosition.y + centerY);

	// 출력 위치 사각형
	RECT rc;

	// 외곽선 색 (검정)
	D3DCOLOR outlineColor = D3DCOLOR_XRGB(0, 0, 0);

	// 8방향 테두리 출력
	for (int dx = -1; dx <= 1; ++dx)
	{
		for (int dy = -1; dy <= 1; ++dy)
		{
			if (dx == 0 && dy == 0)
				continue;

			SetRect(&rc, drawX + dx, drawY + dy, 0, 0);
			m_pFont->DrawTextW(NULL, strText.c_str(), -1, &rc, DT_NOCLIP, outlineColor);
		}
	}

	// 본문 텍스트 색상
	D3DCOLOR fontColor = D3DCOLOR_XRGB(
		static_cast<int>(vColor.x * 255.0f),
		static_cast<int>(vColor.y * 255.0f),
		static_cast<int>(vColor.z * 255.0f));

	// 본문 중앙 텍스트 출력
	SetRect(&rc, drawX, drawY, 0, 0);
	m_pFont->DrawTextW(NULL, strText.c_str(), -1, &rc, DT_NOCLIP, fontColor);

	return S_OK;
}

HRESULT CCustomFont::Render_Size(const wstring& strText, const _float2& vPosition, const _float2& vSize, _float3 vColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	D3DXFONT_DESC fontDesc = m_FontDesc;
	fontDesc.Height = static_cast<int>(vSize.y);
	fontDesc.Width = static_cast<int>(vSize.x);

	LPD3DXFONT pTempFont = nullptr;
	if (FAILED(D3DXCreateFontIndirect(m_pGraphic_Device, &fontDesc, &pTempFont)))
		return E_FAIL;

	int centerX = 1280 / 2;
	int centerY = 720 / 2;
	int drawX = static_cast<int>(vPosition.x + centerX);
	int drawY = static_cast<int>(vPosition.y + centerY);

	RECT rc;
	SetRect(&rc, drawX, drawY, 0, 0);

	// 🔲 외곽선 색
	D3DCOLOR outlineColor = D3DCOLOR_XRGB(0, 0, 0);

	// 🔲 외곽선 출력 (8방향)
	for (int dx = -1; dx <= 1; ++dx)
	{
		for (int dy = -1; dy <= 1; ++dy)
		{
			if (dx == 0 && dy == 0)
				continue;

			RECT rcOutline;
			SetRect(&rcOutline, drawX + dx, drawY + dy, 0, 0);
			pTempFont->DrawTextW(NULL, strText.c_str(), -1, &rcOutline, DT_NOCLIP, outlineColor);
		}
	}

	// 🌈 본문 색
	D3DCOLOR fontColor = D3DCOLOR_XRGB(
		static_cast<int>(vColor.x * 255.0f),
		static_cast<int>(vColor.y * 255.0f),
		static_cast<int>(vColor.z * 255.0f));

	// 🌈 본문 텍스트 출력
	pTempFont->DrawTextW(NULL, strText.c_str(), -1, &rc, DT_NOCLIP, fontColor);

	Safe_Release(pTempFont);

	return S_OK;
}




CCustomFont* CCustomFont::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const wstring& strFontFilePath)
{
	CCustomFont* pInstance = new CCustomFont(pGraphic_Device);
	if (FAILED(pInstance->Initialize(strFontFilePath)))
	{
		MSG_BOX("Failed to Created : CCustomFont");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCustomFont::Free()
{
	Safe_Release(m_pFont);
	Safe_Release(m_pGraphic_Device);
}