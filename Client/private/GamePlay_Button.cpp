#include "GamePlay_Button.h"
#include "GameInstance.h"
#include "CUI_Manager.h"
#include "UI_Episode.h"

CGamePlay_Button::CGamePlay_Button(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CGamePlay_Button::CGamePlay_Button(const CGamePlay_Button& Prototype)
	: CUI_Base(Prototype),
	m_GamePlay_Button_pTextureCom(Prototype.m_GamePlay_Button_pTextureCom),
	m_GamePlayer_Button_pVIBufferCom(Prototype.m_GamePlayer_Button_pVIBufferCom),
	m_GamePlayer_Button_pTransformCom{ Prototype.m_GamePlayer_Button_pTransformCom }
{
}

HRESULT CGamePlay_Button::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CGamePlay_Button::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		m_Button_INFO = *reinterpret_cast<GamePlayer_Button_Desc*>(pArg);
		if (m_Button_INFO.strUIName != L"Level_1_Display"&& m_Button_INFO.Button_type==Episode)
		{
			m_Button_INFO.Button_Desc.vPos += CUI_Manager::GetInstance()->GetEpisode_Display_Pos();
		}
		if (m_Button_INFO.strUIName != L"Layer_Point_Shop_Display" && m_Button_INFO.Button_type == Point_Shop)
		{
			m_Button_INFO.Button_Desc.vPos += CUI_Manager::GetInstance()->GetPoint_Shop();
		}
		Set_Position(m_Button_INFO.Button_Desc.vPos);
		Set_Size(m_Button_INFO.Button_Desc.vSize);
		CUI_Manager::GetInstance()->AddUI(m_Button_INFO.strUIName, this);
	}
	else
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_GamePlayer_Button_pTransformCom->Set_Scale(m_Button_INFO.Button_Desc.vSize.x, m_Button_INFO.Button_Desc.vSize.y, 1.f);
	m_GamePlayer_Button_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Button_INFO.Button_Desc.vPos.x, m_Button_INFO.Button_Desc.vPos.y, 0.1f));

	// 예: "MainFont"라는 태그로 폰트를 등록


	return S_OK;
}

void CGamePlay_Button::Priority_Update(_float fTimeDelta)
{
}

void CGamePlay_Button::Update(_float fTimeDelta)
{
	for (int i = 12; i < 16; i++) // 12 부터 13까지  // 13번째 이미지부터14번쨰 이미지까지
	{
		if (m_Button_INFO.Point_Shop_Seleted[i])
		{
			if (isPick(g_hWnd))
			{
				m_bChange_Click = true;
				m_bOpen_Display = true;
				Current_Image = Point_Shop_Small_Selected;
			}
			else
			{
				m_bOpen_Display = false;
			}

			// 버튼이 하나 선택되면 반복을 종료할 수도?

		}
	}

	if(m_Button_INFO.Button_type==Episode)
	Episode_UI_Update();
	if (m_Button_INFO.Button_type == Point_Shop)
    Point_Shop_Update();

}


void CGamePlay_Button::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

void CGamePlay_Button::Episode_UI_Update()
{
	if (!m_Button_INFO.bDisplay_On)
	{
		if (GetKeyState('3') & 0x8000)
		{
			if (!m_bKeyPressed)
			{
				m_bIsVisible = !m_bIsVisible;
				m_bKeyPressed = true;
			}
		}
		else
			m_bKeyPressed = false;
		switch (m_Button_INFO.Button_type)
		{
		case Episode: //
			Episode_Display_Button();
			break;
		case Point_Shop:
			Point_Shop_Display_Button();
			break;
		case Spell_Shop:
			Spell_Shop_Display_Button();
			break;
		case Upgrade_Weapon:
			Upgrade_Weapon_Display_Button();
			break;
		default:
			break;
		}
	}
	else
	{
		Current_Image = Episode_Display;
		if (GetKeyState('3') & 0x8000)
		{
			if (!m_bKeyPressed)
			{
				m_bIsVisible = !m_bIsVisible;
				m_bKeyPressed = true;
			}
		}
		else
			m_bKeyPressed = false;
	}
}

