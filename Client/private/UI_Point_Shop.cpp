#include "UI_Point_Shop.h"
#include "GameInstance.h"
#include "GamePlay_Button.h"

CUI_Point_Shop::CUI_Point_Shop(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CUI_Shop_Base(pGraphic_Device)
{
}

CUI_Point_Shop::CUI_Point_Shop(const CUI_Point_Shop& Prototype)
	:CUI_Shop_Base(Prototype)
{
}

HRESULT CUI_Point_Shop::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Point_Shop::Initialize(void* pArg)
{

    if (pArg != nullptr)
        m_Shop_INFO = *reinterpret_cast<UI_Desc*>(pArg);
    else
        return E_FAIL;


    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Texture()))
        return E_FAIL;

    if (FAILED(Register_Buttons()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(m_Shop_INFO.vSize.x, m_Shop_INFO.vSize.y, 1.f);


    m_pTransformCom->Set_State(CTransform::STATE_POSITION,
        _float3(m_Shop_INFO.vPos.x, m_Shop_INFO.vPos.y, 0.f));

    return S_OK;
}

void CUI_Point_Shop::Priority_Update(_float fTimeDelta)
{
}

void CUI_Point_Shop::Update(_float fTimeDelta)
{
	isPick(g_hWnd); //이걸로 피킹체크
} 

void CUI_Point_Shop::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

}
HRESULT CUI_Point_Shop::Ready_Texture()
{

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Point_Shop"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

	return S_OK;
}

HRESULT CUI_Point_Shop::Render()
{
    return __super::Render();
}

HRESULT CUI_Point_Shop::Register_Buttons()
{
    CreatePointShopButtons();
    return S_OK;
}

void CUI_Point_Shop::CreatePointShopButtons()
{
    vector<CGamePlay_Button::GamePlayer_Button_Desc> vecButtonDescs(12);

    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            int index = i * 2 + j;
            vecButtonDescs[index].Button_Desc.vSize = { 211.f, 32.f };
            vecButtonDescs[index].Button_Desc.vPos = { 221.f * j + 47.f, 197.f - i * 42.f };
            vecButtonDescs[index].strTexture_Default_Tag = L"Prototype_Component_Texture_Point_Shop_UI";
            vecButtonDescs[index].strUIName = L"Level_Point_Shop_Selected_" + to_wstring(index);

            if (FAILED(m_pGameInstance->Add_GameObject(
                LEVEL_GAMEPLAY,
                TEXT("Prototype_GameObject_GamePlayer_Button"),
                LEVEL_LOGO,
                TEXT("Layer_Point_Shop_Selected"),
                &vecButtonDescs[index])))
                continue;

            CGamePlay_Button* pButton = static_cast<CGamePlay_Button*>(
                m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, vecButtonDescs[index].strUIName.c_str()));

            if (pButton)
            {
                pButton->SetOnClickCallback([index]()
                    {
                    OutputDebugString((L"[PointShop] Button Clicked: " + to_wstring(index) + L"\n").c_str());
                    //  아이템 설명 표시, 플레이어 스탯 업데이트 등
                    });
            }
        }
    }
}

