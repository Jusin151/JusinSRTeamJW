#include "UI_Spell_Shop.h"
#include "GameInstance.h"
#include "UI_Manager.h"




CUI_Spell_Shop::CUI_Spell_Shop(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CUI_Shop_Base(pGraphic_Device)
{
}

CUI_Spell_Shop::CUI_Spell_Shop(const CUI_Spell_Shop& Prototype)
    :CUI_Shop_Base(Prototype)
{
}

HRESULT CUI_Spell_Shop::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Spell_Shop::Initialize(void* pArg)
{

    if (pArg != nullptr)
        m_Shop_INFO = *reinterpret_cast<UI_Desc*>(pArg);
    else
        return E_FAIL;

    m_bOnUI = false;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Texture()))
        return E_FAIL;

    if (FAILED(Register_Buttons()))
        return E_FAIL;

    if(FAILED(Ready_Spell_Button_Text()))
    return E_FAIL;
  
    m_pTransformCom->Set_Scale(m_Shop_INFO.vSize.x, m_Shop_INFO.vSize.y, 1.f);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION,
        _float3(m_Shop_INFO.vPos.x, m_Shop_INFO.vPos.y, 0.f));

    CUI_Manager::GetInstance()->AddUI(L"Spell_Shop_UI", this);




    return S_OK;
}
HRESULT CUI_Spell_Shop::Register_Buttons()
{
    Create_SkillButton();

    return S_OK;
}
void CUI_Spell_Shop::Priority_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

void CUI_Spell_Shop::Update(_float fTimeDelta)
{
    m_bIsActive = m_bOnUI;
}