void CGamePlay_Button::Point_Shop_Update()
{
	if (!m_Button_INFO.bDisplay_On)
	{
		if (GetKeyState('2') & 0x8000)
		{
			if (!m_bKeyPressed)
			{
				if (m_Button_INFO.strUIName == L"Level_Point_Shop_Display")
					m_bOpen_Display = true;
				m_bIsVisible = !m_bIsVisible;
				m_bKeyPressed = true;			
			}
		}
		else
			m_bKeyPressed = false;
	}	
		Point_Shop_Display_Button();
	
}

void CGamePlay_Button::Spell_Shop_Update()
{
}

void CGamePlay_Button::Weapon_Upgrade_Update()
{
}

//void CGamePlay_Button::Episode_Display_Button()
//{
//	if (m_bIsVisible)
//	{
//		if (m_Button_INFO.Episode_Button_Type.bLevel_Icon_Button_Flag)
//		{
//			if (!m_bChange_Click)
//			{
//				if (true == isPick(g_hWnd))
//				{
//
//					Current_Image = Level_ICon_Selected;
//				}
//				else
//					Current_Image = Level_ICon_Defaul;
//			}
//
//		}
//	
//		if (m_Button_INFO.Episode_Button_Type.bLevel_01_Stage_Button_Flag) // 1레벨의 1스테이지는 기본적으로 열려있음 그래서 회색이 없음
//		{
//			if (!m_bChange_Click)
//			{
//				if (true == isPick(g_hWnd))
//				{
//					Current_Image = Stage_01_Color_Selected;
//				}
//				else
//					Current_Image = Stage_01_Color_Default;
//			}
//
//		}
//		if (m_Button_INFO.Episode_Button_Type.bLevel_02_Stage_Button_Flag) // 1레벨의 2스테이지
//		{
//			if (!m_bChange_Click)
//			{
//				if (true == isPick(g_hWnd))
//				{
//					if (GetKeyState(VK_LBUTTON) & 0x8000)
//						m_bChange_Click = true;
//					Current_Image = Stage_02_Gray_Selected;
//				}
//				else
//					Current_Image = Stage_02_Gray_Default;
//			}
//			else
//			{
//				if (true == isPick(g_hWnd))
//				{
//					if (GetKeyState(VK_RBUTTON) & 0x8000)
//						m_bChange_Click = false;
//					Current_Image = Stage_02_Color_Selected;
//				}
//				else
//					Current_Image = Stage_02_Color_Default;
//			}
//		}
//		if (m_Button_INFO.Episode_Button_Type.bLevel_03_Stage_Button_Flag) // 1레벨의 3스테이지
//		{
//			if (!m_bChange_Click)
//			{
//				if (true == isPick(g_hWnd))
//				{
//					if (GetKeyState(VK_LBUTTON) & 0x8000)
//						m_bChange_Click = true;
//					Current_Image = Stage_03_Gray_Selected;
//				}
//				else
//					Current_Image = Stage_03_Gray_Default;
//			}
//			else
//			{
//				if (true == isPick(g_hWnd))
//				{
//					if (GetKeyState(VK_RBUTTON) & 0x8000)
//						m_bChange_Click = false;
//					Current_Image = Stage_03_Color_Selected;
//				}
//				else
//					Current_Image = Stage_03_Color_Default;
//			}
//		}
//		if (m_Button_INFO.Episode_Button_Type.bLevel_04_Stage_Button_Flag) // 1레벨의 4스테이지
//		{
//			if (!m_bChange_Click)
//			{
//				if (true == isPick(g_hWnd))
//				{
//					if (GetKeyState(VK_LBUTTON) & 0x8000)
//						m_bChange_Click = true;
//					Current_Image = Stage_04_Gray_Selected;
//				}
//				else
//					Current_Image = Stage_04_Gray_Default;
//			}
//			else
//			{
//				if (true == isPick(g_hWnd))
//				{
//					if (GetKeyState(VK_RBUTTON) & 0x8000)
//						m_bChange_Click = false;
//					Current_Image = Stage_04_Color_Selected;
//				}
//				else
//					Current_Image = Stage_04_Color_Default;
//			}
//		}
//		if (m_Button_INFO.Episode_Button_Type.bLevel_05_Stage_Button_Flag) // 1레벨의 5스테이지
//		{
//			if (!m_bChange_Click)
//			{
//				if (true == isPick(g_hWnd))
//				{
//					if (GetKeyState(VK_LBUTTON) & 0x8000)
//						m_bChange_Click = true;
//					Current_Image = Stage_05_Gray_Selected;
//				}
//				else
//					Current_Image = Stage_05_Gray_Default;
//			}
//			else
//			{
//				if (true == isPick(g_hWnd))
//				{
//					if (GetKeyState(VK_RBUTTON) & 0x8000)
//						m_bChange_Click = false;
//					Current_Image = Stage_05_Color_Selected;
//				}
//				else
//					Current_Image = Stage_05_Color_Default;
//			}
//		}
//	}
//}


