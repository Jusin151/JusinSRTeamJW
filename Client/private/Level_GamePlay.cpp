#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "PickingSys.h"
#include "Collider_Sphere.h"
#include "UI_Default_Panel.h"
#include "CUI_Base.h"
#include "GamePlay_Button.h"
#include "JsonLoader.h"
#include "Weapon_Base.h"
#include "Effect_Base.h"


CLevel_GamePlay::CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device },
	m_pPickingSys{ CPickingSys::Get_Instance() }

{
	m_pPickingSys->Initialize(g_hWnd, m_pGraphic_Device, m_pGameInstance);
	Safe_AddRef(m_pPickingSys);
}
HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	CJsonLoader jsonLoader;
 	jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL_GAMEPLAY.json", LEVEL_GAMEPLAY);
	

	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Shop()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	//	return E_FAIL;

	/*if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Harpoon"), TEXT("Layer_Monster_Projectile_Harpoon"), 10)))
		return E_FAIL;*/

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Inven"),
		LEVEL_GAMEPLAY, TEXT("Layer_Inven"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_PointShop"),
		LEVEL_GAMEPLAY, TEXT("Layer_Shop"))))
		return E_FAIL;


	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{

	///// 오브젝트 픽킹 후 드래그 테스트/////////////////////////////
	//m_pPickingSys->Update();
	//static CGameObject* dragObject = nullptr;
	//static _float3 vDragOffset{};
	//static _float3 vDragPlaneNormal{};
	//static _float fDistanceFromCamera = 0.f;
	//static _float fDragPlaneDistance = 0.f;
	//static bool bInitialized = false;

	//auto colliderVec = m_pGameInstance->Get_Colliders();

	//// 마우스 버튼이 떼어진 경우 드래그 객체 초기화
	//if (!(GetKeyState(VK_LBUTTON) & 0x8000))
	//{
	//	dragObject = nullptr;
	//	bInitialized = false;
	//}
	//// 마우스 왼쪽 버튼이 눌린 경우
	//else
	//{
	//	// 드래그 객체가 없으면 객체 선택
	//	if (!dragObject)
	//	{
	//		for (auto& colliderList : colliderVec)
	//		{
	//			for (auto& collider : colliderList)
	//			{
	//				if (m_pPickingSys->Ray_Intersection(collider))
	//				{
	//					dragObject = collider->Get_Owner();
	//					bInitialized = false;
	//					break;
	//				}
	//			}
	//			if (dragObject) break;
	//		}
	//	}

	//	// 선택된 객체가 있으면 이동 처리
	//	if (dragObject)
	//	{
	//		CTransform* pTransform = dynamic_cast<CTransform*>(dragObject->Get_Component(TEXT("Com_Transform")));
	//		if (pTransform)
	//		{
	//			// 카메라 정보 가져오기
	//			CGameObject* pCamera = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Camera"));
	//			CTransform* pCamTransform = dynamic_cast<CTransform*>(pCamera->Get_Component(TEXT("Com_Transform")));
	//			_float3 vCamLook = pCamTransform->Get_State(CTransform::STATE_LOOK).GetNormalized();
	//			// 첫 드래그 시 초기화
	//			if (!bInitialized)
	//			{
	//				// 객체의 현재 위치 가져오기
	//				_float3 vObjPos = pTransform->Get_State(CTransform::STATE_POSITION);

	//						 // 객체의 현재 위치와 카메라 시선 방향을 기반으로 드래그 평면 정의
	//		// 카메라 시선 벡터를 평면의 법선으로 사용
	//				vDragPlaneNormal = vCamLook;
	//				fDragPlaneDistance = D3DXVec3Dot(&vDragPlaneNormal, &vObjPos);

	//				// 초기 오프셋 계산
	//				_float3 vRayOrigin = m_pPickingSys->Get_Ray().vOrigin;
	//				_float3 vRayDir = m_pPickingSys->Get_Ray().vDir;

	//				// 평면과의 교차점 계산
	//				float t = (fDragPlaneDistance - D3DXVec3Dot(&vDragPlaneNormal, &vRayOrigin)) /
	//					D3DXVec3Dot(&vDragPlaneNormal, &vRayDir);

	//				_float3 vIntersectPoint = (vRayDir * t);
	//				D3DXVec3Add(&vIntersectPoint, &vRayOrigin, &vIntersectPoint);

	//				// 객체와 교차점 사이의 오프셋 계산
	//				D3DXVec3Subtract(&vDragOffset, &vObjPos, &vIntersectPoint);


	//				bInitialized = true;
	//			}
	//			vDragPlaneNormal = vCamLook;
	//			_float3 vRayOrigin = m_pPickingSys->Get_Ray().vOrigin;
	//			_float3 vRayDir = m_pPickingSys->Get_Ray().vDir;

	//			float t = (fDragPlaneDistance - D3DXVec3Dot(&vDragPlaneNormal, &vRayOrigin)) /
	//				D3DXVec3Dot(&vDragPlaneNormal, &vRayDir);

	//			_float3 vIntersectPoint = (vRayDir * t);
	//			D3DXVec3Add(&vIntersectPoint, &vRayOrigin, &vIntersectPoint);
	//			_float3 vNewPos;
	//			D3DXVec3Add(&vNewPos, &vIntersectPoint, &vDragOffset);ddw

	//			// 객체 위치 업데이트
	//			pTransform->Set_State(CTransform::STATE_POSITION, vNewPos);
	//		}
	//	}
	//}

	///// 오브젝트 픽킹 후 드래그 테스트/////////////////////////////
}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{

	//if (FAILED(m_pGameInstance->Add_GameObject
	//(LEVEL_GAMEPLAY, 
	//	TEXT("Prototype_GameObject_Terrain"),
	//	LEVEL_GAMEPLAY, strLayerTag)))
	//	return E_FAIL;
	// 오브젝트 풀에 등록
	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Terrain"), strLayerTag, 1)))
		return E_FAIL;

	// 오브젝트 풀링에서 가져와서 오브젝트 매니저에 추가
	if(nullptr ==(m_pGameInstance->Add_GameObject_FromPool(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, strLayerTag)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Shop()
{

	CUI_Base::UI_Child_Desc Spell_Shop_UI{};  // 스펠샵
	Spell_Shop_UI.vSize = { 847.f,508.f };
	Spell_Shop_UI.fAlpha = 1.0f;
	Spell_Shop_UI.vPos = { 0.f,00.f }; 

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Spell_Shop_UI"),
		LEVEL_GAMEPLAY, TEXT("Layer_Spell_Shop_1"), &Spell_Shop_UI)))
		return E_FAIL;


	Ready_Layer_Episode_Button();
	Ready_Layer_Point_Shop_Button();

	return S_OK;
}

