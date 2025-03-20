#include "Level_Editor.h"
#include "GameInstance.h"
#include "PickingSys.h"
#include "Collider_Sphere.h"
#include "UI_Default_Panel.h"
#include "CUI_Base.h"
#include "GamePlay_Button.h"
#include "MyImGui.h"
#include "JsonLoader.h"
#include "Weapon_Base.h"


CLevel_Editor::CLevel_Editor(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel{ pGraphic_Device },
	m_pPickingSys{ CPickingSys::Get_Instance() }

{
	m_pPickingSys->Initialize(g_hWnd, m_pGraphic_Device, m_pGameInstance);
	Safe_AddRef(m_pPickingSys);
}
HRESULT CLevel_Editor::Initialize()
{


	//jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL_GAMEPLAY.json", LEVEL_GAMEPLAY);
	

	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	//	return E_FAIL;

	CJsonLoader jsonLoader;
 	jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL.json", LEVEL_EDITOR);

	m_pImgui = CMyImGui::Create(LEVEL_END, m_pGraphic_Device);
	if (nullptr == m_pImgui)
		return E_FAIL;

	return S_OK;
}

void CLevel_Editor::Update(_float fTimeDelta)
{
	/// 오브젝트 픽킹 후 드래그 테스트/////////////////////////////
	m_pPickingSys->Update();
	static CGameObject* dragObject = nullptr;
	static _float3 vDragOffset{};
	static _float3 vDragPlaneNormal{};
	static _float fDistanceFromCamera = 0.f;
	static _float fDragPlaneDistance = 0.f;
	static bool bInitialized = false;

	auto colliderVec = m_pGameInstance->Get_Colliders();

	// 마우스 버튼이 떼어진 경우 드래그 객체 초기화
	if (!(GetKeyState(VK_LBUTTON) & 0x8000))
	{
		dragObject = nullptr;
		bInitialized = false;
		//m_pImgui->Set_Object();
	}
	// 마우스 왼쪽 버튼이 눌린 경우
	else
	{
		if (m_pImgui->IsMouseOverImGui()) return;
		// 드래그 객체가 없으면 객체 선택
		if (!dragObject)
		{
			for (auto& colliderList : colliderVec)
			{
				for (auto& collider : colliderList)
				{
					if (m_pPickingSys->Ray_Intersection(collider))
					{
						if (collider->Get_Owner()->Get_Tag() == L"Layer_Player") continue;
						dragObject = collider->Get_Owner();
						bInitialized = false;
						break;
					}
				}
				if (dragObject) break;
			}
		}

		// 선택된 객체가 있으면 이동 처리
		if (dragObject)
		{
			m_pImgui->Set_Object(dragObject);
			CTransform* pTransform = dynamic_cast<CTransform*>(dragObject->Get_Component(TEXT("Com_Transform")));
			if (pTransform)
			{
				// 카메라 정보 가져오기
				CGameObject* pCamera = m_pGameInstance->Find_Object(LEVEL_EDITOR, TEXT("Layer_Camera"));
				if (!pCamera) return;
				CTransform* pCamTransform = dynamic_cast<CTransform*>(pCamera->Get_Component(TEXT("Com_Transform")));
				_float3 vCamLook = pCamTransform->Get_State(CTransform::STATE_LOOK).GetNormalized();
				// 첫 드래그 시 초기화
				if (!bInitialized)
				{
					// 객체의 현재 위치 가져오기
					_float3 vObjPos = pTransform->Get_State(CTransform::STATE_POSITION);

							 // 객체의 현재 위치와 카메라 시선 방향을 기반으로 드래그 평면 정의
			// 카메라 시선 벡터를 평면의 법선으로 사용
					vDragPlaneNormal = vCamLook;
					fDragPlaneDistance = D3DXVec3Dot(&vDragPlaneNormal, &vObjPos);

					// 초기 오프셋 계산
					_float3 vRayOrigin = m_pPickingSys->Get_Ray().vOrigin;
					_float3 vRayDir = m_pPickingSys->Get_Ray().vDir;

					// 평면과의 교차점 계산
					float t = (fDragPlaneDistance - D3DXVec3Dot(&vDragPlaneNormal, &vRayOrigin)) /
						D3DXVec3Dot(&vDragPlaneNormal, &vRayDir);

					_float3 vIntersectPoint = (vRayDir * t);
					D3DXVec3Add(&vIntersectPoint, &vRayOrigin, &vIntersectPoint);

					// 객체와 교차점 사이의 오프셋 계산
					D3DXVec3Subtract(&vDragOffset, &vObjPos, &vIntersectPoint);


					bInitialized = true;
				}
				vDragPlaneNormal = vCamLook;
				_float3 vRayOrigin = m_pPickingSys->Get_Ray().vOrigin;
				_float3 vRayDir = m_pPickingSys->Get_Ray().vDir;

				float t = (fDragPlaneDistance - D3DXVec3Dot(&vDragPlaneNormal, &vRayOrigin)) /
					D3DXVec3Dot(&vDragPlaneNormal, &vRayDir);

				_float3 vIntersectPoint = (vRayDir * t);
				D3DXVec3Add(&vIntersectPoint, &vRayOrigin, &vIntersectPoint);
				_float3 vNewPos;
				D3DXVec3Add(&vNewPos, &vIntersectPoint, &vDragOffset);

				// 객체 위치 업데이트
				pTransform->Set_State(CTransform::STATE_POSITION, vNewPos);
			}
		}
	}

	/// 오브젝트 픽킹 후 드래그 테스트/////////////////////////////
}

HRESULT CLevel_Editor::Render()
{
	m_pImgui->Render();
	SetWindowText(g_hWnd, TEXT("에디터 레벨입니다."));
	
	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_BackGround(const _wstring& strLayerTag)
{

	//if (FAILED(m_pGameInstance->Add_GameObject
	//(LEVEL_GAMEPLAY, 
	//	TEXT("Prototype_GameObject_Terrain"),
	//	LEVEL_GAMEPLAY, strLayerTag)))
	//	return E_FAIL;
	// 오브젝트 풀에 등록
	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Terrain"), strLayerTag, 1000)))
		return E_FAIL;

	// 오브젝트 풀링에서 가져와서 오브젝트 매니저에 추가
	if (FAILED(m_pGameInstance->Add_GameObject_FromPool(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, strLayerTag)))
		return E_FAIL;
	return S_OK;
}


HRESULT CLevel_Editor::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_EDITOR, TEXT("Prototype_GameObject_Camera_FirstPerson"),
		LEVEL_EDITOR, strLayerTag)))
		return E_FAIL;


	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_EDITOR, TEXT("Prototype_GameObject_Camera_Free"),
	//	LEVEL_EDITOR, strLayerTag)))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_EDITOR, TEXT("Prototype_GameObject_Player"),
		LEVEL_EDITOR, strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_EDITOR, TEXT("Prototype_GameObject_Player"), strLayerTag, 1)))
		return E_FAIL;



	CTransform::TRANSFORM_DESC randTransDesc{};




	randTransDesc.fRotationPerSec = D3DXToRadian(90.f);
	randTransDesc.fSpeedPerSec = 10.f;
	randTransDesc.vPos = { _float(rand() % 50),5.f,_float(rand() % 50) };
	if (FAILED(m_pGameInstance->Add_GameObject_FromPool(LEVEL_EDITOR, LEVEL_EDITOR, strLayerTag, &randTransDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_Monster(const _wstring& strLayerTag)
{

	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestMonster"), strLayerTag, 10)))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_GameObject_FromPool(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, strLayerTag)))
		return E_FAIL;

	return S_OK;
}


CLevel_Editor* CLevel_Editor::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Editor* pInstance = new CLevel_Editor(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Editor");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Editor::Free()
{
	__super::Free();
	Safe_Release(m_pImgui);
	Safe_Release(m_pPickingSys);
	CPickingSys::Destroy_Instance();
}
