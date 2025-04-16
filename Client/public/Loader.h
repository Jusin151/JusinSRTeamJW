#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
struct CUSTOMVERTEX {
	// 화면 좌표에 직접 그리기
	_float x, y, z, rhw;  // rhw: reciprocal of homogeneous w
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
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

	void Output_LoadingText();


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
	HRESULT Loading_For_Ending();

	


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
	void DrawLoadingBar(IDirect3DDevice9* device, _float progress, const RECT& rc);
	void  CompleteOneTask();

private:

	_int     m_iTotalTaskCount = 0;    // 전체 작업 수
	_int     m_iCompletedTaskCount = 0;    // 완료된 작업 수
	_float   m_fProgress = 0.f;  // 0.0f ~ 1.0f
	ID3DXFont* m_pFont = nullptr;

};

END