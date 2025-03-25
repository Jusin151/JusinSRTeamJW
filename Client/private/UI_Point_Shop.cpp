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

   if (FAILED(Ready_Skill_Button_Text()))
       return E_FAIL;

   if (FAILED(Ready_Stat_Button_Text()))
       return E_FAIL;



    m_pTransformCom->Set_Scale(m_Shop_INFO.vSize.x, m_Shop_INFO.vSize.y, 1.f);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION,
        _float3(m_Shop_INFO.vPos.x, m_Shop_INFO.vPos.y, 0.f));

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
            m_str_Skill_MouseOn_Text[index] = L"총기 탄약 획득량 증가";
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
            m_str_Skill_MouseOn_Text[index] = L"이동 속도 대폭 증가";
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
    m_str_Stat_MouseOn_Text.resize(3);

    for (int index = 0; index < 3; ++index)
    {
        switch (index)
        {
        case 0:
            m_str_Skill_MouseOn_Text[index] = L"근력은 근접 무기의 공격력을 \n 0.25만큼 증가시킵니다";
            break;
        case 1:
            m_str_Skill_MouseOn_Text[index] = L"생명력은 최대체력을\n 10 증가시킵니다";
            break;
        case 2:
            m_str_Skill_MouseOn_Text[index] = L"정신력은.. \n어쩌구 저쩌구 ( 번역필요 ) ";
            break;
        case 3:
            m_str_Skill_MouseOn_Text[index] = L"용량은 모든 무기들의\n 최대 탄약을 증가시킵니다.";
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

     m_pGameInstance->Render_Font_Size(L"MainFont",TEXT("근접 마스터"), _float2(0.f, -207.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
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
     m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("스프린터"), _float2(234.f,5.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
    return S_OK;
}
void CUI_Point_Shop::Create_SkillButton() //오른쪽 특성 버튼
{
    vector<CGamePlay_Button::GamePlayer_Button_Desc> vecButtonDescs(12);

    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            int index = i * 2 + j;
            vecButtonDescs[index].Button_Desc.vSize = { 211.f, 32.f };
            vecButtonDescs[index].Button_Desc.vPos = { 221.f * j + 47.f, 197.f - i * 42.f };
            vecButtonDescs[index].strTexture_Default_Tag = L"Prototype_Component_Texture_Button_Point_Shop_Skill"; // 컴포넌트
            vecButtonDescs[index].strUIName = L"Level_Point_Shop_Selected_" + to_wstring(index); // 혹시 몰라서.
            vecButtonDescs[index].Button_Type = CGamePlay_Button::BUTTON_TYPE_ENUM::POINT_SHOP_SKILL;
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
                        switch (index)
                        {
                        case 0:
                            MessageBox(nullptr, L"도끼 강화 스킬 구매!", L"구매 완료", MB_OK);
                            break;
                        case 1:
                            MessageBox(nullptr, L"보물 탐색 강화!", L"구매 완료", MB_OK);
                            break;
                        case 2:
                            MessageBox(nullptr, L"메디킷 회복량 증가!", L"구매 완료", MB_OK);
                            break;
                        case 3:
                            MessageBox(nullptr, L"마나 회복 속도 증가", L"구매 완료", MB_OK);
                            break;
                        case 4:
                            MessageBox(nullptr, L" 산탄총 탄약 +2", L"구매 완료", MB_OK);
                            break;
                        case 5:
                            MessageBox(nullptr, L"연료 및 폭발 아이템 증가", L"구매 완료", MB_OK);
                            break;
                        case 6:
                            MessageBox(nullptr, L" 총기 탄약 획득량 증가", L"구매 완료", MB_OK);
                            break;
                        case 7:
                            MessageBox(nullptr, L"몬스터 통과 가능", L"구매 완료", MB_OK);
                            break;
                        case 8:
                            MessageBox(nullptr, L"최대 체력 +30, 피해 -20%", L"구매 완료", MB_OK);
                            break;
                        case 9:
                            MessageBox(nullptr, L"최대 마나 +30", L"구매 완료", MB_OK);
                            break;
                        case 10:
                            MessageBox(nullptr, L"적 처치 시 마나 회복", L"구매 완료", MB_OK);
                            break;
                        case 11:
                            MessageBox(nullptr, L"이동 속도 대폭 증가", L"구매 완료", MB_OK);
                            break;                                           
                        default:                       
                            break;
                        }
                    });

                pButton->SetOnMouseCallback([this,pButton, index]()
                    {
                        if (index < m_str_Skill_MouseOn_Text.size())
                        {
                            pButton->SetOnMouseCallback([this,pButton, index]()
                                {
                                pButton->m_strMouseOnText = m_str_Skill_MouseOn_Text[index];
                                });
                        }

                    });


            }

        }
    }
}

void CUI_Point_Shop::Create_StatButton() // 왼쪽 스탯버튼 
{
    vector<CGamePlay_Button::GamePlayer_Button_Desc> vecButtonDescs(4);

    vecButtonDescs[0].Button_Desc.vPos.y = 130.f;
    vecButtonDescs[1].Button_Desc.vPos.y = 66.f;
    vecButtonDescs[2].Button_Desc.vPos.y = 2.f;
    vecButtonDescs[3].Button_Desc.vPos.y = -58.f;

    for (int index = 0; index < 4; ++index)
    {
            vecButtonDescs[index].Button_Desc.vSize = { 50.f, 50.f };
            vecButtonDescs[index].Button_Desc.vPos.x = { -349.f };
            vecButtonDescs[index].strTexture_Default_Tag = L"Prototype_Component_Texture_Button_Point_Shop_Stat"; // 컴포넌트
            vecButtonDescs[index].strUIName = L"Level_Point_Shop_Selected_Stat_" + to_wstring(index); // 혹시 몰라서.
            vecButtonDescs[index].Button_Type = CGamePlay_Button::BUTTON_TYPE_ENUM::POINT_SHOP_STAT;
            if (FAILED(m_pGameInstance->Add_GameObject(
                LEVEL_GAMEPLAY,
                TEXT("Prototype_GameObject_GamePlayer_Button"), //  이건 프로토타입 이름이고
                LEVEL_GAMEPLAY,
                vecButtonDescs[index].strUIName, // 이건 레이어 이름이고
                &vecButtonDescs[index])))
                continue;

            CGamePlay_Button* pButton = static_cast<CGamePlay_Button*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, vecButtonDescs[index].strUIName.c_str()));

            if (pButton)
            {
                pButton->SetOnClickCallback([index]()
                    {
                        switch (index)
                        {
                        case 0:
                            MessageBox(nullptr, L"근력 강화!",nullptr, MB_OK);
                            break;
                        case 1:
                            MessageBox(nullptr, L"생명력 강화!", nullptr, MB_OK);
                            break;
                        case 2:
                            MessageBox(nullptr, L"정신력 강화!", nullptr, MB_OK);
                            break;
                        case 3:
                            MessageBox(nullptr, L"용량 강화!", nullptr, MB_OK);
                            break;                   
                        default:
                            break;
                        }
                    });

                pButton->SetOnMouseCallback([this, pButton, index]()
                    {
                        if (index < m_str_Skill_MouseOn_Text.size())
                        {
                            pButton->SetOnMouseCallback([this, pButton, index]()
                                {
                                    pButton->m_strMouseOnText = m_str_Skill_MouseOn_Text[index];
                                });
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
}

