#include "UI_WeaponShop_UI.h"
#include "GameInstance.h"
#include "GamePlay_Button.h"
#include "UI_Manager.h"



CUI_WeaponShop_UI::CUI_WeaponShop_UI(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CUI_Shop_Base(pGraphic_Device)
{
}

CUI_WeaponShop_UI::CUI_WeaponShop_UI(const CUI_WeaponShop_UI& Prototype)
    :CUI_Shop_Base(Prototype)
{
}

HRESULT CUI_WeaponShop_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_WeaponShop_UI::Initialize(void* pArg)
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

    if (FAILED(Ready_01_Page_Weapon_Button_Text())) // 첫번째 페이지 
        return E_FAIL;

    if (FAILED(Ready_02_Page_Weapon_Button_Text())) // 두번째 페이지 
        return E_FAIL;

    if (FAILED(Ready_03_Page_Weapon_Button_Text())) // 세번째 페이지 
        return E_FAIL;

    if (FAILED(Ready_04_Page_Weapon_Button_Text())) // 네번째 페이지 
        return E_FAIL;

    m_pTransformCom->Set_Scale(m_Shop_INFO.vSize.x, m_Shop_INFO.vSize.y, 1.f);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION,
        _float3(m_Shop_INFO.vPos.x, m_Shop_INFO.vPos.y, 0.f));

    CUI_Manager::GetInstance()->AddUI(L"Weapon_Shop_UI", this);

    return S_OK;
}
HRESULT CUI_WeaponShop_UI::Register_Buttons()
{
    Create_SkillButton();

    return S_OK;
}

void CUI_WeaponShop_UI::Priority_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

void CUI_WeaponShop_UI::Update(_float fTimeDelta)
{
    m_bIsActive = m_bOnUI;
}

void CUI_WeaponShop_UI::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

}
HRESULT CUI_WeaponShop_UI::Ready_Texture()
{

    if (FAILED(__super::Add_Component(LEVEL_HUB, TEXT("Prototype_Component_Texture_Weapon_Shop_Display"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_WeaponShop_UI::Ready_01_Page_Weapon_Button_Text()
{
    m_str_Weapon_MouseOn_Text.resize(8);

    for (int index = 0; index < 7; ++index)
    {
        switch (index)
        {
        case 0:
            m_str_Weapon_MouseOn_Text[index] = L"이 업그레이드는 근접 범위를\n 1미터 증가시킵니다.\n적중시 HP를 회복합니다.";
            break;
        case 1:
            m_str_Weapon_MouseOn_Text[index] = L"이 업그레이드는 근접 피해를 \n 증가시킵니다.\n 적중시 마나를 회복합니다";
            break;
        case 2:
            m_str_Weapon_MouseOn_Text[index] = L" 번개 지팡이로 진화합니다\n 더 오래 차징시 더 강한 공격!";
            break;
        case 3:
            m_str_Weapon_MouseOn_Text[index] = L"빙결 지팡이로 진화합니다\n 긴 쿨다운이지만 강한 공격!";
            break;
        case 4:
            m_str_Weapon_MouseOn_Text[index] = L"이 업그레이드는 권총을 \n치명적인 .50 CAL 매그넘으로 \n바꾸어 더 많은 탄약을 소비하는\n 대신 강한 한방을 지니게 됩니다";
            break;
        case 5:
            m_str_Weapon_MouseOn_Text[index] = L"추가대기";
            break;
        case 6:
            m_str_Weapon_MouseOn_Text[index] = L"추가대기";
            break;
        case 7:
            m_str_Weapon_MouseOn_Text[index] = L"추가대기";
            break;
        default:
            return E_FAIL;
            break;
        }
    }

    return S_OK;
}

HRESULT CUI_WeaponShop_UI::Ready_02_Page_Weapon_Button_Text()
{
    return S_OK;
}

HRESULT CUI_WeaponShop_UI::Ready_03_Page_Weapon_Button_Text()
{
    return S_OK;
}

HRESULT CUI_WeaponShop_UI::Ready_04_Page_Weapon_Button_Text()
{
    return S_OK;
}


HRESULT CUI_WeaponShop_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;


    return S_OK;
}
void CUI_WeaponShop_UI::Create_SkillButton() //오른쪽 특성 버튼
{
    vector<CGamePlay_Button::GamePlayer_Button_Desc> vecButtonDescs(12);

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            int index = i * 2 + j;
            vecButtonDescs[index].Button_Desc.vSize = { 240.f, 101.f };
            vecButtonDescs[index].Button_Desc.vPos = { -245.f * (1 - j) + -20.f, 160.f - i * 107.f };

            vecButtonDescs[index].strTexture_Default_Tag = L"Prototype_Component_Texture_Weapon_Shop_Selected"; // 컴포넌트
            vecButtonDescs[index].strUIName = L"Level_Weapon_Shop_Selected_" + to_wstring(index); // 혹시 몰라서.
            vecButtonDescs[index].bActive = true;
            vecButtonDescs[index].Button_Type = CGamePlay_Button::BUTTON_TYPE_ENUM::WEAPON_SHOP_BUTTON;
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

                pButton->SetOnClickCallback([this, pButton, index]()
                    {
                        switch (index)
                        {
                        case 0:                       
                            MessageBox(nullptr, L"피의 학살자로 업글완료!!", L"구매 완료", MB_OK);
                            break;
                        case 1:
                            MessageBox(nullptr, L"공허 약탈자로 업글완료! !", L"구매 완료", MB_OK);
                       
                            break;
                        case 2:
                            MessageBox(nullptr, L"번개 지팡이로 업글완료!", L"구매 완료", MB_OK);
                            break;
                        case 3:
                            MessageBox(nullptr, L"빙결 지팡이로 업글 완료!", L"구매 완료", MB_OK);
                            break;
                        case 4:
                            MessageBox(nullptr, L" .50 구경 매그넘으로 업글완료!", L"구매 완료", MB_OK);
                            break;
                        case 5:
                            MessageBox(nullptr, L"빈 공간!", L"빈 공간", MB_OK);
                            break;
                        case 6:
                            MessageBox(nullptr, L"빈 공간!", L"빈 공간!", MB_OK);
                            break;
                        case 7:
                            MessageBox(nullptr, L"빈 공간!", L"빈 공간", MB_OK);
                            break;                     
                        default:
                            break;
                        }
                    });

                pButton->SetOnMouseCallback([this, pButton, index]()
                    {
                        if (index < m_str_Weapon_MouseOn_Text.size())
                        {
                            pButton->m_strMouseOnText = m_str_Weapon_MouseOn_Text[index];
                        }
                    });
            }

        }
    }
}


CUI_WeaponShop_UI* CUI_WeaponShop_UI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CUI_WeaponShop_UI* pInstance = new CUI_WeaponShop_UI(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CUI_WeaponShop_UI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_WeaponShop_UI::Clone(void* pArg)
{
    CUI_WeaponShop_UI* pInstance = new CUI_WeaponShop_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CUI_WeaponShop_UI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_WeaponShop_UI::Free()
{
    __super::Free();
}

