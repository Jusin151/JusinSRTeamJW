#include "Level_Test.h"
#include "GameInstance.h"
#include "PickingSys.h"
#include "MyImGui.h"
#include "JsonLoader.h"

CLevel_Test::CLevel_Test(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CLevel{pGraphic_Device},
    m_pPickingSys{ CPickingSys::Get_Instance() }
{
    m_pPickingSys->Initialize(g_hWnd, m_pGraphic_Device, m_pGameInstance);
    Safe_AddRef(m_pPickingSys);
}

HRESULT CLevel_Test::Initialize()
{
    CJsonLoader jsonLoader;
    if (FAILED(jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL_TEST.json", LEVEL_TEST)))
        return E_FAIL;
    /*if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
        return E_FAIL;*/
    /*if (FAILED(Ready_Layer_Particle(L"Layer_Cube")))
        return E_FAIL;*/

    return S_OK;
}

void CLevel_Test::Update(_float fTimeDelta)
{
    int a = 10;
}

HRESULT CLevel_Test::Render()
{
    SetWindowText(g_hWnd, TEXT("테스트 레벨입니다."));
    return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_Camera_Free"),
    	LEVEL_TEST, strLayerTag)))
    	return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Particle(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Cube"),
        LEVEL_TEST, strLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Prototype_GameObject_Camera_Free"),
    	LEVEL_TEST, strLayerTag)))
    	return E_FAIL;
    return S_OK;
}

CLevel_Test* CLevel_Test::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CLevel_Test* pInstance = new CLevel_Test(pGraphic_Device);

    if (FAILED(pInstance->Initialize()))
    {
         MSG_BOX("Failed to Created : CLevel_Test");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Test::Free()
{
    __super::Free();

    Safe_Release(m_pPickingSys);
    CPickingSys::Destroy_Instance();
}
