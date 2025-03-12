#include "Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "CUI_Base.h"

CLevel_Logo::CLevel_Logo(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device }
{


}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Menu()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_GAMEPLAY))))
			return;
	}

}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround()
{
	 // 앞에 숫자는 렌더 순서임
	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround_3"),
	//	LEVEL_LOGO, TEXT("01_Layer_BackGround_3"))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround_4"),
	//	LEVEL_LOGO, TEXT("02_Layer_BackGround_4"))))
	//	return E_FAIL;
	CUI_Base::BackGround_DESC tDesc_Man{}; // 사람


	tDesc_Man.BackGround_Desc.vSize = { 1085.f,720.f };
	tDesc_Man.BackGround_Desc.vPos = { -192.f,-90.f };
	tDesc_Man.BackGround_Desc.fAlpha = 1.0f;
	tDesc_Man.strTextureTag = TEXT("Prototype_Component_Texture_BackGround");
	///////////////////////////////////////////////////////// 이 밑으로는 안움직이는 이미지들은 0으로 할당!!
	tDesc_Man.fmoveSpeed = 0.f;
	tDesc_Man.fStack_MoveDistance = 0.f;// 초당 100 단위 이동
	tDesc_Man.fMoveDistance = 0.f;// 이미지의 너비
	tDesc_Man.fNextx = 0.f; // 이미지 갔다가 다시 돌아때의 위치  X
	tDesc_Man.fNexty = 0.f; // 이미지 갔다가 다시 돌아때의 위치  Y 자세한 로직은 CBackGround.cpp에 있음

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround"),
		LEVEL_LOGO, TEXT("Layer_BackGround"), &tDesc_Man)))
		return E_FAIL;


	CUI_Base::BackGround_DESC tDesc_3{}; 


	tDesc_3.BackGround_Desc.vSize = { 2048.f,763.f };
	tDesc_3.BackGround_Desc.vPos = { -0.f,-0.f };
	tDesc_3.BackGround_Desc.fAlpha = 1.0f;
	tDesc_3.strTextureTag = TEXT("Prototype_Component_Texture_BackGround_3");
	///////////////////////////////////////////////////////// 이 밑으로는 안움직이는 이미지들은 0으로 할당!!
	tDesc_3.fmoveSpeed = 20.0f;
	tDesc_3.fMoveDistance = 2048.0f;// 이미지의 너비

	tDesc_3.fNextx = 0.f; // 이미지 갔다가 다시 돌아때의 위치  X
	tDesc_3.fNexty = 0.f; // 이미지 갔다가 다시 돌아때의 위치  Y 자세한 로직은 CBackGround.cpp에 있음

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround"),
		LEVEL_LOGO, TEXT("Layer_BackGround_3"), &tDesc_3)))
		return E_FAIL;
	

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	CUI_Base::BackGround_DESC tDesc_4{};


	tDesc_4.BackGround_Desc.vSize = { 1569.f,600.f };
	tDesc_4.BackGround_Desc.vPos = { -192.f,-90.f };
	tDesc_4.BackGround_Desc.fAlpha = 1.0f;
	tDesc_4.strTextureTag = TEXT("Prototype_Component_Texture_BackGround_4");
	///////////////////////////////////////////////////////// 이 밑으로는 안움직이는 이미지들은 0으로 할당!!
	tDesc_4.fmoveSpeed = 50.0f;
	tDesc_4.fMoveDistance = 1569.0f;// 이미지의 너비
	tDesc_4.fNextx = 0.f; // 이미지 갔다가 다시 돌아때의 위치  X
	tDesc_4.fNexty = 0.f; // 이미지 갔다가 다시 돌아때의 위치  Y 자세한 로직은 CBackGround.cpp에 있음

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround"),
		LEVEL_LOGO, TEXT("Layer_BackGround_4"), &tDesc_4)))
		return E_FAIL;



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	CUI_Base::BackGround_DESC tDesc_2{};

	tDesc_2.BackGround_Desc.vSize = { 3000.f,643.f };
	tDesc_2.BackGround_Desc.vPos = { -300.f,-280.f };
	tDesc_2.BackGround_Desc.fAlpha = 1.0f;
	tDesc_2.strTextureTag = TEXT("Prototype_Component_Texture_BackGround_2");
	///////////////////////////////////////////////////////// 
	tDesc_2.fmoveSpeed = 100.0f;
	tDesc_2.fMoveDistance = 2574.0f;
	tDesc_2.fNextx = -300.f;
	tDesc_2.fNexty = -280.f;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround"),
		LEVEL_LOGO, TEXT("Layer_BackGround_2"), &tDesc_2)))
		return E_FAIL;


	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround_2"),
		LEVEL_LOGO, TEXT("Layer_BackGround_2"))))
		return E_FAIL;*/


	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Menu()
{

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_Menu"),
		LEVEL_LOGO, TEXT("Layer_Menu"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_Menu_StartButton"),
		LEVEL_LOGO, TEXT("Layer_Menu_StartButton"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_Menu_ExitButton"),
		LEVEL_LOGO, TEXT("Layer_Menu_ExitButton"))))
		return E_FAIL;


	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Logo::Free()
{
	__super::Free();

}
