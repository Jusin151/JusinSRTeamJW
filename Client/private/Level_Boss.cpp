#include "Level_Boss.h"
#include "GameInstance.h"
#include "PickingSys.h"
#include "Collider_Sphere.h"
#include "UI_Default_Panel.h"
#include "UI_Base.h"
#include "GamePlay_Button.h"
#include "JsonLoader.h"
#include "Weapon_Base.h"
#include "Effect_Base.h"
#include "UI_Shop_Base.h"
#include "UI_Point_Shop.h"
#include "Image.h"
#include "Sound_Event.h"
#include "Level_Loading.h"
#include <Camera_FirstPerson.h>

CLevel_Boss::CLevel_Boss(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device }

{
}
HRESULT CLevel_Boss::Initialize()
{

	CJsonLoader jsonLoader;
 	jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL_AntarcticBoss.json", LEVEL_GAMEPLAY);
	m_pGameInstance->Stop_All_Event();


	CGameObject* pPlayer = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	if (pPlayer)
	{
		CTransform* pTransform = static_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));
		if (nullptr == pTransform)
			return E_FAIL;
		pTransform->Set_State(CTransform::STATE_POSITION, _float3(-14.5f, 0.f, -2.f));
	}

	CCamera_FirstPerson* pCamera = dynamic_cast<CCamera_FirstPerson*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Camera")));
	if (pCamera)
	{
		pCamera->Set_Yaw(D3DXToRadian(90.f));
	}
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_MiniMap"),
		LEVEL_BOSS, TEXT("Layer_MiniMap"))))
		return E_FAIL;

	m_pGameInstance->Play_Background(L"event:/Backgrounds/030 Antarctic - The End").SetVolume(0.25f);


	return S_OK;
}

void CLevel_Boss::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState('F') & 0x8000)
	{

		if (FAILED(m_pGameInstance->Process_LevelChange(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_HUB))))
			return;
	}
}

HRESULT CLevel_Boss::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Monster(const _wstring& strLayerTag)
{


	return S_OK;
}

CLevel_Boss* CLevel_Boss::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Boss* pInstance = new CLevel_Boss(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Boss::Free()
{
	__super::Free();
}
