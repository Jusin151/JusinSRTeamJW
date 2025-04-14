#include "UI_Episode_Hub.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Hub_Portal.h"


CUI_Episode_Hub::CUI_Episode_Hub(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CUI_Shop_Base(pGraphic_Device)
{
}

CUI_Episode_Hub::CUI_Episode_Hub(const CUI_Episode_Hub& Prototype)
	:CUI_Shop_Base(Prototype)
{
}

HRESULT CUI_Episode_Hub::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Episode_Hub::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_Shop_INFO = *reinterpret_cast<UI_Desc*>(pArg);
	else
		return E_FAIL;

	m_fFisrt_Button_Pos = { -108.f,118.f };

	m_fSecond_Button_Pos = { 80.f,118.f };

	m_fThird_Button_Pos = { 273.f,118.f };

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

	CUI_Manager::GetInstance()->AddUI(L"Episode_Hub_UI", this, UI_TYPE::UI_DYNAMIC);
	m_pPortal = static_cast<CHub_Portal*>(m_pGameInstance->Find_Object(LEVEL_HUB, TEXT("Layer_Portal")));
	return S_OK;
}

HRESULT CUI_Episode_Hub::Register_Buttons()
{
	Create_Episode_Button();
	Create_Episode_Select_Button();
	Create_Episode_Icon_Image();
	return S_OK;
}

void CUI_Episode_Hub::Create_Episode_Button() {}

void CUI_Episode_Hub::Priority_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

void CUI_Episode_Hub::Update(_float fTimeDelta)
{
	m_bIsActive = m_bOnUI;
}

