#include "UI_Point_Shop.h"
#include "GameInstance.h"

#include "UI_Manager.h"


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

    m_bOnUI = false;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Texture()))
        return E_FAIL;

    if (FAILED(Register_Buttons()))
        return E_FAIL;

    if (FAILED(Ready_Skill_Button_Text()))
        return E_FAIL;

    if (FAILED(Ready_Stat_Button_Text()))
        return E_FAIL;


    m_pTransformCom->Set_Scale(m_Shop_INFO.vSize.x, m_Shop_INFO.vSize.y, 1.f);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION,
        _float3(m_Shop_INFO.vPos.x, m_Shop_INFO.vPos.y, 0.f));

    CUI_Manager::GetInstance()->AddUI(L"Point_Shop_UI", this, UI_TYPE::UI_DYNAMIC);
    return S_OK;
}
HRESULT CUI_Point_Shop::Register_Buttons()
{
    Create_SkillButton();
    Create_StatButton();

    return S_OK;
}
void CUI_Point_Shop::Priority_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

void CUI_Point_Shop::Update(_float fTimeDelta)
{
    m_bIsActive = m_bOnUI;
}

void CUI_Point_Shop::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

}
HRESULT CUI_Point_Shop::Ready_Texture()
{

    if (FAILED(__super::Add_Component(LEVEL_HUB, TEXT("Prototype_Component_Texture_UI_Point_Shop"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Point_Shop::Ready_Skill_Button_Text()
{
    m_str_Skill_MouseOn_Text.resize(12);

    for (int index = 0; index < 12; ++index)
    {
        switch (index)
        {
        case 0:
            m_str_Skill_MouseOn_Text[index] = L"강력한 도끼를 휘두릅니다.\n3타를 적중시키면 마지막 타격이 강화됩니다.";
            break;
        case 1:
            m_str_Skill_MouseOn_Text[index] = L"트레저 헌터는 숨겨진 보물을 더 잘 찾습니다.";
            break;
        case 2:
            m_str_Skill_MouseOn_Text[index] = L"빠른 치료자: 메디킷 회복량 +50%";
            break;
        case 3:
            m_str_Skill_MouseOn_Text[index] = L"마나 회복 속도 증가";
            break;
        case 4:
            m_str_Skill_MouseOn_Text[index] = L"산탄총 전문가: 산탄총 탄약 +2";
            break;
        case 5:
            m_str_Skill_MouseOn_Text[index] = L"연료 및 폭발 아이템 증가";
            break;
        case 6:
            m_str_Skill_MouseOn_Text[index] = L"총기 탄약 획득량 2배 증가 \n 스킬포인트 : 5";
            break;
        case 7:
            m_str_Skill_MouseOn_Text[index] = L"몬스터 통과 가능";
            break;
        case 8:
            m_str_Skill_MouseOn_Text[index] = L"최대 체력 +30, 피해 -20%";
            break;
        case 9:
            m_str_Skill_MouseOn_Text[index] = L"최대 마나 +30";
            break;
        case 10:
            m_str_Skill_MouseOn_Text[index] = L"적 처치 시 마나 회복";
            break;
        case 11:
            m_str_Skill_MouseOn_Text[index] = L"이동 속도 대폭 증가 \n 스킬포인트 : 3";
            break;
        default:
            return E_FAIL;
            break;
        }
    }

    return S_OK;
}

HRESULT CUI_Point_Shop::Ready_Stat_Button_Text()
{
    m_str_Stat_MouseOn_Text.resize(4);

    for (int index = 0; index < 4; ++index)
    {
        switch (index)
        {
        case 0:
            m_str_Stat_MouseOn_Text[index] = L"근력은 근접 무기의 공격력을 \n 1만큼 증가시킵니다";
            break;
        case 1:
            m_str_Stat_MouseOn_Text[index] = L"생명력은 최대체력을\n 10 증가시킵니다";
            break;
        case 2:
            m_str_Stat_MouseOn_Text[index] = L"정신력은.. \n어쩌구 저쩌구 ( 번역필요 ) ";
            break;
        case 3:
            m_str_Stat_MouseOn_Text[index] = L"용량은 모든 무기들의\n 최대 탄약을 증가시킵니다.";
            break;
        default:
            return E_FAIL;
            break;
        }
    }
    return S_OK;
}


HRESULT CUI_Point_Shop::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player")));

    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("근접 마스터"), _float2(0.f, -207.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("탄약 수집광"), _float2(223.f, -207.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("트레저 헌터"), _float2(0.f, -165.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("고스트"), _float2(243.f, -165.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("치료 중독자"), _float2(0.f, -123.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("건강한 신체"), _float2(223.f, -123.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("학도"), _float2(25.f, -81.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("마법 잠재력"), _float2(223.f, -81.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("산탄총 전문가"), _float2(0.f, -39.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("소울 수집가"), _float2(223.f, -39.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("폭탄광"), _float2(15.f, 5.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("스프린터"), _float2(234.f, 5.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));


    m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(m_vPlayerInfo.iStr),
        _float2(-298.f, -137.f), _float2(16.f, 24.f), _float3(1.f, 1.f, 0.f));

    m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(m_vPlayerInfo.iLife),
        _float2(-298.f, -76.f), _float2(16.f, 24.f), _float3(1.f, 1.f, 0.f));

    m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(m_vPlayerInfo.iSprit),
        _float2(-298.f, -15.f), _float2(16.f, 24.f), _float3(1.f, 1.f, 0.f));

    m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(m_vPlayerInfo.iCapacity),
        _float2(-298.f, 46.f), _float2(16.f, 24.f), _float3(1.f, 1.f, 0.f));



    if (pPlayer)
    {
        m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(pPlayer->Get_PlayerInfo().iLevel),
            _float2(-163.f, -220.f), _float2(16.f, 24.f), _float3(1.f, 1.f, 0.f));
    }
    if (pPlayer)
    {
        m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(pPlayer->Get_PlayerInfo().iSkillpoint),
            _float2(-44.f, 58.f), _float2(16.f, 24.f), _float3(1.f, 1.f, 0.f));
    }

  
    if (pPlayer)
    {
        m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(pPlayer->Get_PlayerInfo().iStatpoint),
            _float2(-358.f, -200.f), _float2(16.f, 24.f), _float3(1.f, 1.f, 0.f));
    }

  

    return S_OK;
}
void CUI_Point_Shop::Create_SkillButton() //오른쪽 특성 버튼
{
    vector<CGamePlay_Button::GamePlayer_Button_Desc> vecButtonDescs(12);

    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            int index = j * 6 + i;

            vecButtonDescs[index].Button_Desc.vSize = { 211.f, 32.f };
            vecButtonDescs[index].Button_Desc.vPos = { 221.f * j + 47.f, 197.f - i * 42.f };
            vecButtonDescs[index].strTexture_Default_Tag = L"Prototype_Component_Texture_Button_Point_Shop_Skill"; // 컴포넌트
            vecButtonDescs[index].strUIName = L"Level_Point_Shop_Selected_" + to_wstring(index); // 혹시 몰라서.
            vecButtonDescs[index].bActive = true;
            vecButtonDescs[index].Button_Type = CGamePlay_Button::BUTTON_TYPE_ENUM::POINT_SHOP_SKILL;
            if (FAILED(m_pGameInstance->Add_GameObject(
                LEVEL_HUB,
                TEXT("Prototype_GameObject_GamePlayer_Button"), //  이건 프로토타입 이름이고
                LEVEL_HUB,
                vecButtonDescs[index].strUIName, // 이건 레이어 이름이고
                &vecButtonDescs[index])))
                continue;

            CGamePlay_Button* pButton = static_cast<CGamePlay_Button*>(m_pGameInstance->Find_Object(LEVEL_HUB, vecButtonDescs[index].strUIName.c_str()));

            if (pButton)
            {
                m_vecButtons.push_back(pButton); // 버튼 저장

                pButton->SetOnClickCallback([this,pButton,index]()
                    {
                        if (m_pSelectedSpellButton)
                        {
                           m_pSelectedSpellButton->m_strMouseOnText.clear();
                           m_pSelectedSpellButton->m_strToolTipText.clear();
                        }

                        m_pSelectedSpellButton = pButton;

                   
                        pButton->SetOnClickCallback([this, index]()
                            {
                                CHub_PointShop* pShop = dynamic_cast<CHub_PointShop*>(m_pGameInstance->Find_Object(LEVEL_HUB, TEXT("Layer_Point_Shop")));
                                if (!pShop)
                                    return;

                                _uint iPrice = (index == 6 ? 5 : (index == 11 ? 3 : 0));
                                PurchaseStatus status = pShop->Purchase_Skill(TEXT("Skill_") + to_wstring(index), iPrice);
                                auto pUI_Event = static_cast<CUI_Event*>(CUI_Manager::GetInstance()->GetUI(TEXT("UI_Event")));
                                CUI_Event::EVENT_RENDER_TEXT vRenderText;
                                if (pUI_Event)
                                {
                                    vRenderText.vPos = _float2(-70.f, 0.f);
                                    vRenderText.vFontSize = _float2(10.f, 30.f);
                                    vRenderText.vColor = _float3(1.f, 1.f, 0.1f);
                                    vRenderText.fLifeTime = 1.f;
                               
                                }
                                if (status == PurchaseStatus::NotEnoughPoint)
                                {
                                    vRenderText.vPos = _float2(-100.f, 0.f);
                                    vRenderText.stText = TEXT("스킬 포인트가 부족합니다.");
                                 }
                                else if (status == PurchaseStatus::AlreadyOwned)
                                    vRenderText.stText = TEXT("이미 구매했습니다.");

                                 pUI_Event->Add_EventRender(vRenderText);
                        
                            });
                    });

                pButton->SetOnMouseCallback([this, pButton, index]()
                    {

                       if (index < m_str_Skill_MouseOn_Text.size())
                        {
                            pButton->SetOnMouseCallback([this, pButton, index]()
                                {
                                    pButton->m_strMouseOnText = m_str_Skill_MouseOn_Text[index];
                                }
                            );
                        }

                    });


            }

        }
    }
}

void CUI_Point_Shop::Create_StatButton()
{
    vector<CGamePlay_Button::GamePlayer_Button_Desc> vecButtonDescs(4);

    vecButtonDescs[0].Button_Desc.vPos.y = 130.f;
    vecButtonDescs[1].Button_Desc.vPos.y = 66.f;
    vecButtonDescs[2].Button_Desc.vPos.y = 2.f;
    vecButtonDescs[3].Button_Desc.vPos.y = -58.f;

    for (_int index = 0; index < 4; ++index)
    {
        vecButtonDescs[index].Button_Desc.vSize = { 50.f, 50.f };
        vecButtonDescs[index].Button_Desc.vPos.x = { -349.f };
        vecButtonDescs[index].strTexture_Default_Tag = L"Prototype_Component_Texture_Button_Point_Shop_Stat";
        vecButtonDescs[index].strUIName = L"Level_Point_Shop_Selected_Stat_" + to_wstring(index);
        vecButtonDescs[index].bActive = true;
        vecButtonDescs[index].Button_Type = CGamePlay_Button::BUTTON_TYPE_ENUM::POINT_SHOP_STAT;

        if (FAILED(m_pGameInstance->Add_GameObject(
            LEVEL_HUB,
            TEXT("Prototype_GameObject_GamePlayer_Button"),
            LEVEL_HUB,
            vecButtonDescs[index].strUIName,
            &vecButtonDescs[index])))
            continue;

        CGamePlay_Button* pButton = static_cast<CGamePlay_Button*>(m_pGameInstance->Find_Object(LEVEL_HUB, vecButtonDescs[index].strUIName.c_str()));

        if (pButton)
        {
            m_vecButtons.push_back(pButton);

            pButton->SetOnClickCallback([this, index]()
                {
         
                    CHub_PointShop* pShop = dynamic_cast<CHub_PointShop*>(m_pGameInstance->Find_Object(LEVEL_HUB, TEXT("Layer_Point_Shop")));
                    if (!pShop)
                        return E_FAIL;
                    _wstring stStat;
                    switch (index)
                    {
                    case 0:
                        stStat = TEXT("Strength");
                        break;
                    case 1:
                        stStat = TEXT("Life");
                        break;
                    case 2:
                        stStat = TEXT("Sprit");
                        break;
                    case 3:
                        stStat = TEXT("Capacity");
                        break;

                    }

                    PurchaseStatus status = pShop->Purchase_Stat(stStat, 1);
                    auto pUI_Event = static_cast<CUI_Event*>(CUI_Manager::GetInstance()->GetUI(TEXT("UI_Event")));
                    CUI_Event::EVENT_RENDER_TEXT vRenderText;

                    if (status == PurchaseStatus::Success)
                        vRenderText.stText = stStat+ TEXT(" 구매 완료.");
                    else if (status == PurchaseStatus::NotEnoughPoint)
                        vRenderText.stText = TEXT("스탯 포인트가 부족합니다.");

                    if (pUI_Event)
                    {
                        vRenderText.vPos = _float2(-70.f, 0.f);
                        vRenderText.vFontSize = _float2(10.f, 30.f);
                        vRenderText.vColor = _float3(1.f, 1.f, 0.1f);
                        vRenderText.fLifeTime = 1.f;
                        pUI_Event->Add_EventRender(vRenderText);
                    }

                    return S_OK;
                });


            pButton->SetOnMouseCallback([this, pButton, index]()
                {
                    if (index < m_str_Stat_MouseOn_Text.size())
                    {
                        pButton->m_strMouseOnText = m_str_Stat_MouseOn_Text[index];
                    }
                });
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
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);

}