void CUI_Spell_Shop::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

}
HRESULT CUI_Spell_Shop::Ready_Texture()
{

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Spell_Shop_DisPlay"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Spell_Shop::Ready_Spell_Button_Text()
{
    m_str_Spell_Text.resize(8);
    m_str_ToolTip_Text.resize(8);
    for (int index = 0; index < 8; ++index)
    {
        switch (index)
        {
        case 0:
            m_str_Spell_Text[index] = L"근처의 적을 때리는 번개구를 소환합니다";
            m_str_ToolTip_Text[index] = L"사용 비용 : 28 마나\n평행 볼트 : 2 \n히트 당 데미지 :8\n 지속 시간 : 12초";
            break;
        case 1:
            m_str_Spell_Text[index] = L"들어오는 피해를 흡수하는 방패를 시전합니다. ";
            m_str_ToolTip_Text[index] = L"사용 비용 : 19 마나\n피해 흡수 : 44 ";
            break;
        case 2:
            m_str_Spell_Text[index] = L"적을 얼리는 마법의 힘을 사용합니다";
            m_str_ToolTip_Text[index] = L"사용 비용 : 17 마나\n유형 : 웨이브 \n빙결 시간 :4.5초 \n 지속 시간 : 7.5초";
            break;
        case 3:
            m_str_Spell_Text[index] = L"주변을 환하게 비추는 주문을 시전합니다";
            m_str_ToolTip_Text[index] = L"공짜";
            break;
        case 4:
            m_str_Spell_Text[index] = L"다이너마이트를 강화하여  던집니다.";
            m_str_ToolTip_Text[index] = L"사용 비용 : 7 마나\n유형 : 다이너마이트";
            break;
        case 5:
            m_str_Spell_Text[index] = L"추가대기";
            m_str_ToolTip_Text[index] = L"사용 비용 : 28.0 마나\n평행 볼트 : 2 \n히트 당 데미지 :8\n 지속 시간 : 12초";
            break;
        case 6:
            m_str_Spell_Text[index] = L"마나를 사용하여 장착한 무기의 탄약을 생성합니다.";
            m_str_ToolTip_Text[index] = L"사용 비용 : 20 마나";
            break;
        case 7:
            m_str_Spell_Text[index] = L"마법의 도끼를 소환하여 강하게 내려칩니다.";
            m_str_ToolTip_Text[index] = L"사용 비용 : 20 마나\n 피해 : 62 ";
            break;
        default:
            return E_FAIL;
            break;
        }
    }

    return S_OK;
}


HRESULT CUI_Spell_Shop::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;


    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("폭풍의 분노"), _float2(-340.f, -195.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("거룩한 방패"), _float2(-150.f, -195.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("빙결 폭발"), _float2(50.f, -195.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("라이트"), _float2(250.f, -195.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));


    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("다이너마이트!"), _float2(-350.f, -120.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("비어있음"), _float2(-150.f, -120.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("마탄 제조"), _float2(50.f, -120.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("집행자"), _float2(250.f, -120.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));


    return S_OK;
}
void CUI_Spell_Shop::Create_SkillButton() //오른쪽 특성 버튼
{ 
        vector<CGamePlay_Button::GamePlayer_Button_Desc> vecButtonDescs(8); // 총 8개

        for (int i = 0; i < 2; ++i) // 세로 2줄
        {
            for (int j = 0; j < 4; ++j) // 가로 4개
            {
                int index = i * 4 + j;

                vecButtonDescs[index].Button_Desc.vSize = { 183.f, 69.f };

                vecButtonDescs[index].Button_Desc.vPos = { -290.f + j * 192.f, 186.f - i * 80.f };

                vecButtonDescs[index].strTexture_Default_Tag = L"Prototype_Component_Texture_Spell_Shop_Selected";

                vecButtonDescs[index].strUIName = L"Level_Spell_Shop_Selected_" + to_wstring(index);

                vecButtonDescs[index].bActive = true;

                vecButtonDescs[index].Button_Type = CGamePlay_Button::BUTTON_TYPE_ENUM::SPELL_SHOP_BUTTON;

                if (FAILED(m_pGameInstance->Add_GameObject(
                    LEVEL_GAMEPLAY,
                    TEXT("Prototype_GameObject_GamePlayer_Button"),
                    LEVEL_GAMEPLAY,
                    vecButtonDescs[index].strUIName,
                    &vecButtonDescs[index])))
                    continue;

                CGamePlay_Button* pButton = static_cast<CGamePlay_Button*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, vecButtonDescs[index].strUIName.c_str()));

            if (pButton)
            {
                m_vecButtons.push_back(pButton); // 버튼 저장
                pButton->SetOnClickCallback([this, pButton, index]()
                    {
                        if (m_pSelectedSpellButton)
                        {
                            m_pSelectedSpellButton->m_strMouseOnText.clear();
                            m_pSelectedSpellButton->m_strToolTipText.clear();
                        }

                        m_pSelectedSpellButton = pButton;

                        switch (index)
                        {
                        case 0:
                           // MessageBox(nullptr, L"첫번째 스펠샵 !!", L"구매 완료", MB_OK);
                            pButton->m_strMouseOnText = m_str_Spell_Text[index];
                            pButton->m_strToolTipText = m_str_ToolTip_Text[index];
                            break;
                        case 1:
                           // MessageBox(nullptr, L"두번째 스펠샵 !!", L"구매 완료", MB_OK);
                            pButton->m_strMouseOnText = m_str_Spell_Text[index];
                            pButton->m_strToolTipText = m_str_ToolTip_Text[index];
                            break;
                        case 2:
                           // MessageBox(nullptr, L"세번째 스펠샵 !!", L"구매 완료", MB_OK);
                            pButton->m_strMouseOnText = m_str_Spell_Text[index];
                            pButton->m_strToolTipText = m_str_ToolTip_Text[index];
                            break;
                        case 3:
                            //MessageBox(nullptr, L"네번째 스펠샵 !!", L"구매 완료", MB_OK);
                            pButton->m_strMouseOnText = m_str_Spell_Text[index];
                            pButton->m_strToolTipText = m_str_ToolTip_Text[index];
                            break;
                        case 4:
                           // MessageBox(nullptr, L"다섯번째 스펠샵 !!", L"구매 완료",MB_OK);
                            pButton->m_strMouseOnText = m_str_Spell_Text[index];
                            pButton->m_strToolTipText = m_str_ToolTip_Text[index];
                            break;
                        case 5:
                           // MessageBox(nullptr, L"여섯번째 스펠샵 !!", L"빈 공간", MB_OK);
                            pButton->m_strMouseOnText = m_str_Spell_Text[index];
                            pButton->m_strToolTipText = m_str_ToolTip_Text[index];
                            break;
                        case 6:
                            //MessageBox(nullptr, L"일곱번째 스펠샵 !!", L"빈 공간!", MB_OK);
                            pButton->m_strMouseOnText = m_str_Spell_Text[index];
                            pButton->m_strToolTipText = m_str_ToolTip_Text[index];
                            break;
                        case 7:
                            //MessageBox(nullptr, L"여덟번째 스펠샵 !!", L"빈 공간", MB_OK);
                            pButton->m_strMouseOnText = m_str_Spell_Text[index];
                            pButton->m_strToolTipText = m_str_ToolTip_Text[index];
                            break;
                        default:
                            break;
                        }
                    });

                pButton->SetOnMouseCallback([this, pButton, index]()
                    {
                        //if (index < m_str_Spell_Text.size())
                        //{
                        //    //pButton->m_strMouseOnText = m_str_Spell_Text[index];
                        //    //pButton->m_strToolTipText = m_str_ToolTip_Text[index];
                        //}
                    });
            }

        }
    }
}


CUI_Spell_Shop* CUI_Spell_Shop::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CUI_Spell_Shop* pInstance = new CUI_Spell_Shop(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CUI_Spell_Shop");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Spell_Shop::Clone(void* pArg)
{
    CUI_Spell_Shop* pInstance = new CUI_Spell_Shop(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CUI_Spell_Shop");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Spell_Shop::Free()
{
    __super::Free();
}