void CUI_Episode_Hub::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_Episode_Hub::Ready_Texture()
{
	if (FAILED(__super::Add_Component(LEVEL_HUB, TEXT("Prototype_Component_Texture_Episode_Hub_UI"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	return S_OK;
}

HRESULT CUI_Episode_Hub::Ready_Skill_Button_Text() { return S_OK; }
HRESULT CUI_Episode_Hub::Ready_Stat_Button_Text() { return S_OK; }

HRESULT CUI_Episode_Hub::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

void CUI_Episode_Hub::Create_Episode_Select_Button() //초록색 셀렉트 버튼
{
	vector<CGamePlay_Button::GamePlayer_Button_Desc> vecButtonDescs(3); // 현재 3개만 생성이라 3개로 잡음

	for (int index = 0; index < MAP_END; ++index) // 버튼 3개만 생성
	{
		vecButtonDescs[index].Button_Desc.vSize = { 192.f, 192.f };

		switch (index) // 버튼 현재 3개
		{
		case 0:
			vecButtonDescs[index].Button_Desc.vPos = m_fFisrt_Button_Pos;  //{ -108.f,118.f };
			break;
		case 1:
			vecButtonDescs[index].Button_Desc.vPos = m_fSecond_Button_Pos; //{ 273.f,118.f };
			break;
		case 2:
			vecButtonDescs[index].Button_Desc.vPos = m_fThird_Button_Pos; //{ -108.f,-73.f };
			break;
		default:
			break;
		}

		vecButtonDescs[index].strTexture_Default_Tag = L"Prototype_Component_Texture_Episode_Hub_UI_Selected";
		vecButtonDescs[index].strUIName = L"Level_Episode_Button_Selected_" + to_wstring(index);
		vecButtonDescs[index].bActive = true;
		vecButtonDescs[index].iCurrentImageNum = 0;
		vecButtonDescs[index].Button_Type = CGamePlay_Button::BUTTON_TYPE_ENUM::EPISODE_BUTTON_SELECTED;

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

			pButton->SetOnMouseCallback([this, pButton, index]()
				{
					if (index < m_str_Skill_MouseOn_Text.size())
					{
						pButton->m_strMouseOnText = m_str_Skill_MouseOn_Text[index];
					}
				});
		}
	}

}
void CUI_Episode_Hub::Create_Episode_Icon_Image() // 맵 이미지들
{
	vector<CGamePlay_Button::GamePlayer_Button_Desc> vecButtonDescs(5);

	for (int index = 0; index < MAP_END; ++index) // 버튼 3개만 생성
	{
		vecButtonDescs[index].Button_Desc.vSize = { 192.f, 192.f };
		switch (index)
		{
		case FIRST_NORMAL_MAP:
			vecButtonDescs[index].Button_Desc.vPos = m_fFisrt_Button_Pos;
			vecButtonDescs[index].iCurrentImageNum = FIRST_NORMAP_MAP_COLOR;
			break;
		case FIRST_BOSS_MAP:
			vecButtonDescs[index].Button_Desc.vPos = m_fSecond_Button_Pos;
			vecButtonDescs[index].iCurrentImageNum = FIRST_BOSS_MAP_GRAY;
			break;
		case SECOND_BOSS_MAP:
			vecButtonDescs[index].Button_Desc.vPos = m_fThird_Button_Pos;
			vecButtonDescs[index].iCurrentImageNum = SECOND_BOSS_MAP_GRAY;
			break;
		}
		vecButtonDescs[index].strTexture_Default_Tag = L"Prototype_Component_Texture_Episode_Hub_UI_Level_1";
		vecButtonDescs[index].strUIName = L"Level_Episode_Button_Image_" + to_wstring(index);
		vecButtonDescs[index].bActive = true;
		vecButtonDescs[index].bRender = true;
		vecButtonDescs[index].Button_Type = CGamePlay_Button::BUTTON_TYPE_ENUM::EPISODE_BUTTON_MAP_IMAGE;

		if (FAILED(m_pGameInstance->Add_GameObject(
			LEVEL_HUB,
			TEXT("Prototype_GameObject_GamePlayer_Button"),
			LEVEL_HUB,
			vecButtonDescs[index].strUIName,
			&vecButtonDescs[index])))
			continue;

		CGamePlay_Button* pButton = static_cast<CGamePlay_Button*>(m_pGameInstance->Find_Object(
			LEVEL_HUB, vecButtonDescs[index].strUIName.c_str()));
		if (pButton)
		{
			m_vecMapButtons.push_back(pButton);

			pButton->SetOnClickCallback([this, index]()
				{
					_uint& current = m_vecMapButtons[index]->m_Button_Info.iCurrentImageNum;

					switch (index)
					{
					case FIRST_NORMAL_MAP: // 0번
						if (current == FIRST_NORMAP_MAP_COLOR)
						{
							current = FIRST_NORMAP_MAP_RED;

							if (m_vecMapButtons.size() > FIRST_BOSS_MAP &&
								m_vecMapButtons[FIRST_BOSS_MAP]->m_Button_Info.iCurrentImageNum == FIRST_BOSS_MAP_GRAY)
							{

								if (m_pPortal)
								{
									m_pPortal->SetActive(true);
									m_pPortal->Set_Level(LEVEL_GAMEPLAY);
								}

								m_vecMapButtons[FIRST_BOSS_MAP]->m_Button_Info.iCurrentImageNum = FIRST_BOSS_MAP_COLOR;
							}
						}
						break;

					case FIRST_BOSS_MAP:
						if (m_vecMapButtons[FIRST_NORMAL_MAP]->m_Button_Info.iCurrentImageNum != FIRST_NORMAP_MAP_RED)
						{
							MessageBox(nullptr, L"선행 맵을 클리어하세요!", L"에피소드 진입 불가", MB_OK);
							return;
						}

						if (current == FIRST_BOSS_MAP_COLOR)
						{
							current = FISRT_BOSS_MAP_RED;
							if (m_vecMapButtons.size() > SECOND_BOSS_MAP &&
								m_vecMapButtons[SECOND_BOSS_MAP]->m_Button_Info.iCurrentImageNum == SECOND_BOSS_MAP_GRAY)
							{
								if (m_pPortal)
								{
									m_pPortal->SetActive(true);
									m_pPortal->Set_Level(LEVEL_BOSS);
								}
								m_vecMapButtons[SECOND_BOSS_MAP]->m_Button_Info.iCurrentImageNum = SECOND_BOSS_MAP_COLOR;
							}
						}
						break;

					case SECOND_BOSS_MAP:
						if (m_vecMapButtons[FIRST_BOSS_MAP]->m_Button_Info.iCurrentImageNum != FISRT_BOSS_MAP_RED)
						{
							MessageBox(nullptr, L"선행 맵을 클리어하세요!", L"에피소드 진입 불가", MB_OK);
							return;
						}

						if (current == SECOND_BOSS_MAP_COLOR)
						{
							current = SECOND_BOSS_MAP_RED;

							if (m_pPortal)
							{
								m_pPortal->SetActive(true);
								m_pPortal->Set_Level(LEVEL_HONG);
							}
						}
						break;
					}
				});

		}
	}
}

void CUI_Episode_Hub::Change_Button_Image(CGamePlay_Button* type, _uint FirstImage, _uint SecondImage)
{
	if (type->m_Button_Info.iCurrentImageNum == FirstImage)
		type->m_Button_Info.iCurrentImageNum = SecondImage;
	else
		type->m_Button_Info.iCurrentImageNum = FirstImage;
}

CUI_Episode_Hub* CUI_Episode_Hub::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Episode_Hub* pInstance = new CUI_Episode_Hub(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Episode_Hub");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Episode_Hub::Clone(void* pArg)
{
	CUI_Episode_Hub* pInstance = new CUI_Episode_Hub(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Episode_Hub");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Episode_Hub::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

}
