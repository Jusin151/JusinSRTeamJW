#include "Level_Ending.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "UI_Manager.h"
#include "Sound_Event.h"

CLevel_Ending::CLevel_Ending(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device }
{


}

HRESULT CLevel_Ending::Initialize()
{

	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	m_pGameInstance->Stop_All_Event();
	m_pGameInstance->Play_Background(L"event:/Backgrounds/100 The Warlock Is Dead").SetVolume(0.5f);

	CUI_Manager::GetInstance()->Set_Actived_UI(false);

	return S_OK;
}

void CLevel_Ending::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState('E') & 0x8000)
	{
		if (FAILED(m_pGameInstance->Process_LevelChange(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_LOGO))))
			return;
	}
	fOffSetY += 1.0f;
}

HRESULT CLevel_Ending::Render()
{
	SetWindowText(g_hWnd, TEXT("엔딩레벨입니다."));

	m_pGameInstance->Render_Font_Size(L"MainFont", L"The End"/*+ to_wstring()*/,
		_float2(-56.f, -307.f - fOffSetY), _float2(16.f, 32.f), _float3(1.f, 1.f, 1.f));
	m_pGameInstance->Render_Font_Size(L"EndingFont", L"팀장:\t김장원\n "/*+ to_wstring()*/,
		_float2(-88.f, -207.f - fOffSetY), _float2(16.f, 32.f), _float3(1.f, 1.f, 1.f));
	m_pGameInstance->Render_Font_Size(L"EndingFont", L"김경래\n"/*+ to_wstring()*/,
		_float2(-24.f, -157.f - fOffSetY), _float2(16.f, 32.f), _float3(1.f, 1.f, 1.f));
	m_pGameInstance->Render_Font_Size(L"EndingFont", L"차명훈\n"/*+ to_wstring()*/,
		_float2(-24.f, -107.f - fOffSetY), _float2(16.f, 32.f), _float3(1.f, 1.f, 1.f));
	m_pGameInstance->Render_Font_Size(L"EndingFont", L"홍동완\n"/*+ to_wstring()*/,
		_float2(-24.f, -57.f - fOffSetY), _float2(16.f, 32.f), _float3(1.f, 1.f, 1.f));
	//m_pGameInstance->Render_Font_Size(L"MainFont", L"CHA"/*+ to_wstring()*/,
	//	_float2(-300.f, +107.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 1.f));

	return S_OK;
}
HRESULT CLevel_Ending::Ready_Layer_BackGround()
{
	CUI_Base::BackGround_DESC uiDesc = {};
	uiDesc.BackGround_Desc.vPos = { 0.f, 0.f };
	uiDesc.BackGround_Desc.vSize = { g_iWinSizeX, g_iWinSizeY };
	uiDesc.strTextureTag = L"Prototype_Component_Texture_Ending";
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Prototype_GameObject_BackGround"),
		LEVEL_ENDING, TEXT("Layer_BackGround"), &uiDesc)))
		return E_FAIL;
	return S_OK;
}

CLevel_Ending* CLevel_Ending::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Ending* pInstance = new CLevel_Ending(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Ending");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Ending::Free()
{
	__super::Free();
	m_pGameInstance->Stop_All_Event();
}
