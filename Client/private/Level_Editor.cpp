#include "Level_Editor.h"
#include "GameInstance.h"
#include "PickingSys.h"
#include "Collider_Sphere.h"
#include "UI_Default_Panel.h"
#include "UI_Base.h"
#include "GamePlay_Button.h"
#include "MyImGui.h"
#include "JsonLoader.h"
#include "Weapon_Base.h"

#include "Level_Loading.h"

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

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	CJsonLoader jsonLoader;
 	//jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL_Antarctic1.json", LEVEL_EDITOR);
 	jsonLoader.Load_Prototypes(m_pGameInstance, m_pGraphic_Device, L"../Save/Prototypes_For_Boss1.json");
 	jsonLoader.Load_Level(m_pGameInstance, m_pGraphic_Device, L"../Save/LEVEL_AntarcticBoss.json", LEVEL_EDITOR);

	m_pImgui = CMyImGui::Create(LEVEL_END, m_pGraphic_Device);
	if (nullptr == m_pImgui)
		return E_FAIL;

	return S_OK;
}

void CLevel_Editor::Update(_float fTimeDelta)
{


	if (m_pPickingSys)
	{

	m_pPickingSys->Update();
	static CGameObject* dragObject = nullptr;

	auto colliderVec = m_pGameInstance->Get_Colliders();
	for (auto& colliderList : colliderVec)
	{
		colliderList.sort([&] (CCollider* a, CCollider* b){

			_float3 aPos = a->Get_State(CTransform::STATE_POSITION);
			_float3 bPos = b->Get_State(CTransform::STATE_POSITION);

			_float vDis1 = _float3::Distance(aPos, m_pPickingSys->Get_Ray().vOrigin);
			_float vDis2 = _float3::Distance(bPos, m_pPickingSys->Get_Ray().vOrigin);

			return vDis1 < vDis2;
			});
	}

	if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		dragObject = nullptr;
	}
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
						//if (collider->Get_Owner()->Get_Tag() == L"Layer_Player") continue;
						dragObject = collider->Get_Owner();
						break;
					}
				}
				if (dragObject) break;
			}
		}

		if (dragObject)
		{
			m_pImgui->Set_Object(dragObject);
		}
	}
	}
}

HRESULT CLevel_Editor::Render()
{
	if(m_pImgui)
	m_pImgui->Render();
	SetWindowText(g_hWnd, TEXT("에디터 레벨입니다."));
	
	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_Camera(const _wstring& strLayerTag)
{

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
		LEVEL_EDITOR, strLayerTag)))
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