#include "GamePlay_Button.h"

// ...

HRESULT CLevel_GamePlay::Ready_Layer_Episode_Button() // 레벨은 큰 라운드 스테이지는 레벨안의 스테이지
{
	CGamePlay_Button::GamePlayer_Button_Desc Level_1_Display{}; // 레벨 5 이미지의 첫번째 스테이지
	Level_1_Display.Button_Desc.vSize = { 846.f,508.f };
	Level_1_Display.Button_Desc.vPos = { 0.f,0.f };
	Level_1_Display.strTexture_Default_Tag = { L"Prototype_Component_Texture_Episode_Level_1_UI" };
	Level_1_Display.strUIName = { L"Level_1_Display" };
	Level_1_Display.Button_type = Button_type::Episode;
	Level_1_Display.bDisplay_On = true;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GamePlayer_Button"),
		LEVEL_LOGO, TEXT("Layer_Episode_0"),
		&Level_1_Display)))
		return E_FAIL;



    CGamePlay_Button::GamePlayer_Button_Desc Level_1_Icon_Desc{}; // 왼쪽 레벨 1 이미지 ( ex 후드쓴 개구리 
    Level_1_Icon_Desc.Button_Desc.vSize = { 147.f,72.f };
    Level_1_Icon_Desc.Button_Desc.vPos = { -320.f,184.f };
    Level_1_Icon_Desc.strTexture_Default_Tag = { L"Prototype_Component_Texture_Episode_Level_1_UI" };
    Level_1_Icon_Desc.strUIName = { L"Level_1_Icon" };
    Level_1_Icon_Desc.Episode_Button_Type.bLevel_Icon_Button_Flag = true;
	Level_1_Icon_Desc.Button_type = Button_type::Episode;
    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY,TEXT("Prototype_GameObject_GamePlayer_Button"),
        LEVEL_LOGO, TEXT("Layer_Episode_2"), 
        &Level_1_Icon_Desc)))
        return E_FAIL;

	CGamePlay_Button::GamePlayer_Button_Desc Level_1_Level_Stage_Desc{}; // 레벨 1 이미지의 첫번째 스테이지
	Level_1_Level_Stage_Desc.Button_Desc.vSize = { 192.f,192.f };
	Level_1_Level_Stage_Desc.Button_Desc.vPos = { -112.f,121.f };
	Level_1_Level_Stage_Desc.strTexture_Default_Tag = { L"Prototype_Component_Texture_Episode_Level_1_UI" };
	Level_1_Level_Stage_Desc.strUIName = { L"Level_1_01_Stage" };
	Level_1_Level_Stage_Desc.Episode_Button_Type.bLevel_01_Stage_Button_Flag = true;
	Level_1_Level_Stage_Desc.Button_type = Button_type::Episode;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GamePlayer_Button"),
		LEVEL_LOGO, TEXT("Layer_Episode_3"),
		&Level_1_Level_Stage_Desc)))
		return E_FAIL;

	CGamePlay_Button::GamePlayer_Button_Desc Level_2_Level_Stage_Desc{}; // 레벨 2 이미지의 첫번째 스테이지
	Level_2_Level_Stage_Desc.Button_Desc.vSize = { 192.f,192.f };
	Level_2_Level_Stage_Desc.Button_Desc.vPos = { 87.f,121.f };
	Level_2_Level_Stage_Desc.strTexture_Default_Tag = { L"Prototype_Component_Texture_Episode_Level_1_UI" };
	Level_2_Level_Stage_Desc.strUIName = { L"Level_1_02_Stage" };
	Level_2_Level_Stage_Desc.Episode_Button_Type.bLevel_02_Stage_Button_Flag = true;
	Level_2_Level_Stage_Desc.Button_type = Button_type::Episode;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GamePlayer_Button"),
		LEVEL_LOGO, TEXT("Layer_Episode_4"),
		&Level_2_Level_Stage_Desc)))
		return E_FAIL;

	CGamePlay_Button::GamePlayer_Button_Desc Level_3_Level_Stage_Desc{}; // 레벨 3 이미지의 첫번째 스테이지
	Level_3_Level_Stage_Desc.Button_Desc.vSize = { 192.f,192.f };
	Level_3_Level_Stage_Desc.Button_Desc.vPos = { 288.f,121.f };
	Level_3_Level_Stage_Desc.strTexture_Default_Tag = { L"Prototype_Component_Texture_Episode_Level_1_UI" };
	Level_3_Level_Stage_Desc.strUIName = { L"Level_1_03_Stage" };
	Level_3_Level_Stage_Desc.Episode_Button_Type.bLevel_03_Stage_Button_Flag = true;
	Level_3_Level_Stage_Desc.Button_type = Button_type::Episode;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GamePlayer_Button"),
		LEVEL_LOGO, TEXT("Layer_Episode_5"),
		&Level_3_Level_Stage_Desc)))
		return E_FAIL;

	CGamePlay_Button::GamePlayer_Button_Desc Level_4_Level_Stage_Desc{}; // 레벨 4 이미지의 첫번째 스테이지
	Level_4_Level_Stage_Desc.Button_Desc.vSize = { 192.f,192.f };
	Level_4_Level_Stage_Desc.Button_Desc.vPos = { -112.f,-76.f };
	Level_4_Level_Stage_Desc.strTexture_Default_Tag = { L"Prototype_Component_Texture_Episode_Level_1_UI" };
	Level_4_Level_Stage_Desc.strUIName = { L"Level_1_04_Stage" };
	Level_4_Level_Stage_Desc.Episode_Button_Type.bLevel_04_Stage_Button_Flag = true;
	Level_4_Level_Stage_Desc.Button_type = Button_type::Episode;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GamePlayer_Button"),
		LEVEL_LOGO, TEXT("Layer_Episode_6"),
		&Level_4_Level_Stage_Desc)))
		return E_FAIL;

	CGamePlay_Button::GamePlayer_Button_Desc Level_5_Level_Stage_Desc{}; // 레벨 5 이미지의 첫번째 스테이지
	Level_5_Level_Stage_Desc.Button_Desc.vSize = { 192.f,192.f };
	Level_5_Level_Stage_Desc.Button_Desc.vPos = { 87.f,-76.f };
	Level_5_Level_Stage_Desc.strTexture_Default_Tag = { L"Prototype_Component_Texture_Episode_Level_1_UI" };
	Level_5_Level_Stage_Desc.strUIName = { L"Level_1_05_Stage" };
	Level_5_Level_Stage_Desc.Episode_Button_Type.bLevel_05_Stage_Button_Flag = true;
	Level_5_Level_Stage_Desc.Button_type = Button_type::Episode;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GamePlayer_Button"),
		LEVEL_LOGO, TEXT("Layer_Episode_7"),
		&Level_5_Level_Stage_Desc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Point_Shop_Button()
{

	//객체화 연습중 삭제  x
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Point_Shop"),
		LEVEL_GAMEPLAY, TEXT("Layer_Point_Shop"))))
		return E_FAIL;



	CGamePlay_Button::GamePlayer_Button_Desc Level_Poinst_Shop_Display{}; // 포인트 상점
	Level_Poinst_Shop_Display.Button_Desc.vSize = { 804.f,482.f };
	Level_Poinst_Shop_Display.Button_Desc.vPos = { 0.f,0.f };
	Level_Poinst_Shop_Display.strTexture_Default_Tag = { L"Prototype_Component_Texture_Point_Shop_UI" };
	Level_Poinst_Shop_Display.strUIName = { L"Level_Point_Shop_Display" };
	Level_Poinst_Shop_Display.Button_type = Button_type::Point_Shop;


	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GamePlayer_Button"),
		LEVEL_LOGO, TEXT("Layer_Point_Shop_Display"),
		&Level_Poinst_Shop_Display)))
		return E_FAIL;

	vector<CGamePlay_Button::GamePlayer_Button_Desc> vecButtonDescs(12);

	vecButtonDescs[0].Button_Desc.vPos.y = 197.f;
	vecButtonDescs[1].Button_Desc.vPos.y = 197.f;

	vecButtonDescs[2].Button_Desc.vPos.y = 155.f;
	vecButtonDescs[3].Button_Desc.vPos.y = 155.f;

	vecButtonDescs[4].Button_Desc.vPos.y = 113.f;
	vecButtonDescs[5].Button_Desc.vPos.y = 113.f;

	vecButtonDescs[6].Button_Desc.vPos.y = 70.f;
	vecButtonDescs[7].Button_Desc.vPos.y = 70.f;

	vecButtonDescs[8].Button_Desc.vPos.y = 28.f;
	vecButtonDescs[9].Button_Desc.vPos.y = 28.f;

	vecButtonDescs[10].Button_Desc.vPos.y = -16.f;
	vecButtonDescs[11].Button_Desc.vPos.y = -16.f;



	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			int index = i * 2 + j;
			vecButtonDescs[index].Button_Desc.vSize = { 211.f, 32.f };
			vecButtonDescs[index].Button_Desc.vPos.x = { 221.f*j+47.f }; // 위치 조정
			vecButtonDescs[index].strTexture_Default_Tag = L"Prototype_Component_Texture_Point_Shop_UI"; // 컴포넌트
			vecButtonDescs[index].strUIName = L"Level_Point_Shop_Selected_" + to_wstring(index); // UI 등록 이름
			vecButtonDescs[index].Button_type = Button_type::Point_Shop;
			vecButtonDescs[index].Point_Shop_Num = index;
			vecButtonDescs[index].Point_Shop_Seleted[0] = true;

			if (FAILED(m_pGameInstance->Add_GameObject(
				LEVEL_GAMEPLAY,
				TEXT("Prototype_GameObject_GamePlayer_Button"),
				LEVEL_LOGO,
				TEXT("Layer_Point_Shop_Selected"),
				&vecButtonDescs[index])))
			{
				return E_FAIL;
			}
		}
	}

	vector<CGamePlay_Button::GamePlayer_Button_Desc> vecSmallButtonDescs(4);

	vecSmallButtonDescs[0].Point_Shop_Seleted[12] = true; 
	vecSmallButtonDescs[1].Point_Shop_Seleted[13] = true;
	vecSmallButtonDescs[2].Point_Shop_Seleted[14] = true;
	vecSmallButtonDescs[3].Point_Shop_Seleted[15] = true;

	vecSmallButtonDescs[0].Button_Desc.vPos.y = 130.f;
	vecSmallButtonDescs[1].Button_Desc.vPos.y = 66.f;
	vecSmallButtonDescs[2].Button_Desc.vPos.y = 2.f;
	vecSmallButtonDescs[3].Button_Desc.vPos.y = -58.f;

	for(int i = 0; i < 4; ++i)
	{
		vecSmallButtonDescs[i].Button_Desc.vSize = { 50.f,50.f };
		vecSmallButtonDescs[i].Button_Desc.vPos.x ={ -349.f}; // 위치 조정
		vecSmallButtonDescs[i].strTexture_Default_Tag = L"Prototype_Component_Texture_Point_Shop_UI";
		vecSmallButtonDescs[i].strUIName = L"Level_Point_Shop_Small_Button" + to_wstring(i);
		vecSmallButtonDescs[i].Button_type = Button_type::Point_Shop;
		vecSmallButtonDescs[i].Point_Shop_Num = 12+i;

			if (FAILED(m_pGameInstance->Add_GameObject(
				LEVEL_GAMEPLAY,
				TEXT("Prototype_GameObject_GamePlayer_Button"),
				LEVEL_LOGO,
				TEXT("Layer_Point_Shop_Selected"+ to_wstring(i)),
				&vecSmallButtonDescs[i])))
			{
				return E_FAIL;
			}
		
	}


	return S_OK;
}



HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_FirstPerson"),
		LEVEL_GAMEPLAY, strLayerTag)))
		return E_FAIL;


	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Free"),
	//	LEVEL_GAMEPLAY, strLayerTag)))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Player"),
		LEVEL_GAMEPLAY, strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Player"), strLayerTag, 1)))
		return E_FAIL;

	/*CTransform::TRANSFORM_DESC randTransDesc{};

	randTransDesc.fRotationPerSec = D3DXToRadian(90.f);
	randTransDesc.fSpeedPerSec = 10.f;
	randTransDesc.vPos = { _float(rand() % 50),5.f,_float(rand() % 50) };

	if (nullptr ==(m_pGameInstance->Add_GameObject_FromPool(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, strLayerTag, &randTransDesc)))
		return E_FAIL;*/


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Crocman"), strLayerTag, 10)))
		return E_FAIL;

	if(nullptr ==(m_pGameInstance->Add_GameObject_FromPool(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, strLayerTag)));
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI()
{
	CUI_Base::UI_Child_Desc Menu_Panel{};  // 자식 UI는 3개만 소유 부모 상대적으로 위치 잡을꺼임
	Menu_Panel.vSize = { 453.f,720.f };
	Menu_Panel.fAlpha = 1.0f;
	Menu_Panel.vPos = { 0.f,0.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Menu_Panel"),
		LEVEL_GAMEPLAY, TEXT("1"), &Menu_Panel)))
		return E_FAIL;



	CUI_Base::UI_Parent_Desc DefaultUI_Desc{}; // 부모 UI는 총 6개의 정보 소유

	DefaultUI_Desc.vSize = { 0,0 };
	DefaultUI_Desc.fAlpha = 1.0f;
	DefaultUI_Desc.vPos = { 0.f,0.f };
	DefaultUI_Desc.ProjMatrix = {};
	DefaultUI_Desc.ViewMatrix = {};
	DefaultUI_Desc.WorldMatrix = {};

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Default_PlayerUI"),
		LEVEL_GAMEPLAY, TEXT("Layer_Default_PlayerUI"), &DefaultUI_Desc)))
		return E_FAIL;


	CUI_Base::UI_Child_Desc Left_Panel{};  // 왼쪽 하단 판넬
	Left_Panel.vSize = { 262,210 };
	Left_Panel.fAlpha = 1.0f;
	Left_Panel.vPos = { -510.f,-255.f }; // 부모위치 설정

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Left_Panel"),
		LEVEL_GAMEPLAY, TEXT("Layer_Left_Panel_UI_1"), &Left_Panel)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc Left_Panel_HP{};  // HP 바
	Left_Panel_HP.vSize = { 118.f,18.f };
	Left_Panel_HP.fAlpha = 1.0f;
	Left_Panel_HP.vPos = { 47.f,-80.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Hp_Bar"),
		LEVEL_GAMEPLAY, TEXT("Layer_Left_Panel_UI_2"), &Left_Panel_HP)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc Left_Panel_Player_Icon{};  // 플레이어 아이콘
	Left_Panel_Player_Icon.vSize = { 69.f,61.f };
	Left_Panel_Player_Icon.fAlpha = 1.0f;
	Left_Panel_Player_Icon.vPos = { -589.f,-314.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Player_Icon"),
		LEVEL_GAMEPLAY, TEXT("Layer_Left_Panel_UI_3"), &Left_Panel_Player_Icon)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc Left_Panel_Mana_Bar{};  // 플레이어 아이콘
	Left_Panel_Mana_Bar.vSize = { 120.f,21.f };
	Left_Panel_Mana_Bar.fAlpha = 1.0f;
	Left_Panel_Mana_Bar.vPos = { 22.f,-53.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_MP_Bar"),
		LEVEL_GAMEPLAY, TEXT("Layer_Left_Panel_UI_4"), &Left_Panel_Mana_Bar)))
		return E_FAIL;



	CUI_Base::UI_Child_Desc EXP_Desc{};  // 중앙 패널
	EXP_Desc.vSize = { 850,13.f };
	EXP_Desc.fAlpha = 1.0f;
	EXP_Desc.vPos = { 4.f,-353.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Mid_Panel"),
		LEVEL_GAMEPLAY, TEXT("Layer_Mid_Panel"), &EXP_Desc)))
		return E_FAIL;



	CUI_Base::UI_Child_Desc RIght_Panel{};  // 우하단 패널 
	Left_Panel.vSize = { 270,177 };
	Left_Panel.fAlpha = 1.0f;
	Left_Panel.vPos = { 505.f,-272.f }; // 부모위치 잡아주기

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Right_Bar"),
		LEVEL_GAMEPLAY, TEXT("Layer_Right_Panel_UI_1"), &Left_Panel)))
		return E_FAIL;

	CUI_Base::UI_Child_Desc RIght_Panel_Bullet{};  // 자식 UI는 3개만 소유 부모 상대적으로 위치 잡을꺼임
	Left_Panel_HP.vSize = { 222.f,29.f };
	Left_Panel_HP.fAlpha = 1.0f;
	Left_Panel_HP.vPos = { 8.f,-59.f }; // 부모위치가 원점 상대적으로 얼만큼 잡을껀지

	if (FAILED(m_pGameInstance->Add_GameObject
	(LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_Bullet_Bar"),
		LEVEL_GAMEPLAY, TEXT("Layer_Right_Panel_UI_2"), &Left_Panel_HP)))
		return E_FAIL;


	CWeapon_Base::Weapon_DESC Weapon_Claymore_Desc{}; // 클레이 모어
	Weapon_Claymore_Desc.WeaponID = CWeapon_Base::WEAPON_ID::Claymore;
	Weapon_Claymore_Desc.vPos = { 330.f,-40.f };
	Weapon_Claymore_Desc.vSize = { 2048.,682.f };
	Weapon_Claymore_Desc.Damage = { 100.f };
	Weapon_Claymore_Desc.AttackSpeed = { 1.f };
	Weapon_Claymore_Desc.Range = { 3.f };
	Weapon_Claymore_Desc.Cooldown = { 1.f };
	//Weapon_Claymore_Desc.TextureKey = L"Prototype_Component_Texture_Claymore";
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Weapon_Claymore"),
		LEVEL_GAMEPLAY, TEXT("Layer_Weapon_Claymore"),
		&Weapon_Claymore_Desc)))
		return E_FAIL;

	CWeapon_Base::Weapon_DESC Weapon_Axe_Desc{}; // 도끼
	Weapon_Axe_Desc.WeaponID = CWeapon_Base::WEAPON_ID::Axe;
	Weapon_Axe_Desc.vPos = { 0.f,-160.f };
	Weapon_Axe_Desc.vSize = { 1500,423.f };
	Weapon_Axe_Desc.Damage = { 100.f };
	Weapon_Axe_Desc.AttackSpeed = { 1.f };
	Weapon_Axe_Desc.Range = { 3.f };
	Weapon_Axe_Desc.Cooldown = { 1.f };
	//Weapon_Axe_Desc.TextureKey = L"Prototype_Component_Texture_Axe";
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Weapon_Axe"),
		LEVEL_GAMEPLAY, TEXT("Layer_Weapon_Axe"),
		&Weapon_Axe_Desc)))
		return E_FAIL;


	CWeapon_Base::Weapon_DESC Weapon_ShotGun_Desc{}; // 샷건
	Weapon_ShotGun_Desc.WeaponID = CWeapon_Base::WEAPON_ID::ShotGun;
	Weapon_ShotGun_Desc.vPos = { 0.f,-170.f };
	Weapon_ShotGun_Desc.vSize = { 749,420.f };
	Weapon_ShotGun_Desc.Damage = { 100.f };
	Weapon_ShotGun_Desc.AttackSpeed = { 1.f };
	Weapon_ShotGun_Desc.Range = { 3.f };
	Weapon_ShotGun_Desc.Cooldown = { 1.f };
	//Weapon_ShotGun_Desc.TextureKey = L"Prototype_Component_Texture_ShotGun";
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Weapon_ShotGun"),
		LEVEL_GAMEPLAY, TEXT("Layer_Weapon_ShotGun"),
		&Weapon_ShotGun_Desc)))
		return E_FAIL;

	CWeapon_Base::Weapon_DESC Weapon_Magnum_Desc{}; // 매그넘
	Weapon_Magnum_Desc.WeaponID = CWeapon_Base::WEAPON_ID::Magnum;
	Weapon_Magnum_Desc.vPos = { 70.f,-250.f };
	Weapon_Magnum_Desc.vSize = { 390.f,520.f };
	Weapon_Magnum_Desc.Damage = { 100.f };
	Weapon_Magnum_Desc.AttackSpeed = { 1.f };
	Weapon_Magnum_Desc.Range = { 3.f };
	Weapon_Magnum_Desc.Cooldown = { 1.f };
	//Weapon_Magnum_Desc.TextureKey = L"Prototype_Component_Texture_Magnum";
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Weapon_Magnum"),
		LEVEL_GAMEPLAY, TEXT("Layer_Weapon_Magnum"),
		&Weapon_Magnum_Desc)))
		return E_FAIL;

	CWeapon_Base::Weapon_DESC Weapon_Staff_Desc{}; // 스태프 원형객체
	Weapon_Staff_Desc.WeaponID = CWeapon_Base::WEAPON_ID::Staff;
	Weapon_Staff_Desc.vPos = { -10.f,-220.f };
	Weapon_Staff_Desc.vSize = { 936,525.f };
	Weapon_Staff_Desc.Damage = { 100.f };
	Weapon_Staff_Desc.AttackSpeed = { 1.f };
	Weapon_Staff_Desc.Range = { 3.f };
	Weapon_Staff_Desc.Cooldown = { 1.f };
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Weapon_Staff"),
		LEVEL_GAMEPLAY, TEXT("Layer_Weapon_Staff"),
		&Weapon_Staff_Desc)))
		return E_FAIL;

	CWeapon_Base::Weapon_DESC Weapon_Minigun_Desc{}; // 미니건 원형객체
	Weapon_Minigun_Desc.WeaponID = CWeapon_Base::WEAPON_ID::Minigun;
	Weapon_Minigun_Desc.vPos = { 0.f,-200.f };
	Weapon_Minigun_Desc.vSize = { 959,347.f };
	Weapon_Minigun_Desc.Damage = { 100.f };
	Weapon_Minigun_Desc.AttackSpeed = { 1.f };
	Weapon_Minigun_Desc.Range = { 3.f };
	Weapon_Minigun_Desc.Cooldown = { 1.f };
  	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Minigun"),
		LEVEL_GAMEPLAY, TEXT("Layer_Weapon_Minigun"),
		&Weapon_Minigun_Desc)))
		return E_FAIL;


	CWeapon_Base::Weapon_DESC Weapon_Harvester_Desc{}; // 하베스터 원형객체
	Weapon_Harvester_Desc.WeaponID = CWeapon_Base::WEAPON_ID::Harvester;
	Weapon_Harvester_Desc.vPos = { 80.f,-213.f };
	Weapon_Harvester_Desc.vSize = { 356.f,375.f };
	Weapon_Harvester_Desc.Damage = { 100.f };
	Weapon_Harvester_Desc.AttackSpeed = { 1.f };
	Weapon_Harvester_Desc.Range = { 3.f };
	Weapon_Harvester_Desc.Cooldown = { 1.f };
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Harvester"),
		LEVEL_GAMEPLAY, TEXT("Layer_Weapon_Harvester"),
		&Weapon_Harvester_Desc)))
		return E_FAIL;

	CWeapon_Base::Weapon_DESC Weapon_Sonic_Desc{}; // 소닉 원형객체
	Weapon_Sonic_Desc.WeaponID = CWeapon_Base::WEAPON_ID::Sonic;
	Weapon_Sonic_Desc.vPos = { 270.f,-200.f };
	Weapon_Sonic_Desc.vSize = { 436.f,316.f };
	Weapon_Sonic_Desc.Damage = { 100.f };
	Weapon_Sonic_Desc.AttackSpeed = { 1.f };
	Weapon_Sonic_Desc.Range = { 3.f };
	Weapon_Sonic_Desc.Cooldown = { 1.f };
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Sonic"),
		LEVEL_GAMEPLAY, TEXT("Layer_Weapon_Sonic"),
		&Weapon_Sonic_Desc)))
		return E_FAIL;



	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_GamePlay::Free()
{
	__super::Free();

	Safe_Release(m_pPickingSys);
	CPickingSys::Destroy_Instance();

}
