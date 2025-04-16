#include "Level_Loading.h"

#include "Loading_UI.h"
#include "Level_GamePlay.h"
#include "Level_Logo.h"
#include "Level_Editor.h"
#include "Level_Hub.h"
#include "Level_Test.h"
#include "Level_Hong.h"
#include "Level_Boss.h"
#include "Level_Ending.h"
#include "Loader.h"
#include "StructureManager.h"
#include "GameInstance.h"
#include <UI_Manager.h>

CLevel_Loading::CLevel_Loading(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel { pGraphic_Device }
{

}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	/* 로딩레벨 자체에 필요한 객체를 생성한다. */
	/* 배경, 로딩바, 버튼, font */

	if (FAILED(Ready_Loading()))
 		return E_FAIL;


	/* 로딩의 역할(다음레벨에 필요한 자원(Resource)(텍스쳐, 모델, 사운드 등등등 )을 생성하는)을 
	수행할 로더객체를 생성한다. */
	m_pLoader = CLoader::Create(m_pGraphic_Device, m_eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;
	m_pGameInstance->Play_Background(L"event:/Backgrounds/004 Entering Level - Waking The Dead").SetVolume(0.50f);
	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	if (m_eNextLevelID == LEVEL_ENDING && true == m_pLoader->isFinished())
	{
		CLevel* pLevel = { nullptr };
		CStructureManager::Get_Instance()->Clear();
		CUI_Manager::GetInstance()->Clear();
		pLevel = CLevel_Ending::Create(m_pGraphic_Device);
		if (nullptr == pLevel)
			return;

		if (FAILED(m_pGameInstance->Process_LevelChange(m_eNextLevelID, pLevel)))
		{
			return;
		}
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (true == m_pLoader->isFinished())
		{
			CLevel* pLevel = { nullptr };
			CStructureManager::Get_Instance()->Clear();
			CUI_Manager::GetInstance()->Clear();
			switch (m_eNextLevelID)
			{
			case LEVEL_LOGO:
				pLevel = CLevel_Logo::Create(m_pGraphic_Device);
				break;
			case LEVEL_GAMEPLAY:
				pLevel = CLevel_GamePlay::Create(m_pGraphic_Device);
				break;
			case LEVEL_EDITOR:
				pLevel = CLevel_Editor::Create(m_pGraphic_Device);
				break;
			case LEVEL_TEST:
				pLevel = CLevel_Test::Create(m_pGraphic_Device);
				break;
			case LEVEL_HUB:
				pLevel = CLevel_Hub::Create(m_pGraphic_Device);
				break;
			case LEVEL_HONG:
				pLevel = CLevel_Hong::Create(m_pGraphic_Device);
				break;
			case LEVEL_BOSS:
				pLevel = CLevel_Boss::Create(m_pGraphic_Device);
				break;
			case LEVEL_ENDING:
				pLevel = CLevel_Ending::Create(m_pGraphic_Device);
				break;
			}

			if (nullptr == pLevel)
				return;
		
			if (FAILED(m_pGameInstance->Process_LevelChange(m_eNextLevelID, pLevel)))
			{
				return;
			}
		}
	}	
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Output_LoadingText();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Loading()
{

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING,
		TEXT("Prototype_Component_Texture_Loading_UI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
			TEXT("../../Resources/Textures/UI/Loading/bg_entering.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING,
		TEXT("Prototype_Component_VIBuffer_Loading_UI"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING,
		TEXT("Prototype_Component_Transform_Loading_UI"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING,
		TEXT("Prototype_GameObject_Loading_UI"),
		CLoading_UI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Prototype_GameObject_Loading_UI"),
		LEVEL_LOADING, TEXT("Layer_Loading_UI"))))
		return E_FAIL;


	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pGraphic_Device);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Loading::Free()
{

	__super::Free();

	Safe_Release(m_pLoader);
}
