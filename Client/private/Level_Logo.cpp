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


	CUI_Base::BackGround_DESC t_Desc_WhiteBackGround{}; 

	t_Desc_WhiteBackGround.BackGround_Desc.vSize = { 2048.f,763.f };
	t_Desc_WhiteBackGround.BackGround_Desc.vPos = { -0.f,-0.f };
	t_Desc_WhiteBackGround.BackGround_Desc.fAlpha = 1.0f;
	t_Desc_WhiteBackGround.strTextureTag = TEXT("Prototype_Component_Texture_BackGround_3");
	///////////////////////////////////////////////////////// 이 밑으로는 안움직이는 이미지들은 0으로 할당!!
	t_Desc_WhiteBackGround.fmoveSpeed = -20.0f;
	t_Desc_WhiteBackGround.fMoveDistance = 2048.0f;// 이미지의 너비
	t_Desc_WhiteBackGround.fNextx = 0.f; // 이미지 갔다가 다시 돌아때의 위치  X
	t_Desc_WhiteBackGround.fNexty = 0.f; // 이미지 갔다가 다시 돌아때의 위치  Y 자세한 로직은 CBackGround.cpp에 있음
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround"),
		LEVEL_LOGO, TEXT("Layer_BackGround_1"), &t_Desc_WhiteBackGround)))
		return E_FAIL;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	CUI_Base::BackGround_DESC tDesc_CanSeeMonster{};
	tDesc_CanSeeMonster.BackGround_Desc.vSize = { 1569.f,600.f };
	tDesc_CanSeeMonster.BackGround_Desc.vPos = { -192.f,-90.f };
	tDesc_CanSeeMonster.BackGround_Desc.fAlpha = 1.0f;
	tDesc_CanSeeMonster.strTextureTag = TEXT("Prototype_Component_Texture_BackGround_4");
	tDesc_CanSeeMonster.fmoveSpeed = -50.0f;
	tDesc_CanSeeMonster.fMoveDistance = 1569.0f;// 이미지의 너비
	tDesc_CanSeeMonster.fNextx = 0.f; // 이미지 갔다가 다시 돌아때의 위치  X
	tDesc_CanSeeMonster.fNexty = 0.f; // 이미지 갔다가 다시 돌아때의 위치  Y 자세한 로직은 CBackGround.cpp에 있음
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround"),
		LEVEL_LOGO, TEXT("Layer_BackGround_2"), &tDesc_CanSeeMonster)))
		return E_FAIL;
	/////////////////////////////////////////////////////////////////////////////////////////////////
	CUI_Base::BackGround_DESC tDesc_Man{}; // 사람
	tDesc_Man.BackGround_Desc.vSize = { 1085.f,720.f };
	tDesc_Man.BackGround_Desc.vPos = { -192.f,-90.f };
	tDesc_Man.BackGround_Desc.fAlpha = 1.0f;
	tDesc_Man.strTextureTag = TEXT("Prototype_Component_Texture_BackGround");
	tDesc_Man.fmoveSpeed = 0.f;
	tDesc_Man.fStack_MoveDistance = 0.f;// 초당 100 단위 이동
	tDesc_Man.fMoveDistance = 0.f;// 이미지의 너비
	tDesc_Man.fNextx = tDesc_Man.BackGround_Desc.vPos.x; // 이미지 갔다가 다시 돌아때의 위치  X
	tDesc_Man.fNexty = 0.f; // 이미지 갔다가 다시 돌아때의 위치  Y 자세한 로직은 CBackGround.cpp에 있음
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround"),
		LEVEL_LOGO, TEXT("Layer_BackGround_3"), &tDesc_Man)))
		return E_FAIL;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CUI_Base::BackGround_DESC tDesc_BlackMonster_Image{};
	tDesc_BlackMonster_Image.BackGround_Desc.vSize = { 2574.f,643.f };
	tDesc_BlackMonster_Image.BackGround_Desc.vPos = { -0.f,-280.f };
	tDesc_BlackMonster_Image.BackGround_Desc.fAlpha = 1.0f;
	tDesc_BlackMonster_Image.strTextureTag = TEXT("Prototype_Component_Texture_BackGround_2");
	///////////////////////////////////////////////////////// 
	tDesc_BlackMonster_Image.fmoveSpeed = 70.0f;
	tDesc_BlackMonster_Image.fMoveDistance = 1287.f;
	tDesc_BlackMonster_Image.fNextx = -607.f;
	tDesc_BlackMonster_Image.fNexty = 0.f;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround"),
		LEVEL_LOGO, TEXT("Layer_BackGround_4"), &tDesc_BlackMonster_Image)))
		return E_FAIL;
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
