#include "GamePlay_Button.h"
#include "GameInstance.h"
#include "CUI_Manager.h"
#include "UI_Episode.h"

_int CGamePlay_Button::s_fStrength_Point = 0;
_int CGamePlay_Button::s_fLife_Point = 0;
_int CGamePlay_Button::s_fSprit_Point = 0;
_int CGamePlay_Button::s_fCapacity_Point = 0;

_int CGamePlay_Button::s_fStat_Point = 0;
_int CGamePlay_Button::s_fLevel_Point = 0;
_int CGamePlay_Button::s_fLive_Point = 0;

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
		if (m_Button_INFO.strUIName != L"Level_1_Display" && m_Button_INFO.Button_type == Episode)
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

	s_fStat_Point; // 잔여 스탯 포인트
	s_fLevel_Point;// 레벨 포인트 
	s_fLive_Point; // 목숨 갯수  

	s_fStrength_Point; // 근력
	s_fLife_Point; // 생맹력
	s_fSprit_Point; // 정신력
	s_fCapacity_Point; // 용량

	strStat_Tag_box = L"";
	if (m_Button_INFO.Button_type == Episode)
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
void CGamePlay_Button::Font_Render()
{
	// "MainFont" 태그로 등록한 폰트를 사용하여 텍스트를 좌표 위치에 RGB로 렌더링.
	if (m_Button_INFO.Button_type == Point_Shop)
	{
		m_pGameInstance->Render_Font(L"MainFont", L"근접 마스터", _float2(0.f, -205.0f), _float3(1.f, 1.f, 0.0f));
		m_pGameInstance->Render_Font(L"MainFont", L"트레져 헌터", _float2(0.f, -163.0f), _float3(1.f, 1.f, 0.0f));
		m_pGameInstance->Render_Font(L"MainFont", L"빠른 치료자", _float2(0.f, -122.0f), _float3(1.f, 1.f, 0.0f));
		m_pGameInstance->Render_Font(L"MainFont", L"학생", _float2(23.f, -78.0f), _float3(1.f, 1.f, 0.0f));
		m_pGameInstance->Render_Font(L"MainFont", L"산탄총 전문가", _float2(0.f, -35.F), _float3(1.f, 1.f, 0.0f));
		m_pGameInstance->Render_Font(L"MainFont", L"방화광", _float2(20.f, 10.F), _float3(1.f, 1.f, 0.0f));

		m_pGameInstance->Render_Font(L"MainFont", L"리드 수집가", _float2(220.f, -205.0f), _float3(1.f, 1.f, 0.0f));
		m_pGameInstance->Render_Font(L"MainFont", L"귀신", _float2(250.f, -163.0f), _float3(1.f, 1.f, 0.0f));
		m_pGameInstance->Render_Font(L"MainFont", L"강인함", _float2(240.f, -123.0f), _float3(1.f, 1.f, 0.0f));
		m_pGameInstance->Render_Font(L"MainFont", L"마법 잠재력", _float2(225.f, -78.0f), _float3(1.f, 1.f, 0.0f));
		m_pGameInstance->Render_Font(L"MainFont", L"소울 수집가", _float2(225.f, -35.F), _float3(1.f, 1.f, 0.0f));
		m_pGameInstance->Render_Font(L"MainFont", L"스프린터", _float2(232.f, 10.F), _float3(1.f, 1.f, 0.0f));

		m_pGameInstance->Render_Font(L"MainFont", strMin_Stat_box, _float2(-40.f, 137.0f), _float3(1.f, 1.f, 0.0f));
		m_pGameInstance->Render_Font(L"MainFont", strItem_Tag, _float2(0.f, 50.0f), _float3(1.f, 1.f, 0.0f));


	}


}