void CGamePlay_Button::Episode_Display_Button()
{
	if (m_bIsVisible)
	{
		// Level Icon 버튼 처리 (m_bChange_Click 상태에 상관없이 처리)
		if (m_Button_INFO.Episode_Button_Type.bLevel_Icon_Button_Flag && !m_bChange_Click)
			Current_Image = isPick(g_hWnd) ? Level_ICon_Selected : Level_ICon_Defaul;

		// 1레벨 1스테이지는 기본적으로 열려 있으므로, 회색/기본 이미지가 필요없음
		if (m_Button_INFO.Episode_Button_Type.bLevel_01_Stage_Button_Flag && !m_bChange_Click)
			Current_Image = isPick(g_hWnd) ? Stage_01_Color_Selected : Stage_01_Color_Default;

		//  2~5스테이지 버튼은 클릭에 따라 다른 이미지와 상태 변경 할꺼임
		auto processStageButton = [&](bool buttonFlag,
			auto graySelected, auto grayDefault,
			auto colorSelected, auto colorDefault)
			{
				if (!buttonFlag)
					return; // 해당 스테이지 버튼 플래그가 false이면 아무 작업 x

				if (!m_bChange_Click)
				{
					if (isPick(g_hWnd))
					{
						// 왼쪽 버튼 클릭 시 Gray Selected 이미지 적용
						if (GetKeyState(VK_LBUTTON) & 0x8000)
							m_bChange_Click = true;
						Current_Image = graySelected;
					}
					else
						Current_Image = grayDefault;
				}
				else
				{
					if (isPick(g_hWnd))
					{
						// 오른쪽 버튼 클릭 시  Color Selected 이미지 적용
						if (GetKeyState(VK_RBUTTON) & 0x8000)
						{
							m_bChange_Click = false;
						}

						Current_Image = colorSelected;
					}
					else
						Current_Image = colorDefault;
				}
			};

		// 각 스테이지 버튼에 대해 공통으로
		processStageButton(m_Button_INFO.Episode_Button_Type.bLevel_02_Stage_Button_Flag,
			Stage_02_Gray_Selected, Stage_02_Gray_Default,
			Stage_02_Color_Selected, Stage_02_Color_Default);

		processStageButton(m_Button_INFO.Episode_Button_Type.bLevel_03_Stage_Button_Flag,
			Stage_03_Gray_Selected, Stage_03_Gray_Default,
			Stage_03_Color_Selected, Stage_03_Color_Default);

		processStageButton(m_Button_INFO.Episode_Button_Type.bLevel_04_Stage_Button_Flag,
			Stage_04_Gray_Selected, Stage_04_Gray_Default,
			Stage_04_Color_Selected, Stage_04_Color_Default);

		processStageButton(m_Button_INFO.Episode_Button_Type.bLevel_05_Stage_Button_Flag,
			Stage_05_Gray_Selected, Stage_05_Gray_Default,
			Stage_05_Color_Selected, Stage_05_Color_Default);
	}

}

