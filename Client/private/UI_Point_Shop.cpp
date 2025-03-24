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

   m_strBurron_Text
       =
   {
       L"강력한 도끼를 휘두릅니다.\n3타를 적중시키면 마지막 타격이 강화됩니다.",
       L"트레저 헌터는 숨겨진 보물을 더 잘 찾습니다.",
       L"빠른 치료자: 메디킷 회복량 +50%",
       L"마나 회복 속도 증가",
       L"산탄총 전문가: 산탄총 탄약 +2",
       L"연료 및 폭발 아이템 증가",
       L" 총기 탄약 획득량 증가",
       L"몬스터 통과 가능",
       L"최대 체력 +30, 피해 -20%",
       L"최대 마나 +30",
       L"적 처치 시 마나 회복",
       L"이동 속도 대폭 증가"
   };

    m_pTransformCom->Set_Scale(m_Shop_INFO.vSize.x, m_Shop_INFO.vSize.y, 1.f);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION,
        _float3(m_Shop_INFO.vPos.x, m_Shop_INFO.vPos.y, 0.f));

    return S_OK;
}

void CUI_Point_Shop::Priority_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

void CUI_Point_Shop::Update(_float fTimeDelta)
{
	
} 

void CUI_Point_Shop::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

}
HRESULT CUI_Point_Shop::Ready_Texture()
{

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY,TEXT("Prototype_Component_Texture_UI_Point_Shop"),
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
            //vecButtonDescs[index].strTexture_Default_Tag = L"Prototype_Component_Texture_Button_Point_Shop"; // 컴포넌트
            vecButtonDescs[index].strUIName = L"Level_Point_Shop_Selected_" + to_wstring(index); // 혹시 몰라서.

            if (FAILED(m_pGameInstance->Add_GameObject(
                LEVEL_GAMEPLAY,
                TEXT("Prototype_GameObject_GamePlayer_Button"), //  이건 프로토타입 이름이고
                LEVEL_GAMEPLAY,
                vecButtonDescs[index].strUIName, // 이건 레이어 이름이고
                &vecButtonDescs[index])))
                continue;

            CGamePlay_Button* pButton = static_cast<CGamePlay_Button*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY,vecButtonDescs[index].strUIName.c_str()));

            if (pButton)
            {
                pButton->SetOnClickCallback([index]()
                    {
                        // 클릭은 나주ㅠㅇ에..

                    });

                pButton->SetOnMouseCallback([this,pButton, index]()
                    {
                        if (index < m_strBurron_Text.size())
                        {
                            pButton->SetOnMouseCallback([this,pButton, index]()
                                {
                                pButton->m_strMouseOnText = m_strBurron_Text[index];
                                });
                        }

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

