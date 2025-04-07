#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
struct CUSTOMVERTEX {
	FLOAT x, y, z;    // 화면 좌표 (z는 0으로)
	DWORD color;      // ARGB 컬러
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)
class CLoader final : public CBase
{
private:
	CLoader(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLoader() = default;

public:
	_bool isFinished() const {
		return m_isFinished;
	}

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading();

	void Output_LoadingText() {
		// 윈도우 타이틀에도 진행률 표시
		SetWindowText(g_hWnd, m_szLoadingText);

		RECT barRect = { 80, 640, 1200, 690 };
		DrawLoadingBar(m_pGraphic_Device, m_fProgress, barRect);

		RECT textRect = { 80, 600, 1200, 630 };
		m_pFont->DrawTextW(
			nullptr,
			m_szLoadingText,
			-1,
			&textRect,
			DT_LEFT | DT_NOCLIP,
			D3DCOLOR_ARGB(255, 255, 255, 255)
		);
	}



private:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = { nullptr };
	LEVEL						m_eNextLevelID = { LEVEL_END };
	_bool						m_isFinished = { false };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	HANDLE						m_hThread = {};
	CRITICAL_SECTION			m_CriticalSection = {};
	_tchar						m_szLoadingText[MAX_PATH] = {};

public:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_GamePlay();
	HRESULT Loading_For_Editor();
	HRESULT Loading_For_Test();
	HRESULT Loading_For_Hub();
	HRESULT Loading_For_Hong();
	HRESULT Loading_For_Boss();

	void DrawLoadingBar(IDirect3DDevice9* device, float progress, const RECT& rc);


public:
	static CLoader* Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevelID);
	virtual void Free() override;

//////////////////////////////////////////////////////////////
	HRESULT Add_To_Logo_Textures();
	HRESULT Add_To_Logo_Prototype();

	HRESULT Add_To_Logo_Menu_Textures();
	HRESULT Add_To_Logo_Menu_Prototype();
////////////////////////////////////////////////////////////
	HRESULT Add_To_GamePlay_Prototype();
		

private:

	int     m_iTotalTaskCount = 0;    // 전체 작업 수
	int     m_iCompletedTaskCount = 0;    // 완료된 작업 수
	float   m_fProgress = 0.f;  // 0.0f ~ 1.0f
	ID3DXFont* m_pFont = nullptr;
	void    CompleteOneTask();
};

END