CUI_Point_Shop* CUI_Point_Shop::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CUI_Point_Shop* pInstance = new CUI_Point_Shop(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CUI_Point_Shop");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Point_Shop::Clone(void* pArg)
{
    CUI_Point_Shop* pInstance = new CUI_Point_Shop(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CUI_Point_Shop");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Point_Shop::Free()
{
    __super::Free();
}

//// 포인트샵 메인 디스플레이 버튼 등록    << 이거는 직접 이 PointShop에서 띄울예정
  //CGamePlay_Button::GamePlayer_Button_Desc shopDisplayDesc{};
  //shopDisplayDesc.Button_Desc.vSize = { 804.f, 482.f };
  //shopDisplayDesc.Button_Desc.vPos = { 0.f, 0.f };
  //shopDisplayDesc.strTexture_Default_Tag = L"Prototype_Component_Texture_Point_Shop_UI";
  //shopDisplayDesc.strUIName = L"Level_Point_Shop_Display";
  //shopDisplayDesc.Button_type = Button_type::Point_Shop;

  //if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY,
  //    TEXT("Prototype_GameObject_GamePlayer_Button"),
  //    LEVEL_LOGO, TEXT("Layer_Point_Shop_Display"),
  //    &shopDisplayDesc)))
  //{
  //    return E_FAIL;
  //}

  //  일반 버튼 12개 등록
  //vector<CGamePlay_Button::GamePlayer_Button_Desc> vecButtonDescs(12);

  //// 수직 위치 설정
  //vecButtonDescs[0].Button_Desc.vPos.y = 197.f;
  //vecButtonDescs[1].Button_Desc.vPos.y = 197.f;
  //vecButtonDescs[2].Button_Desc.vPos.y = 155.f;
  //vecButtonDescs[3].Button_Desc.vPos.y = 155.f;
  //vecButtonDescs[4].Button_Desc.vPos.y = 113.f;
  //vecButtonDescs[5].Button_Desc.vPos.y = 113.f;
  //vecButtonDescs[6].Button_Desc.vPos.y = 70.f;
  //vecButtonDescs[7].Button_Desc.vPos.y = 70.f;
  //vecButtonDescs[8].Button_Desc.vPos.y = 28.f;
  //vecButtonDescs[9].Button_Desc.vPos.y = 28.f;
  //vecButtonDescs[10].Button_Desc.vPos.y = -16.f;
  //vecButtonDescs[11].Button_Desc.vPos.y = -16.f;

  //for (int i = 0; i < 6; ++i)
  //{
  //    for (int j = 0; j < 2; ++j)
  //    {
  //        int index = i * 2 + j;
  //        vecButtonDescs[index].Button_Desc.vSize = { 211.f, 32.f };
  //        vecButtonDescs[index].Button_Desc.vPos.x = 221.f * j + 47.f; // 위치 조정
  //        vecButtonDescs[index].strTexture_Default_Tag = L"Prototype_Component_Texture_Point_Shop_UI";
  //        vecButtonDescs[index].strUIName = L"Level_Point_Shop_Selected_" + std::to_wstring(index);
  //        vecButtonDescs[index].Button_type = Button_type::Point_Shop;
  //        vecButtonDescs[index].Point_Shop_Num = index;
  //        vecButtonDescs[index].Point_Shop_Seleted[0] = true;

  //        if (FAILED(m_pGameInstance->Add_GameObject(
  //            LEVEL_GAMEPLAY,
  //            TEXT("Prototype_GameObject_GamePlayer_Button"),
  //            LEVEL_LOGO,
  //            TEXT("Layer_Point_Shop_Selected"),
  //            &vecButtonDescs[index])))
  //        {
  //            return E_FAIL;
  //        }
  //    }
  //}

  //// 작은 버튼 4개 등록
  //vector<CGamePlay_Button::GamePlayer_Button_Desc> vecSmallButtonDescs(4);

  //vecSmallButtonDescs[0].Point_Shop_Seleted[12] = true;
  //vecSmallButtonDescs[1].Point_Shop_Seleted[13] = true;
  //vecSmallButtonDescs[2].Point_Shop_Seleted[14] = true;
  //vecSmallButtonDescs[3].Point_Shop_Seleted[15] = true;

  //vecSmallButtonDescs[0].Button_Desc.vPos.y = 130.f;
  //vecSmallButtonDescs[1].Button_Desc.vPos.y = 66.f;
  //vecSmallButtonDescs[2].Button_Desc.vPos.y = 2.f;
  //vecSmallButtonDescs[3].Button_Desc.vPos.y = -58.f;

  //for (int i = 0; i < 4; ++i)
  //{
  //    vecSmallButtonDescs[i].Button_Desc.vSize = { 50.f, 50.f };
  //    vecSmallButtonDescs[i].Button_Desc.vPos.x = -349.f; // 위치 조정
  //    vecSmallButtonDescs[i].strTexture_Default_Tag = L"Prototype_Component_Texture_Point_Shop_UI";
  //    vecSmallButtonDescs[i].strUIName = L"Level_Point_Shop_Small_Button" + std::to_wstring(i);
  //    vecSmallButtonDescs[i].Button_type = Button_type::Point_Shop;
  //    vecSmallButtonDescs[i].Point_Shop_Num = 12 + i;

  //    wstring layerName = L"Layer_Point_Shop_Selected_" + to_wstring(i);

  //    if (FAILED(m_pGameInstance->Add_GameObject(
  //        LEVEL_GAMEPLAY,
  //        TEXT("Prototype_GameObject_GamePlayer_Button"),
  //        LEVEL_LOGO,
  //        layerName.c_str(),
  //        &vecSmallButtonDescs[i])))
  //    {
  //        return E_FAIL;
  //    }
  //}