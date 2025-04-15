#include "UI_WeaponShop_UI.h"
#include "GameInstance.h"
#include "GamePlay_Button.h"
#include "UI_Manager.h"
#include <Hub_WeaponShop.h>



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

	m_pTransformCom->Set_Scale(m_Shop_INFO.vSize.x, m_Shop_INFO.vSize.y, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Shop_INFO.vPos.x, m_Shop_INFO.vPos.y, 0.f));

	CUI_Manager::GetInstance()->AddUI(L"Weapon_Shop_UI", this, UI_TYPE::UI_DYNAMIC);

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

	for (int index = 0; index < 3; ++index)
	{
		switch (index)
		{
		case 0:
			m_str_Weapon_MouseOn_Text[index] = L"클레이모어 \n 적을 압도하는 대검.\n느리지만 강력한 \n공격력을 자랑한다.\n 스킬 포인트: 3";
			break;
		case 1:
			m_str_Weapon_MouseOn_Text[index] = L"하베스터 \n 샷건의 상위 호환형 무기. \n 근거리에서 압도적인 화력과 \n파괴력을 발휘한다.\n 스킬 포인트: 5 ";
			break;
		case 2:
			m_str_Weapon_MouseOn_Text[index] = L"스태프 \n 신비한 마력을 방출하는 지팡이.\n 원거리 마법 공격과 \n적군 무력화에 탁월하다.\n 스킬 포인트: 5";
			break;
		default:
			break;
		}
	}

	return S_OK;
}

HRESULT CUI_WeaponShop_UI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(m_iPlayerSkillPoint),
		_float2(150.f, -220.f), _float2(20.f, 40.f), _float3(1.f, 1.f, 0.f));
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
					{   PurchaseStatus eState = PurchaseStatus::ShopClosed;
				CHub_WeaponShop* pShop = dynamic_cast<CHub_WeaponShop*>(m_pGameInstance->Find_Object(LEVEL_HUB, L"Layer_Weapon_Shop"));
				if (pShop)
				{
					switch (index)
					{
					case 0:
						eState = pShop->Purchase_Item(TEXT("Claymore"), 3);
						break;
					case 1:
						eState = pShop->Purchase_Item(TEXT("Harvester"), 5);
						break;
					case 2:
						eState = pShop->Purchase_Item(TEXT("Staff"), 5);
						break;
					default:
						break;
					}

					auto pUI_Event = static_cast<CUI_Event*>(CUI_Manager::GetInstance()->GetUI(TEXT("UI_Event")));
					CUI_Event::EVENT_RENDER_TEXT vRenderText;
					if (pUI_Event)
					{
						vRenderText.vPos = _float2(-70.f, 0.f);
						vRenderText.vFontSize = _float2(10.f, 30.f);
						vRenderText.vColor = _float3(1.f, 1.f, 0.1f);
						vRenderText.fLifeTime = 1.f;
					}
					switch (eState)
					{
					case PurchaseStatus::Success:		
						vRenderText.vPos = _float2(-100.f, -100.f);
						vRenderText.vColor = _float3(1.f, 0.4f, 0.4f);
						vRenderText.stText = TEXT("구매 완료.");
						break;
					case PurchaseStatus::NotEnoughPoint:
						vRenderText.vPos = _float2(-100.f, 0.f);
						vRenderText.stText = TEXT("스킬 포인트가 부족합니다.");
						break;
					case PurchaseStatus::AlreadyOwned:
						vRenderText.stText = TEXT("이미 구매했습니다.");
						break;
					case PurchaseStatus::ShopClosed:
					default:
						break;
					}
					
					pUI_Event->Add_EventRender(vRenderText);
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

