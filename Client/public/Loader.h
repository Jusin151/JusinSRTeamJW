#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

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
		SetWindowText(g_hWnd, m_szLoadingText);
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
	


public:
	static CLoader* Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevelID);
	virtual void Free() override;

// 생성 순서는 텍스쳐 -> 버퍼 -> 트랜스폼 -> 프로토타입순으로 해주는게 좋음
	
////////////////////////////////////////////////////////////
//                      로고 레벨                         //
//////////////////////////////////////////////////////////// 
	HRESULT Add_To_Logo_Textures(); // 로고레벨의 텍스쳐 생성
	HRESULT Add_To_Logo_Buffer();   // 로고레벨의 버퍼 생성
	HRESULT Add_To_Logo_Transform();// 로고레벨의 트랜스폼 생성
	HRESULT Add_To_Logo_Prototype();// 로고레벨의 원형객체 생성
//////////////////////////////////////////////////////////// 
			HRESULT Add_To_Logo_UI_Textures(); // 로고레벨의 UI 텍스쳐들 생성

				   HRESULT Add_To_Logo_Menu_Textures(); // 로고레벨의 메뉴 텍스쳐 생성
				   HRESULT Add_To_Logo_Art_Textures();  // 로고레벨의 일러스트 텍스쳐 생성
//////////////////////////////////////////////////////////// 
		     HRESULT Add_To_Logo_UI_Buffer(); // 로고레벨의 UI 버퍼들 생성

				   HRESULT Add_To_Logo_Menu_Buffer(); // 로고레벨의 메뉴 버퍼 생성
				   HRESULT Add_To_Logo_Art_Buffer();  // 로고레벨의 일러스트 버퍼 생성
//////////////////////////////////////////////////////////// 
		    HRESULT Add_To_Logo_UI_Transform(); // 로고레벨의 UI 트랜스폼들 생성

	               HRESULT Add_To_Logo_Menu_Transform(); // 로고레벨의 메뉴 트랜스폼 생성
				   HRESULT Add_To_Logo_Art_Transform();  // 로고레벨의 일러스트 트랜스폼 생성
//////////////////////////////////////////////////////////// 	
		     HRESULT Add_To_Logo_UI_Prototype(); // 로고레벨의 UI 원본들 생성

	               HRESULT Add_To_Logo_Menu_Prototype(); // 로고레벨의 메뉴 원본 생성
				   HRESULT Add_To_Logo_Art_Prototype();  // 로고레벨의 일러스트 원본 생성

////////////////////////////////////////////////////////////
//                   게임 플레이 레벨                     //
//////////////////////////////////////////////////////////// 
	HRESULT Add_To_GamePlay_Textures(); // 게임플레이 레벨의 텍스쳐들 생성
	HRESULT Add_To_GamePlay_Buffer();   // 게임플레이 레벨의 버퍼들 생성
	HRESULT Add_To_GamePlay_Transform();// 게임플레이 레벨의 트랜스폼들 생성
	HRESULT Add_To_GamePlay_Prototype();// 게임플레이 레벨의 원형객체들 생성
//////////////////////////////////////////////////////////// 
	        HRESULT Add_To_UI_Textures(); // UI의 텍스쳐들 생성

	              HRESULT Add_To_UI_Left_DisPlay_Textures(); // UI의 왼쪽 디스플레이 텍스쳐 생성
				  HRESULT Add_To_UI_Mid_DisPlay_Textures();  // UI의 중간 디스플레이 텍스쳐 생성
				  HRESULT Add_To_UI_Right_DisPlay_Textures();// UI의 오른쪽 디스플레이 텍스쳐 생성
 //////////////////////////////////////////////////////////// 
	        HRESULT Add_To_UI_Buffer(); // UI의 버퍼들 생성
 
			      HRESULT Add_To_UI_Left_DisPlay_Buffer(); // UI의 왼쪽 디스플레이 버퍼 생성
				  HRESULT Add_To_UI_Mid_DisPlay_Buffer();  // UI의 중간 디스플레이 버퍼 생성
				  HRESULT Add_To_UI_Right_DisPlay_Buffer();// UI의 오른쪽 디스플레이 버퍼 생성
//////////////////////////////////////////////////////////// 
	        HRESULT Add_To_UI_Transform(); // UI의 트랜스폼들 생성

				  HRESULT Add_To_UI_Left_DisPlay_Transform(); // UI의 왼쪽 디스플레이 트랜스폼 생성
				  HRESULT Add_To_UI_Mid_DisPlay_Transform();  // UI의 중간 디스플레이 트랜스폼 생성
				  HRESULT Add_To_UI_Right_DisPlay_Transform();// UI의 오른쪽 디스플레이 트랜스폼 생성
//////////////////////////////////////////////////////////// 
		     HRESULT Add_To_UI_Prototype(); // UI의 원형객체들 생성

				  HRESULT Add_To_UI_Left_DisPlay_Prototype();  // UI의 왼쪽 디스플레이 원본생성
				  HRESULT Add_To_UI_Mid_DisPlay_Prototype();   // UI의 중간 디스플레이 원본생성
				  HRESULT Add_To_UI_Right_DisPlay_Prototype(); // UI의 오른쪽 디스플레이 원본생성
/////////////////////////////////////////////////////////////


};

END