void CGamePlay_Button::Point_Shop_Display_Button()
{

	if (m_bIsVisible)
	{
		// (총 12개의 버튼)
		for (int i = 0; i < 12; i++) // 0부터 11까지   // 1번째 이미지부터 12번째 이미지까지
		{
			if (m_Button_INFO.Point_Shop_Seleted[i])
			{
				if (isPick(g_hWnd))
				{
					m_bChange_Click = true;
					m_bOpen_Display = true;
					Current_Image = Point_Shop_Selected;
					// 여기 안에서 버튼 클릭 할 예정 배열로 
				}
				else
				{
					m_bOpen_Display = false;
				}

				// 버튼이 하나 선택되면 반복을 종료할 수도?
				
			}
		}
	
	}

}

void CGamePlay_Button::Spell_Shop_Display_Button()
{

}

void CGamePlay_Button::Upgrade_Weapon_Display_Button()
{
	
}
HRESULT CGamePlay_Button::Render()
{
	if (!m_bIsVisible)
		return S_OK;
	if ((m_Button_INFO.Button_type == Point_Shop))
	{
		if (!m_bOpen_Display)
			return S_OK;
	}
	D3DXMATRIX matOldView, matOldProj;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matOldProj);

	D3DXMATRIX matView;
	D3DXMatrixIdentity(&matView);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixOrthoLH(&matProj, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matProj);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	if (FAILED(m_GamePlayer_Button_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_GamePlay_Button_pTextureCom->Bind_Resource(Current_Image)))
		return E_FAIL;


	if (FAILED(m_GamePlayer_Button_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_GamePlayer_Button_pVIBufferCom->Render()))
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	// "MainFont" 태그로 등록한 폰트를 사용하여 텍스트를 좌표 위치에 RGB로 렌더링.
	if (m_Button_INFO.Button_type == Point_Shop)
	{
		/*m_pGameInstance->Render_Font(L"MainFont", L"앙 기머찌 ㅋ", _float2(270.0f, 260.0f), _float3(1.0f, 1.0f, 1.0f));

		m_pGameInstance->Render_Font(L"MainFont", L"잉 기모링 ㅋ", _float2(270.0f, 355.0f), _float3(1.0f, 1.0f, 1.0f));

		m_pGameInstance->Render_Font(L"MainFont", L"양 금모띠 ㅋ", _float2(270.0f, 445.0f), _float3(1.0f, 1.0f, 1.0f));*/

		m_pGameInstance->Render_Font(L"MainFont", L"흑인이 놀라면? 깜짝ㅋㅋ", _float2(300.0f, 510.0f), _float3(1.0f, 1.0f, 1.0f));
	}

	return S_OK;
}
HRESULT CGamePlay_Button::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_Button_INFO.strTexture_Default_Tag,
		TEXT("Com_Texture_Menu_1"), reinterpret_cast<CComponent**>(&m_GamePlay_Button_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Menu"), reinterpret_cast<CComponent**>(&m_GamePlayer_Button_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Menu"), reinterpret_cast<CComponent**>(&m_GamePlayer_Button_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CGamePlay_Button* CGamePlay_Button::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGamePlay_Button* pInstance = new CGamePlay_Button(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("게임플레이레벨 버튼 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CGamePlay_Button::Clone(void* pArg)
{
	CGamePlay_Button* pInstace = new CGamePlay_Button(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("게임 플레이레벨 버튼 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CGamePlay_Button::Free()
{
	__super::Free();

	Safe_Release(m_GamePlay_Button_pTextureCom);
	Safe_Release(m_GamePlayer_Button_pVIBufferCom);
	Safe_Release(m_GamePlayer_Button_pTransformCom);
	
}
