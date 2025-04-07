#include "Level_Hong.h"
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

CLevel_Hong::CLevel_Hong(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device }

{
}
HRESULT CLevel_Hong::Initialize()
{


	CGameObject* pPlayer = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	if (pPlayer)
	{
		CTransform* pTransform = static_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));
		if (nullptr == pTransform)
			return E_FAIL;
		pTransform->Set_State(CTransform::STATE_POSITION, _float3(-4.6f, 0.f, -1.1f));
	}

	CCamera_FirstPerson* pCamera = dynamic_cast<CCamera_FirstPerson*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Camera")));
	if (pCamera)
	{
		pCamera->Set_Yaw(D3DXToRadian(180.f));
	}
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_MiniMap"),
		LEVEL_HONG, TEXT("Layer_MiniMap"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss"),
		LEVEL_HONG, TEXT("Layer_HellBoss"))))
		return E_FAIL;


	return S_OK;
}

void CLevel_Hong::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState('F') & 0x8000)
	{
		if (FAILED(m_pGameInstance->Process_LevelChange(LEVEL_LOADING,
			CLevel_Loading::Create(m_pGraphic_Device, LEVEL_HUB))))
			return;
	}
}

HRESULT CLevel_Hong::Render()
{
	SetWindowText(g_hWnd, TEXT("홍동완 레벨입니다"));

	return S_OK;
}

CLevel_Hong* CLevel_Hong::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Hong* pInstance = new CLevel_Hong(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Hong");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Hong::Free()
{
	__super::Free();
}