void CGamePlay_Button::Stat_Render()
{
	if (m_Button_INFO.Button_type == Point_Shop)
	{
		// 테스트용 나중에 플레이어와 연동


		m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(s_fStrength_Point), _float2(-308.f, -148.F), _float2(30.f, 40.F), _float3(1.f, 1.f, 0.0f));

		m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(s_fLife_Point), _float2(-308.f, -85.F), _float2(30.f, 40.F), _float3(1.f, 1.f, 0.0f));

		m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(s_fSprit_Point), _float2(-308.f, -22.F), _float2(30.f, 40.F), _float3(1.f, 1.f, 0.0f));

		m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(s_fCapacity_Point), _float2(-308.f, 38.F), _float2(30.f, 40.F), _float3(1.f, 1.f, 0.0f));

		m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(s_fStat_Point), _float2(-366.f, -208.F), _float2(30.f, 40.F), _float3(1.f, 1.f, 0.0f));
		m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(s_fLevel_Point), _float2(-285.f, -226.F), _float2(30.f, 40.F), _float3(1.f, 1.f, 0.0f));
		m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(s_fLive_Point), _float2(-172.f, -228.F), _float2(30.f, 40.F), _float3(1.f, 1.f, 0.0f));

		m_pGameInstance->Render_Font(L"MainFont", strStat_Tag_box, _float2(-350.f, 137.0f), _float3(1.f, 1.f, 0.0f));
	}
}

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
	////////////// 나중에 여기서 버튼 클릭 여부 체크하면됨
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
					Button_TexT(m_Button_INFO.Point_Shop_Num);
					// 여기 안에서 버튼 클릭 할 예정 배열로 
				}
				else
				{
					m_bOpen_Display = false;
				}

				// 버튼이 하나 선택되면 반복을 종료할 수도?

			}
		}

		for (int i = 12; i < 16; i++) // 12 부터 13까지  // 13번째 이미지부터14번쨰 이미지까지
		{
			if (m_Button_INFO.Point_Shop_Seleted[i])
			{
				if (isPick(g_hWnd))
				{
					m_bChange_Click = true;
					m_bOpen_Display = true;
					Current_Image = Point_Shop_Small_Selected;

					Button_TexT(m_Button_INFO.Point_Shop_Num);
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
void CGamePlay_Button::Button_TexT(_int CurrentButtonType)
{
	// 밑은 각 버튼에 대한 설명 텍스트박스임
	switch (CurrentButtonType)
	{
	case AXE_POWER:
		strItem_Tag = L"도끼로 연속 3번 히트시  \n 마지막 3타를 줜나 쎄게 때립니다!";
		strMin_Stat_box = L"요구 스탯 : 근력 5";
		break;
	case BULLET_COLLECTOR:
		strItem_Tag = L"모든 피스톨(+2),SMG총기(+5),미니건(+5)에게 \n 추가탄약을 제공합니다 ";
		strMin_Stat_box = L"요구 스탯 : 용량 5";
		break;
	case TREASURE_HUNTER:
		strItem_Tag = L"보물을 찾을 확률이 2배가 됩니당";
		strMin_Stat_box = L"요구 스탯 없음";
		break;
	case GHOST:
		strItem_Tag = L"모든 몬스터를 통과 할 수 있게 됩니당.";
		strMin_Stat_box = L"요구 스탯 : 정신력 5";
		break;
	case HEAL_IS_GOOD:
		strItem_Tag = L"모든 메디킷의 효과를 더 야물딱지게 받게됩니다.";
		strMin_Stat_box = L"요구 스탯 : 생명력 5";
		break;
	case SUPER_BOY:
		strItem_Tag = L"최대 생명력이(+30)만큼 늘어나고 들어오는 \n 피해는 (-20%) 만큼 감소합니다.";
		strMin_Stat_box = L"요구 스탯 : 생명력 5";
		break;
	case STUDENT:
		strItem_Tag = L"학생 효과를 모르겠어";
		strMin_Stat_box = L"조건을 모르겠엉";
		break;
	case MAGICIAN:
		strItem_Tag = L"최대 마나가(+30) 증가합니다";
		strMin_Stat_box = L"요구 스탯 : 정신력 5";
		break;
	case SHUTGUN_MASTER:
		strItem_Tag = L"산탄총이 2개의 탄약을 더 제공합니다.";
		strMin_Stat_box = L"요구 스탯 : 용량 5";
		break;
	case SOUL_COLLECTOR:
		strItem_Tag = L"적을 죽일시 에너지가 드랍되어 획득시 \n 마나가 소량 증가합니다";
		strMin_Stat_box = L"요구 스탯 : 정신력 7";
		break;
	case FIRE_BOY:
		strItem_Tag = L"방화광은 추가 연료(+15),다이너마이트(+1),로켓(+1)\n 그리고 세포(+5)를 제공합니다";
		strMin_Stat_box = L"요구 스탯 : 용량 5";
		break;
	case VERY_FASTER:
		strItem_Tag = L"조오오온나 빨라짐 ㅋㅋ";
		strMin_Stat_box = L"요구 스탯 : 근,생맹력 3";
		break;
	case 12:
		strItem_Tag = L"앙깔롱띠 ㅋㅋ";
		strStat_Tag_box = L"어 그래 나 하남의 홍동완이다";
		if (GetKeyState(VK_LBUTTON) & 0x8000)
		{
			s_fStrength_Point += 1;
			s_fStat_Point -= 1;
			s_fLevel_Point += 2;
			s_fLive_Point -= 2;
		}
		break;
	case 13:
		strItem_Tag = L"앙검은띠  ㅋㅋ";
		strStat_Tag_box = L"어 그래 나 수원의 차명훈이다";
		if (GetKeyState(VK_LBUTTON) & 0x8000)
			s_fLife_Point += 1;
		break;
	case 14:
		strItem_Tag = L"앙후드띠  ㅋㅋ";
		strStat_Tag_box = L"어 그래 나 파주의 김장원이다";
		if (GetKeyState(VK_LBUTTON) & 0x8000)
			s_fSprit_Point += 1;
		break;
	case 15:
		strItem_Tag = L"앙 허리띠 ㅋㅋ";
		strStat_Tag_box = L"어 그래 나 낙성대의 김경래다";
		if (GetKeyState(VK_LBUTTON) & 0x8000)
			s_fCapacity_Point += 1;
		break;

	default:
		strItem_Tag = L"";
		break;

	}

}
void CGamePlay_Button::Spell_Shop_Display_Button()
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

	Font_Render();
	Stat_Render();

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);


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
