
#include "MyImGui.h"
#include "Transform.h"
#include "Structure.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"
#include "JsonLoader.h"
#include "Editor.h"
#include <Item.h>
#include <Trigger.h>
#include <Door.h>

// 히스토리 항목 구조체
_uint CMyImGui::m_NextObjectID = 0;

CMyImGui::CMyImGui(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device{ pGraphic_Device },
	m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CMyImGui::Initialize(_uint iNumLevels, LPDIRECT3DDEVICE9 pGraphic_Device)
{

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);

	ImGui_ImplDX9_Init(pGraphic_Device);

	// ImGuizmo 초기화
	ImGuizmo::Enable(true);
	m_Editor = new CEditor;
	m_wstrSelectedTexturePath.clear();
	memset(m_szSelectedPathBuffer, 0, sizeof(m_szSelectedPathBuffer));
	m_iNumLevels = iNumLevels;


	ImGuizmo::Style& style = ImGuizmo::GetStyle();

	// 선 두께 조정
	style.TranslationLineThickness = 4.0f;
	style.RotationLineThickness = 3.0f;
	style.ScaleLineThickness = 3.0f;

	// 색상 조정 (더 눈에 잘 띄게)
	style.Colors[ImGuizmo::DIRECTION_X] = ImVec4(1.0f, 0.2f, 0.2f, 1.0f); // X축 빨간색
	style.Colors[ImGuizmo::DIRECTION_Y] = ImVec4(0.2f, 1.0f, 0.2f, 1.0f); // Y축 초록색
	style.Colors[ImGuizmo::DIRECTION_Z] = ImVec4(0.2f, 0.2f, 1.0f, 1.0f); // Z축 파란색
	style.Colors[ImGuizmo::SELECTION] = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);   //

	return S_OK;
}

void CMyImGui::Update(_float fTimeDelta)
{

}

HRESULT CMyImGui::Render()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuiIO& io = ImGui::GetIO();

	// ImGuizmo는 매 프레임 시작시 호출해야 합니다
	ImGuizmo::BeginFrame();
	Show_Objects();
	ShowPrevTextureImage();

	m_Editor->RenderUI();

	ImGui::EndFrame();
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	return S_OK;
}
void CMyImGui::Show_Objects()
{
	ImGui::Begin("Editor");

	// 입력 처리
	InputKey();

	// 탭 시스템 추가
	if (ImGui::BeginTabBar("EditorTabs"))
	{
		// 통합된 인스펙터 탭 (오브젝트 속성 + 기즈모 컨트롤)
		if (ImGui::BeginTabItem("Inspector"))
		{
			ShowInspectorTab();
			ImGui::EndTabItem();
		}

		// 오브젝트 생성 탭
		if (ImGui::BeginTabItem("Create Object"))
		{
			ShowCreateObjectTab();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}


void CMyImGui::ShowInspectorTab()
{
	// 오브젝트 선택 관련 정보
	if (!m_pCurrentGameObject)
	{
		ImGui::Text("No object selected");
		return;
	}

	// 트랜스폼 컴포넌트 가져오기
	CTransform* pTransform = (CTransform*)m_pCurrentGameObject->Get_Component(TEXT("Com_Transform"));
	string stLayerTag = ISerializable::WideToUtf8(m_pCurrentGameObject->Get_Tag());

	if (pTransform == nullptr)
	{
		ImGui::Text("Selected object has no transform component");
		return;
	}

	// 오브젝트 기본 정보
	ImGui::Text("Selected Object: %s", stLayerTag.c_str());
	ImGui::Separator();

	// Gizmo 모드 선택 버튼
	if (ImGui::RadioButton("Translate", m_CurrentGizmoOperation == ImGuizmo::TRANSLATE))
		m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", m_CurrentGizmoOperation == ImGuizmo::ROTATE))
		m_CurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", m_CurrentGizmoOperation == ImGuizmo::SCALE))
		m_CurrentGizmoOperation = ImGuizmo::SCALE;
	ImGui::Separator();
	ImGui::Text("Debug Visualization");

	// 디버그 큐브 토글 옵션
	static bool showDebugCube = true;
	ImGui::Checkbox("Show Debug Cube", &showDebugCube);


	// 선택된 오브젝트가 있고 디버그 큐브 옵션이 활성화된 경우 디버그 큐브 렌더링
	if (showDebugCube && pTransform)
	{
		ImGuizmo::Style& style = ImGuizmo::GetStyle();

		// 원래 색상 저장
		ImVec4 origDirX = style.Colors[ImGuizmo::DIRECTION_X];
		ImVec4 origDirY = style.Colors[ImGuizmo::DIRECTION_Y];
		ImVec4 origDirZ = style.Colors[ImGuizmo::DIRECTION_Z];

		// 더 연한 색상으로 설정 (알파 값 낮추고 밝기 올림)
		style.Colors[ImGuizmo::DIRECTION_X] = ImVec4(1.0f, 0.5f, 0.5f, 0.6f); // X축 (빨간색) 연하게
		style.Colors[ImGuizmo::DIRECTION_Y] = ImVec4(0.5f, 1.0f, 0.5f, 0.6f); // Y축 (초록색) 연하게
		style.Colors[ImGuizmo::DIRECTION_Z] = ImVec4(0.5f, 0.5f, 1.0f, 0.6f); // Z축 (파란색) 연하게

		// 뷰/투영 행렬 가져오기
		D3DXMATRIX d3dViewMatrix, d3dProjMatrix;
		m_pGraphic_Device->GetTransform(D3DTS_VIEW, &d3dViewMatrix);
		m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &d3dProjMatrix);

		// 오브젝트의 월드 행렬 가져오기
		_float4x4 worldMatrix = pTransform->Get_WorldMat();

		// 디버그 큐브 렌더링
		ImGuizmo::DrawCubes((float*)&d3dViewMatrix, (float*)&d3dProjMatrix, (float*)&worldMatrix, 1);

		// 원래 색상으로 복원
		style.Colors[ImGuizmo::DIRECTION_X] = origDirX;
		style.Colors[ImGuizmo::DIRECTION_Y] = origDirY;
		style.Colors[ImGuizmo::DIRECTION_Z] = origDirZ;
	}

	// Snap 설정
	ImGui::Checkbox("Use Snap", &m_bUseSnap);
	if (m_bUseSnap)
	{
		ImGui::InputFloat3("Snap Values", m_SnapValue);
	}

	ImGui::Separator();

	// Transform 속성
	ImGui::Text("Transform Properties");

	// 현재 위치, 스케일, 회전값 가져오기
	_float3 position = pTransform->Get_State(CTransform::STATE_POSITION);
	_float3 scale = pTransform->Compute_Scaled();
	_float3 euler = pTransform->Get_EulerAngles();
	bool positionChanged = false;
	bool scaleChanged = false;
	bool rotationChanged = false;

	// 위치 컨트롤
	if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushID("Position");
		ImGui::Text("X"); ImGui::SameLine();
		positionChanged |= ImGui::DragFloat("##X", &position.x, 0.1f, -FLT_MAX, FLT_MAX);

		ImGui::Text("Y"); ImGui::SameLine();
		positionChanged |= ImGui::DragFloat("##Y", &position.y, 0.1f, -FLT_MAX, FLT_MAX);

		ImGui::Text("Z"); ImGui::SameLine();
		positionChanged |= ImGui::DragFloat("##Z", &position.z, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::PopID();
	}

	// 크기 컨트롤
	if (ImGui::CollapsingHeader("Scale", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushID("Scale");
		ImGui::Text("X"); ImGui::SameLine();
		scaleChanged |= ImGui::DragFloat("##X", &scale.x, 0.1f, 0.1f, FLT_MAX);

		ImGui::Text("Y"); ImGui::SameLine();
		scaleChanged |= ImGui::DragFloat("##Y", &scale.y, 0.1f, 0.1f, FLT_MAX);

		ImGui::Text("Z"); ImGui::SameLine();
		scaleChanged |= ImGui::DragFloat("##Z", &scale.z, 0.1f, 0.1f, FLT_MAX);
		ImGui::PopID();
	}

	// 회전 컨트롤
	if (ImGui::CollapsingHeader("Rotation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushID("Rotation");
		ImGui::Text("X"); ImGui::SameLine();
		rotationChanged |= ImGui::DragFloat("##X", &euler.x, 1.0f, 0.0f, 360.0f);

		ImGui::Text("Y"); ImGui::SameLine();
		rotationChanged |= ImGui::DragFloat("##Y", &euler.y, 1.0f, 0.0f, 360.0f);

		ImGui::Text("Z"); ImGui::SameLine();
		rotationChanged |= ImGui::DragFloat("##Z", &euler.z, 1.0f, 0.0f, 360.0f);
		ImGui::PopID();
	}

	// 트랜스폼에 변경사항 적용
	if (positionChanged)
	{
		if (!m_bTrackingTransform)
			BeginTransformAction();

		pTransform->Set_State(CTransform::STATE_POSITION, position);

		if (ImGui::IsMouseReleased(0))
			EndTransformAction();
	}

	if (scaleChanged)
	{
		if (!m_bTrackingTransform)
			BeginTransformAction();
		pTransform->Set_Scale(scale.x, scale.y, scale.z);
		if (ImGui::IsMouseReleased(0))
			EndTransformAction();
	}


	if (rotationChanged)
	{
		if (!m_bTrackingTransform)
			BeginTransformAction();
		pTransform->Rotate_EulerAngles(euler);
		if (ImGui::IsMouseReleased(0))
			EndTransformAction();
	}

	// 씬 뷰에 ImGuizmo 렌더링
	RenderImGuizmo(pTransform);

	// 단축키 안내
	ImGui::Separator();
	ImGui::BulletText("1: Translate Mode");
	ImGui::BulletText("2: Rotate Mode");
	ImGui::BulletText("3: Scale Mode");

	if (ImGui::Button("Remove Current Object"))
	{
		Remove_Object();
	}
}



// 오브젝트 생성 탭 내용
void CMyImGui::ShowCreateObjectTab()
{
	static int selectedColliderType = 0;
	static int selectedBufferType = 0;
	static int selectedClassType = 0;
	static int selectedItemType = 0;
	static char colliderNameBuffer[256] = "Prototype_Component_Collider_Cube";
	static char bufferNameBuffer[256] = "Prototype_Component_VIBuffer_Rect";
	static char layerTagBuffer[128] = "Com_Collider";
	static char objectLayerTagBuffer[256] = "Layer_";
	static char textureTagBuffer[256] = "Prototype_Component_Texture";
	static char objectClassTagBuffer[256] = "C";
	static char objectProtoTagBuffer[256] = "Prototype_GameObject_";
	static char protoJsonFileNameBuffer[256] = "Prototypes_For_Editor";
	static _int iPoolingCount = 0;
	static _int iLevel = 3;
	static _int iProtoLevel = 4;

	// 기존 프로토타입 변수 추가
	static bool bPrototypesLoaded = false;
	static vector<PrototypeInfo> prototypeInfos;
	static vector<const char*> prototypeNames;
	static int selectedPrototypeIndex = -1;

	// JSON에서 로드한 클래스 이름 목록
	static _bool bClassNamesLoaded = false;
	static vector<string> classNames;
	static vector<const char*> classNamesCStr;

	// JSON 파일에서 클래스 이름 로드 (최초 한 번만 실행)
	if (!bClassNamesLoaded)
	{
		CJsonLoader jsonLoader;
		// JSON 파일 로드
		if (SUCCEEDED(jsonLoader.LoadClassNamesFromJson("../Save/ClassNames.json", classNames)))
		{
			classNamesCStr.clear();
			for (const auto& name : classNames)
			{
				classNamesCStr.push_back(name.c_str());
			}
			bClassNamesLoaded = true;
		}
		else
		{
			// 기본 클래스 이름 설정
			classNames = {
				"CGameObject_Cube", "CGameObject_Plane", "CGameObject_Light","CPlayer", "CTestMonster", "CTerrain", "CStructure",
				"CCamera_Free", "CCamera_FirstPerson",
				"CUI_Default_Panel", "CUI_Left_Display", "CUI_Player_Icon",
				"CUI_HP_Bar", "CUI_MP_Bar", "CUI_Mid_Display",
				"CUI_Right_Display", "CUI_Bullet_Bar", "CUI_Menu",
				"CGamePlay_Button", "CUI_Spell_Shop",
				"CAxe", "CClaymore", "CMagnum", "CStaff", "CShotGun"
			};

			classNamesCStr.clear();
			for (const auto& name : classNames)
			{
				classNamesCStr.push_back(name.c_str());
			}
			bClassNamesLoaded = true;
		}
	}

	// 프로토타입 목록 로드 (최초 한 번만 실행)
	if (!bPrototypesLoaded)
	{
		if (SUCCEEDED(LoadPrototypesFromJson("Prototypes_For_Editor", prototypeInfos)))
		{
			prototypeNames.clear();
			for (const auto& info : prototypeInfos)
			{
				prototypeNames.push_back(info.tag.c_str());
			}
			bPrototypesLoaded = true;
		}
	}


	ImGui::SameLine();
	ImGui::Checkbox("Item Create", &m_bCreateItem);

	if (m_bCreateItem)
	{
		ShowItemCreationTab();
	}

	ImGui::SameLine();
	ImGui::Checkbox("Trigger Create", &m_bCreateTrigger);

	if (m_bCreateTrigger)
	{
		ShowTriggerTab();
	}

	ImGui::SameLine();
	ImGui::Checkbox("Door Create", &m_bCreateDoor);
	if (m_bCreateDoor)
	{
		ShowDoorTab();
	}


	// 컴포넌트 타입 선택 UI 앞에 추가
	ImGui::Separator();
	ImGui::Text("Use Existing Prototype:");

	// 프로토타입 콤보박스
	if (ImGui::Combo("Prototypes", &selectedPrototypeIndex, prototypeNames.data(), static_cast<int>(prototypeNames.size())))
	{
		if (selectedPrototypeIndex >= 0 && selectedPrototypeIndex < prototypeInfos.size())
		{
			// 선택된 프로토타입 정보로 UI 필드 업데이트
			const auto& info = prototypeInfos[selectedPrototypeIndex];

			// 클래스 타입 업데이트
			for (size_t i = 0; i < classNames.size(); ++i)
			{
				if (classNames[i] == info.className)
				{
					selectedClassType = static_cast<int>(i);
					strcpy_s(objectClassTagBuffer, info.className.c_str());
					break;
				}
			}

			// 프로토타입 태그
			strcpy_s(objectProtoTagBuffer, info.tag.c_str());

			// 텍스처 태그
			strcpy_s(textureTagBuffer, info.textureTag.c_str());

			// 버퍼 타입 업데이트 - 수정된 부분
			if (info.bufferTag == "Prototype_Component_VIBuffer_Cube")
			{
				selectedBufferType = 0;
				strcpy_s(bufferNameBuffer, "Prototype_Component_VIBuffer_Cube");
			}
			else if (info.bufferTag == "Prototype_Component_VIBuffer_Rect")
			{
				selectedBufferType = 1;
				strcpy_s(bufferNameBuffer, "Prototype_Component_VIBuffer_Rect");
			}
			else if (info.bufferTag == "Prototype_Component_VIBuffer_Terrain")
			{
				selectedBufferType = 2;
				strcpy_s(bufferNameBuffer, "Prototype_Component_VIBuffer_Terrain");
			}

			string protoTag = info.tag;
			string extractedName = "";
			string prefix = "Prototype_GameObject_";
			if (protoTag.find(prefix) == 0)
			{
				extractedName = protoTag.substr(prefix.length());
			}
			else
			{
				// 접두사가 없는 경우 그대로 사용
				extractedName = protoTag;
			}

			// Layer_ 접두사와 추출된 이름을 합쳐서 레이어 태그 생성
			string layerTag = "Layer_" + extractedName;
			strcpy_s(objectLayerTagBuffer, layerTag.c_str());


			// 레벨 업데이트
			iLevel = 3; // 기본값
			iProtoLevel = info.level;

			// 선택된 텍스처 경로 업데이트
			m_wstrSelectedTexturePath = ISerializable::Utf8ToWide(info.texturePath);
			strcpy_s(m_szSelectedPathBuffer, info.texturePath.c_str());

			// 텍스처 미리보기 로드
			if (!m_wstrSelectedTexturePath.empty())
			{
				LoadImagesFromFolder(m_wstrSelectedTexturePath);
				m_bShowImageWindow = true;
			}
		}
	}

	//// 프로토타입으로부터 게임 오브젝트 생성 버튼
	//if (selectedPrototypeIndex >= 0 && ImGui::Button("Create From Selected Prototype"))
	//{
	//	if (selectedPrototypeIndex < prototypeInfos.size())
	//	{
	//		const auto& info = prototypeInfos[selectedPrototypeIndex];
	//		_wstring protoTag = ISerializable::Utf8ToWide(info.tag);
	//		_wstring layerTag = ISerializable::Utf8ToWide(objectLayerTagBuffer);

	//		CGameObject::OBJECT_DESC tObjDesc{};
	//		tObjDesc.iLevel = iLevel;
	//		tObjDesc.iProtoLevel = info.level;
	//		tObjDesc.stProtTag = protoTag;
	//		tObjDesc.stBufferTag = ISerializable::Utf8ToWide(info.bufferTag);
	//		tObjDesc.stProtTextureTag = ISerializable::Utf8ToWide(info.textureTag);
	//		tObjDesc.iPoolCount = iPoolingCount;

	//		// 게임 오브젝트 추가
	//		if (iPoolingCount != 0)
	//		{
	//			m_pGameInstance->Reserve_Pool(info.level, protoTag, layerTag, iPoolingCount, &tObjDesc);
	//		}
	//		else
	//		{
	//			m_pGameInstance->Add_GameObject(info.level, protoTag, iLevel, layerTag, &tObjDesc);
	//		}
	//	}
	//}

	// 프로토타입 목록 새로고침 버튼
	if (ImGui::Button("Refresh Prototypes"))
	{
		bPrototypesLoaded = false;  // 다음 프레임에 재로드하도록 설정
	}

	ImGui::Separator();

	// 선택된 클래스 이름으로 업데이트
	strcpy_s(objectClassTagBuffer, classNames[selectedClassType].c_str());

	// 충돌체 타입 배열
	static const char* colliderTypes[] = { "Cube", "Sphere" };
	static const char* colliderProtoNames[] = {
		"Prototype_Component_Collider_Cube",
		"Prototype_Component_Collider_Sphere"
	};

	// 버퍼 타입 배열
	static const char* bufferTypes[] = { "Cube", "Rect","Terrain" };
	static const char* bufferProtoNames[] = {
		"Prototype_Component_VIBuffer_Cube",
		"Prototype_Component_VIBuffer_Rect",
		"Prototype_Component_VIBuffer_Terrain"
	};

	static const char* ItemTypes[] = { "HP", "MP","Ammo","Stat","EXP" };
	// UI 요소들 배치
	if (ImGui::Combo("Collider Type", &selectedColliderType, colliderTypes, IM_ARRAYSIZE(colliderTypes)))
	{
		strcpy_s(colliderNameBuffer, colliderProtoNames[selectedColliderType]);
	}

	if (ImGui::Combo("Buffer Type", &selectedBufferType, bufferTypes, IM_ARRAYSIZE(bufferTypes)))
	{
		strcpy_s(bufferNameBuffer, bufferProtoNames[selectedBufferType]);
	}

	if (ImGui::Combo("Class Type", &selectedClassType, classNamesCStr.data(), static_cast<int>(classNamesCStr.size())))
	{
		strcpy_s(objectClassTagBuffer, classNames[selectedClassType].c_str());
	}



	ImGui::InputText("Collider Tag", colliderNameBuffer, IM_ARRAYSIZE(colliderNameBuffer));
	ImGui::InputText("Component Layer Tag", layerTagBuffer, IM_ARRAYSIZE(layerTagBuffer));
	ImGui::InputText("Object Proto Layer Tag", objectProtoTagBuffer, IM_ARRAYSIZE(objectProtoTagBuffer));
	ImGui::InputText("Object Layer Tag", objectLayerTagBuffer, IM_ARRAYSIZE(objectLayerTagBuffer));
	ImGui::InputText("Texture Tag", textureTagBuffer, IM_ARRAYSIZE(textureTagBuffer));
	ImGui::InputInt("Pool Count", &iPoolingCount);
	ImGui::InputInt("Level", &iLevel);
	ImGui::InputInt("ProtoLevel", &iProtoLevel);
	ImGui::InputText("JSON Prototypes File Name", protoJsonFileNameBuffer, IM_ARRAYSIZE(protoJsonFileNameBuffer));
	static _wstring selectedFolder;
	static char selectedPathBuffer[512] = { 0 }; // 선택된 경로를 표시할 버퍼


	// 이미 선택된 경로가 있으면 버퍼에 표시
	if (selectedFolder.length() > 0 && selectedPathBuffer[0] == 0) {
		strcpy_s(selectedPathBuffer, ISerializable::WideToUtf8(selectedFolder).c_str());
	}

	// 파일 선택 버튼
	if (ImGui::Button("Select Texture"))
	{
		selectedFolder = SelectFile();
		if (!selectedFolder.empty())
		{
			selectedFolder = GetRelativePath(selectedFolder);
			strcpy_s(selectedPathBuffer, ISerializable::WideToUtf8(selectedFolder).c_str());

			// 이미지 로드
			LoadImagesFromFolder(selectedFolder.substr(0, selectedFolder.find_last_of(L"\\")));
			m_bShowImageWindow = true;
		}
	}

	// 선택된 파일 경로 표시
	ImGui::SameLine();
	ImGui::InputText("Selected Path", m_szSelectedPathBuffer, IM_ARRAYSIZE(m_szSelectedPathBuffer), ImGuiInputTextFlags_ReadOnly);

	// 이미지 미리보기 섹션
	ImGui::Separator();
	ImGui::Text("Selected Texture Preview:");

	// 텍스처가 로드되었는지 확인
	if (!m_Textures.empty())
	{
		// ImGui에서 텍스처 ID 사용하기 위한 처리
		ImTextureID texID = (ImTextureID)m_Textures[m_SelectedTextureIndex];

		// 이미지 정보 가져오기
		D3DSURFACE_DESC desc;
		if (SUCCEEDED(m_Textures[m_SelectedTextureIndex]->GetLevelDesc(0, &desc)))
		{
			// 이미지 원본 크기
			float originalWidth = (float)desc.Width;
			float originalHeight = (float)desc.Height;

			// 미리보기 크기 (적절히 조절)
			float previewWidth = 200.0f;
			float previewHeight = previewWidth * (originalHeight / originalWidth);

			// 이미지 미리보기 표시
			ImGui::Image(texID, ImVec2(previewWidth, previewHeight));

			// 이미지 정보 표시
			ImGui::Text("Size: %dx%d", desc.Width, desc.Height);
			ImGui::Text("Format: %d", desc.Format);
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Failed to get texture info");
		}
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "No texture selected or failed to load");
	}

	// 생성 버튼 및 로직
	if (ImGui::Button("Create"))
	{
		_wstring texturePath = selectedFolder;

		// 선택된 텍스처가 있으면 해당 텍스처 경로 사용
		if (!m_wstrSelectedTexturePath.empty())
		{
			texturePath = m_wstrSelectedTexturePath;
		}

		CreateObjectInstance(
			protoJsonFileNameBuffer,
			bufferNameBuffer, colliderNameBuffer, layerTagBuffer,
			textureTagBuffer, objectClassTagBuffer, objectProtoTagBuffer,
			objectLayerTagBuffer, iPoolingCount, iLevel, iProtoLevel, texturePath);
	}
}

HRESULT CMyImGui::CreateObjectInstance(
	const char* jsonFileNameBuffer,
	const char* bufferNameBuffer, const char* colliderNameBuffer,
	const char* layerTagBuffer, const char* textureTagBuffer,
	const char* objectClassTagBuffer, const char* objectProtoTagBuffer,
	const char* objectLayerTagBuffer, _int iPoolingCount,
	_int iLevel, _int iProtoLevel, const _wstring& selectedFolder)
{
	wchar_t wBufferName[256] = {};
	wchar_t wColliderName[256] = {};
	wchar_t wLayerTag[128] = {};
	wchar_t wtextureTag[256] = {};
	wchar_t wobjectClassTag[256] = {};
	wchar_t wobjectProtoTagBuffer[256] = {};
	wchar_t wobjectLayerTagBuffer[256] = {};
	wchar_t wprotoJsonFileNameBuffer[256] = {};


	// 문자열 변환
	MultiByteToWideChar(CP_ACP, 0, bufferNameBuffer, -1, wBufferName, 256);
	MultiByteToWideChar(CP_ACP, 0, colliderNameBuffer, -1, wColliderName, 256);
	MultiByteToWideChar(CP_ACP, 0, layerTagBuffer, -1, wLayerTag, 128);
	MultiByteToWideChar(CP_ACP, 0, textureTagBuffer, -1, wtextureTag, 256);
	MultiByteToWideChar(CP_ACP, 0, objectClassTagBuffer, -1, wobjectClassTag, 256);
	MultiByteToWideChar(CP_ACP, 0, objectProtoTagBuffer, -1, wobjectProtoTagBuffer, 256);
	MultiByteToWideChar(CP_ACP, 0, objectLayerTagBuffer, -1, wobjectLayerTagBuffer, 256);
	MultiByteToWideChar(CP_ACP, 0, jsonFileNameBuffer, -1, wprotoJsonFileNameBuffer, 256);

	CJsonLoader jsonLoader;
	CBase* pGameObject = jsonLoader.Create_Object_ByClassName(ISerializable::WideToUtf8(wobjectClassTag), m_pGraphic_Device);

	_wstring stProtoTag = ISerializable::Utf8ToWide(ISerializable::WideToUtf8(wobjectProtoTagBuffer));
	_wstring stLayerTag = ISerializable::Utf8ToWide(ISerializable::WideToUtf8(wobjectLayerTagBuffer));
	_wstring stClassName = ISerializable::Utf8ToWide(ISerializable::WideToUtf8(wobjectClassTag));
	_wstring stTextureTag = ISerializable::Utf8ToWide(ISerializable::WideToUtf8(wtextureTag));
	_wstring stBufferTag = ISerializable::Utf8ToWide(ISerializable::WideToUtf8(wBufferName));

	CGameObject::OBJECT_DESC tObjDesc{};
	tObjDesc.iLevel = iLevel;
	tObjDesc.iProtoLevel = iProtoLevel;
	tObjDesc.stProtTag = stProtoTag;
	tObjDesc.stBufferTag = wBufferName;
	tObjDesc.stProtTextureTag = stTextureTag;
	tObjDesc.iPoolCount = iPoolingCount;

	bool textureCreated = false;
	if (FAILED(m_pGameInstance->Find_Prototype(stTextureTag)))
	{
		if (FAILED(m_pGameInstance->Add_Prototype(iLevel, stTextureTag,
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, ISerializable::WStringToWChar(selectedFolder), 1))))
		{
			return E_FAIL;
		}
		textureCreated = true;
	}
	bool prototypeCreated = false;
	if (FAILED(m_pGameInstance->Find_Prototype(stProtoTag)))
	{
		if (FAILED(m_pGameInstance->Add_Prototype(iProtoLevel, stProtoTag, pGameObject)))
		{
			Safe_Release(pGameObject);
			return E_FAIL;
		}
		prototypeCreated = true;
	}
	else
	{
		Safe_Release(pGameObject);
	}

	// 게임 오브젝트 추가
	if (iPoolingCount != 0)
	{
		if (FAILED(m_pGameInstance->Reserve_Pool(iProtoLevel, stProtoTag,
			stLayerTag, iPoolingCount, &tObjDesc)))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pGameInstance->Add_GameObject(iProtoLevel, stProtoTag,
			iLevel, stLayerTag, &tObjDesc)))
		{
			return E_FAIL;
		}

		tHistoryItem item;
		item.iLevel = iLevel;
		item.iProtoLevel = iProtoLevel;
		item.eType = EHistoryActionType::OBJECT_CREATE;
		item.pGameObject = m_pGameInstance->Find_Last_Object(iLevel, stLayerTag);
		item.wstrPrototypeTag = stProtoTag;
		item.wstrLayerTag = stLayerTag;
		item.tObjDesc = &tObjDesc;
		AddToHistory(item);
	}


	if (textureCreated || prototypeCreated)
	{
		// 텍스처 파일이 선택되었으면 해당 정보 저장
		int textureCount = 1; // 기본값

		// 버퍼 정보 (지금은 기본값으로, 특별히 설정할 수 있는 UI가 있다면 그걸 사용)
		int bufferWidth = 0;
		int bufferHeight = 0;

		// 특별한 경우에 버퍼 크기 설정 (예: 지형)
		if (strcmp(bufferNameBuffer, "Prototype_Component_VIBuffer_Terrain") == 0)
		{
			bufferWidth = 256;  // 기본값, 필요하면 UI에서 입력받아 수정
			bufferHeight = 256;
		}

		// JSON에 저장
		SaveObjectToJson(
			ISerializable::WideToUtf8(wprotoJsonFileNameBuffer),
			stProtoTag,         // 오브젝트 태그
			iProtoLevel,        // 오브젝트 레벨
			stClassName,        // 클래스 이름
			stTextureTag,       // 텍스처 태그
			iLevel,             // 텍스처 레벨
			selectedFolder,     // 텍스처 경로
			textureCount,       // 텍스처 개수
			stBufferTag,        // 버퍼 태그
			3,                  // 버퍼 레벨 (기본값)
			L"CVIBuffer_" + stBufferTag.substr(stBufferTag.find_last_of(L"_") + 1), // 버퍼 클래스 이름
			bufferWidth,        // 버퍼 너비
			bufferHeight        // 버퍼 높이
		);
	}

	return S_OK;
}

HRESULT CMyImGui::SaveObjectToJson(const string& jsonFileName, const _wstring& objectProtoTag, _int objectLevel, const _wstring& className, const _wstring& textureTag, _int textureLevel, const _wstring& texturePath, _int textureCount, const _wstring& bufferTag, _int bufferLevel, const _wstring& bufferClass, _int bufferWidth, _int bufferHeight)
{

	// JSON 객체 생성
	json jsonData;
	string JSON_FILE_PATH = "../Save/";

	if (jsonFileName.empty())
	{
		JSON_FILE_PATH += "Prototypes_For_Editor.json";
	}
	else
	{
		if (jsonFileName.find(".json") == string::npos)
		{
			JSON_FILE_PATH += jsonFileName + ".json";
		}
		else
		{
			JSON_FILE_PATH += jsonFileName;
		}
	}
	// 파일이 존재하는지 확인
	ifstream inputFile(JSON_FILE_PATH);
	if (inputFile.is_open()) {
		// 기존 파일이 있으면 내용 로드
		try {
			inputFile >> jsonData;
		}
		catch (const json::parse_error&) {

			// 파싱 에러가 발생하면 새 JSON 객체 생성
			OutputDebugStringA("JSON 파일 파싱 에러, 새 파일을 생성합니다.\n");
			jsonData = json({
				{"textures", json::array()},
				{"gameObjects", json::array()},
				{"buffers", json::array()}
				});
		}
		inputFile.close();
	}
	else {
		// 파일이 없으면 기본 구조 생성
		jsonData = json({
			{"textures", json::array()},
			{"gameObjects", json::array()},
			{"buffers", json::array()}
			});
	}

	// 문자열 변환 함수
	auto WStringToString = [](const _wstring& wstr) -> string {
		if (wstr.empty()) return "";
		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
		if (bufferSize == 0) return "";
		string str(bufferSize, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], bufferSize, nullptr, nullptr);
		str.resize(strlen(str.c_str())); // null 문자 제거
		return str;
		};

	// 텍스처 정보 추가 (없는 경우에만)
	if (!texturePath.empty() && !textureTag.empty()) {
		string strTextureTag = WStringToString(textureTag);
		string strTexturePath = WStringToString(texturePath);

		// 이미 존재하는지 확인
		bool textureExists = false;
		for (auto& texture : jsonData["textures"]) {
			if (texture["tag"] == strTextureTag) {
				textureExists = true;
				break;
			}
		}

		// 존재하지 않으면 추가
		if (!textureExists) {
			jsonData["textures"].push_back({
				{"tag", strTextureTag},
				{"level", textureLevel},
				{"path", strTexturePath},
				{"count", textureCount}
				});
		}
	}

	// 게임 오브젝트 정보 추가 (없는 경우에만)
	if (!objectProtoTag.empty() && !className.empty()) {
		string strObjectTag = WStringToString(objectProtoTag);
		string strClassName = WStringToString(className);

		// 이미 존재하는지 확인
		bool objectExists = false;
		for (auto& gameObject : jsonData["gameObjects"]) {
			if (gameObject["tag"] == strObjectTag) {
				objectExists = true;
				break;
			}
		}

		// 존재하지 않으면 추가
		if (!objectExists) {
			jsonData["gameObjects"].push_back({
				{"tag", strObjectTag},
				{"level", objectLevel},
				{"class", strClassName}
				});
		}
	}

	// 버퍼 정보 추가 (없는 경우에만)
	if (!bufferTag.empty() && !bufferClass.empty()) {
		string strBufferTag = WStringToString(bufferTag);
		string strBufferClass = WStringToString(bufferClass);

		// 이미 존재하는지 확인
		bool bufferExists = false;
		for (auto& buffer : jsonData["buffers"]) {
			if (buffer["tag"] == strBufferTag) {
				bufferExists = true;
				break;
			}
		}

		// 존재하지 않으면 추가
		if (!bufferExists) {
			jsonData["buffers"].push_back({
				{"tag", strBufferTag},
				{"level", bufferLevel},
				{"class", strBufferClass},
				{"width", bufferWidth},
				{"height", bufferHeight}
				});
		}
	}

	// JSON 파일 저장
	ofstream outputFile(JSON_FILE_PATH);
	if (!outputFile.is_open()) {
		OutputDebugStringA("JSON 파일을 저장할 수 없습니다.\n");
		return E_FAIL;
	}

	outputFile << jsonData.dump(2); // 2는 들여쓰기 공백 수
	outputFile.close();

	OutputDebugStringA("객체 정보가 JSON 파일에 저장되었습니다.\n");
	return S_OK;
}

void CMyImGui::Remove_Object()
{
	ImGui::Separator();
	static _int iLevel = 3;
	ImGui::InputInt("Level", &iLevel);
	if (m_pCurrentGameObject)
	{
		tHistoryItem item;
		item.iLevel = m_pCurrentGameObject->m_tObjDesc.iLevel;
		item.eType = EHistoryActionType::OBJECT_DELETE;
		item.wstrLayerTag = m_pCurrentGameObject->Get_Tag();
		item.wstrPrototypeTag = m_pCurrentGameObject->m_tObjDesc.stProtTag;
		item.iProtoLevel = m_pCurrentGameObject->m_tObjDesc.iProtoLevel;
		item.pGameObject = nullptr;

		// 이것도 누수 잡아야해서 일단 막음
		//CGameObject::OBJECT_DESC* pObjDesc = new CGameObject::OBJECT_DESC;
		//*pObjDesc = m_pCurrentGameObject->m_tObjDesc;  // 값 복사
		//item.tObjDesc = pObjDesc;  // void* 포인터에 저장

		CTransform* pTransform = (CTransform*)m_pCurrentGameObject->Get_Component(TEXT("Com_Transform"));
		if (pTransform)
		{
			item.vOldPosition = pTransform->Get_State(CTransform::STATE_POSITION);
			item.vOldScale = pTransform->Compute_Scaled();
			item.vOldRotation = pTransform->Get_EulerAngles();
		}
		// 선택된 오브젝트 삭제
		m_pGameInstance->Remove_Object((_uint)iLevel, m_pCurrentGameObject->Get_Tag(), m_pCurrentGameObject);
		m_pCurrentGameObject = nullptr;


		AddToHistory(item);
	}

}

void CMyImGui::Duplicate_Object()
{
	// 누수 나서 일단 막음
	//if (m_pCurrentGameObject)
	//{
	//	m_pGameInstance->Add_GameObject(m_pCurrentGameObject->m_tObjDesc.iProtoLevel, m_pCurrentGameObject->m_tObjDesc.stProtTag, m_pCurrentGameObject->m_tObjDesc.iLevel, m_pCurrentGameObject->Get_Tag(),
	//		&m_pCurrentGameObject->m_tObjDesc);
	//	CGameObject* pDuplicateObject = m_pGameInstance->Find_Last_Object(m_pCurrentGameObject->m_tObjDesc.iLevel, m_pCurrentGameObject->Get_Tag());

	//	CTransform* pTransform = (CTransform*)m_pCurrentGameObject->Get_Component(TEXT("Com_Transform"));

	//	if (pDuplicateObject && pTransform)
	//	{
	//		CTransform* pDuplicateTransform = (CTransform*)pDuplicateObject->Get_Component(TEXT("Com_Transform"));
	//		if (pDuplicateTransform)
	//		{
	//			_float3 vScale = pTransform->Compute_Scaled();
	//			pDuplicateTransform->Set_State(CTransform::STATE_POSITION, pTransform->Get_State(CTransform::STATE_POSITION));
	//			pDuplicateTransform->Set_Scale(vScale.x, vScale.y, vScale.z);
	//			pDuplicateTransform->Rotate_EulerAngles(pTransform->Get_EulerAngles());



	//			tHistoryItem item;
	//			item.iLevel = pDuplicateObject->m_tObjDesc.iLevel;
	//			item.iProtoLevel = pDuplicateObject->m_tObjDesc.iProtoLevel;
	//			item.eType = EHistoryActionType::OBJECT_CREATE;
	//			item.pGameObject = pDuplicateObject;
	//			item.wstrPrototypeTag = pDuplicateObject->m_tObjDesc.stProtTag;
	//			item.wstrLayerTag = pDuplicateObject->Get_Tag();
	//			CGameObject::OBJECT_DESC* pObjDesc = new CGameObject::OBJECT_DESC;
	//			*pObjDesc = m_pCurrentGameObject->m_tObjDesc;  // 값 복사
	//			item.tObjDesc = pObjDesc;
	//			AddToHistory(item);
	//		}
	//	}

	//}
}

CGameObject* CMyImGui::Find_Object_ByAddress(CGameObject* pAddress)
{
	if (!pAddress)
		return nullptr;

	// 각 레벨마다 모든 레이어를 순회하며 검색
	for (_uint iLevel = 0; iLevel < m_iNumLevels; ++iLevel)
	{
		// 레벨의 모든 레이어 가져오기 (구현에 따라 코드 조정 필요)
		// NOTE: 실제 GameInstance 구현에 맞게 수정해야 합니다!
		auto& layers = m_pGameInstance->m_pObject_Manager->m_pLayers[iLevel];

		for (auto& layerPair : layers)
		{
			auto& layer = layerPair.second;
			for (auto& pObj : layer->m_GameObjects)
			{
				if (pObj == pAddress)
					return pObj; // 주소값이 일치하면 반환
			}
		}
	}

	return nullptr; // 찾지 못함
}

HRESULT CMyImGui::LoadPrototypesFromJson(const string& jsonFileName, vector<PrototypeInfo>& outPrototypes)
{

	string JSON_FILE_PATH = "../Save/";
	if (jsonFileName.empty())
	{
		JSON_FILE_PATH += "Prototypes_For_Editor.json";
	}
	else
	{
		if (jsonFileName.find(".json") == string::npos)
		{
			JSON_FILE_PATH += jsonFileName + ".json";
		}
		else
		{
			JSON_FILE_PATH += jsonFileName;
		}
	}

	// 파일 열기
	ifstream inputFile(JSON_FILE_PATH);
	if (!inputFile.is_open())
	{
		return E_FAIL;
	}

	// JSON 파싱
	json jsonData;
	try
	{
		inputFile >> jsonData;
	}
	catch (const json::parse_error&)
	{
		inputFile.close();
		return E_FAIL;
	}
	inputFile.close();

	outPrototypes.clear();

	// 프로토타입 정보 추출
	if (jsonData.contains("gameObjects") && jsonData["gameObjects"].is_array())
	{
		for (const auto& obj : jsonData["gameObjects"])
		{
			PrototypeInfo info;
			info.tag = obj["tag"].get<string>();
			info.level = obj["level"].get<int>();
			info.className = obj["class"].get<string>();
			// 태그에서 기본 이름 추출 (예: "Prototype_GameObject_Wall" -> "Wall", "Prototype_GameObject_Hub_Floor1" -> "Hub_Floor1")
			string objBaseName = info.tag;
			string prefix = "Prototype_GameObject_";
			size_t prefixPos = objBaseName.find(prefix);
			if (prefixPos == 0 && objBaseName.length() > prefix.length())
			{
				objBaseName = objBaseName.substr(prefix.length());
			}

			// 관련 텍스처 찾기 (이름 패턴 매칭)
			if (jsonData.contains("textures") && jsonData["textures"].is_array())
			{
				bool textureFound = false;

				// 정확한 이름 매칭 (예: "Prototype_Component_Texture_Wall" 또는 "Prototype_Component_Texture_Hub_Floor1")
				string specificTextureName = "Prototype_Component_Texture_" + objBaseName;

				for (const auto& tex : jsonData["textures"])
				{
					string texTag = tex["tag"].get<string>();

					// 정확한 이름 매칭 시도
					if (texTag == specificTextureName)
					{
						info.textureTag = texTag;
						info.texturePath = tex["path"].get<string>();
						info.textureCount = tex["count"].get<int>();
						textureFound = true;
						break;
					}
				}

				// 정확한 매칭 실패 시 기본 텍스처 사용
				if (!textureFound && !jsonData["textures"].empty())
				{
					const auto& defaultTex = jsonData["textures"][0];
					info.textureTag = defaultTex["tag"].get<string>();
					info.texturePath = defaultTex["path"].get<string>();
					info.textureCount = defaultTex["count"].get<int>();
				}
			}

			// 버퍼 태그는 클래스에 따라 기본값 설정
			if (info.className == "CTerrain")
			{
				info.bufferTag = "Prototype_Component_VIBuffer_Terrain";
			}
			else
			{
				info.bufferTag = "Prototype_Component_VIBuffer_Cube"; // 기본값
			}

			outPrototypes.push_back(info);
		}
	}

	return S_OK;
}

void CMyImGui::LoadImagesFromFolder(const _wstring& folderPath)
{
	// 기존 텍스처 해제
	for (auto& texture : m_Textures)
	{
		if (texture)
			texture->Release();
	}
	m_Textures.clear();

	// 경로가 폴더인지 파일인지 확인
	DWORD fileAttributes = GetFileAttributesW(folderPath.c_str());

	if (fileAttributes == INVALID_FILE_ATTRIBUTES)
	{
		// 경로가 존재하지 않음
		return;
	}

	if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		// 폴더인 경우, 해당 폴더 내 이미지 파일들 로드
		LoadImagesFromDirectoryPath(folderPath);
	}
	else
	{
		// 단일 파일인 경우, 해당 파일만 로드
		LoadSingleImageFile(folderPath);
	}
}
// 단일 이미지 파일 로드
void CMyImGui::LoadSingleImageFile(const _wstring& filePath)
{
	m_TextureFilePaths.clear();

	// 이미지 파일 확장자 확인
	_wstring extension = filePath.substr(filePath.find_last_of(L"."));
	transform(extension.begin(), extension.end(), extension.begin(), ::towlower);

	vector<_wstring> imageExtensions = { L".png", L".jpg", L".jpeg", L".bmp", L".dds", L".tga" };

	bool isImageFile = false;
	for (const auto& ext : imageExtensions)
	{
		if (extension == ext)
		{
			isImageFile = true;
			break;
		}
	}

	if (isImageFile)
	{
		// 텍스처 로드
		LPDIRECT3DTEXTURE9 texture = nullptr;
		HRESULT hr = D3DXCreateTextureFromFileW(
			m_pGraphic_Device,
			filePath.c_str(),
			&texture);

		if (SUCCEEDED(hr))
		{
			m_Textures.push_back(texture);
			m_TextureFilePaths.push_back(filePath); // 파일 경로 저장
		}
	}

	// 선택된 인덱스 초기화
	m_SelectedTextureIndex = 0;
}

// 폴더 내 모든 이미지 파일 로드
void CMyImGui::LoadImagesFromDirectoryPath(const _wstring& folderPath)
{
	m_TextureFilePaths.clear();

	// 이미지 파일 확장자 목록
	vector<_wstring> imageExtensions = { L".png", L".jpg", L".jpeg", L".bmp", L".dds", L".tga" };

	// 폴더 내 파일 검색 패턴
	wstring searchPattern = folderPath + L"\\*.*";

	WIN32_FIND_DATAW findData;
	HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &findData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			// 폴더는 건너뜁니다
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			_wstring fileName = findData.cFileName;
			_wstring extension = fileName.substr(fileName.find_last_of(L"."));

			// 확장자 소문자로 변환
			transform(extension.begin(), extension.end(), extension.begin(), ::towlower);

			// 이미지 파일 확장자인지 확인
			bool isImageFile = false;
			for (const auto& ext : imageExtensions)
			{
				if (extension == ext)
				{
					isImageFile = true;
					break;
				}
			}

			if (isImageFile)
			{
				// 전체 파일 경로 생성
				_wstring filePath = folderPath + L"\\" + fileName;

				// 텍스처 로드
				LPDIRECT3DTEXTURE9 texture = nullptr;
				HRESULT hr = D3DXCreateTextureFromFileW(
					m_pGraphic_Device,
					filePath.c_str(),
					&texture);

				if (SUCCEEDED(hr))
				{
					m_Textures.push_back(texture);
					m_TextureFilePaths.push_back(filePath); // 파일 경로 저장
				}
			}
		} while (FindNextFileW(hFind, &findData) != 0);

		FindClose(hFind);
	}

	// 선택된 인덱스 초기화
	m_SelectedTextureIndex = 0;
}

void CMyImGui::ShowPrevTextureImage()
{
	if (m_bShowImageWindow && !m_Textures.empty())
	{
		ImGui::Begin("Texture Preview", &m_bShowImageWindow);
		// 현재 선택된 텍스처 표시
		if (m_SelectedTextureIndex < m_Textures.size())
		{
			// 텍스처 정보
			D3DSURFACE_DESC desc;
			if (SUCCEEDED(m_Textures[m_SelectedTextureIndex]->GetLevelDesc(0, &desc)))
			{
				// 이미지 원본 크기
				float originalWidth = (float)desc.Width;
				float originalHeight = (float)desc.Height;
				// 윈도우 크기에 맞게 이미지 크기 조절
				ImVec2 contentSize = ImGui::GetContentRegionAvail();
				float scale = min(contentSize.x / originalWidth, contentSize.y / originalHeight);
				float displayWidth = originalWidth * scale;
				float displayHeight = originalHeight * scale;
				// 이미지 미리보기 표시
				ImTextureID texID = (ImTextureID)m_Textures[m_SelectedTextureIndex];
				ImGui::Image(texID, ImVec2(200.f, 200.f));
				// 이미지 정보 표시
				ImGui::Text("Size: %dx%d", desc.Width, desc.Height);
				// 선택된 파일 경로 표시
				if (m_SelectedTextureIndex < m_TextureFilePaths.size())
				{
					ImGui::Text("File: %s", ISerializable::WideToUtf8(m_TextureFilePaths[m_SelectedTextureIndex]).c_str());
				}
			}
		}
		// 여러 이미지가 로드된 경우 (폴더에서 로드한 경우)
		if (m_Textures.size() > 1)
		{
			ImGui::Separator();
			ImGui::Text("All loaded textures (click to select):");
			// 썸네일 표시
			float thumbnailSize = 50.0f;
			int itemsPerRow = max(1, (int)(ImGui::GetContentRegionAvail().x / (thumbnailSize + 10.0f)));
			for (size_t i = 0; i < m_Textures.size(); i++)
			{
				ImTextureID texID = (ImTextureID)m_Textures[i];
				// 썸네일 배치 (한 줄에 여러 개)
				if (i % itemsPerRow != 0)
					ImGui::SameLine();
				ImGui::BeginGroup();
				// 선택된 텍스처 강조 표시
				bool isSelected = (i == m_SelectedTextureIndex);
				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
				}
				char btnId[32];
				sprintf_s(btnId, "ImgBtn_%zu", i);
				if (ImGui::ImageButton(btnId, texID, ImVec2(thumbnailSize, thumbnailSize)))
				{
					m_SelectedTextureIndex = i;
					// 선택된 텍스처 경로 업데이트
					if (i < m_TextureFilePaths.size())
					{
						// 클래스 멤버 변수에 직접 저장
						m_wstrSelectedTexturePath = GetRelativePath(m_TextureFilePaths[i]);
						// 화면 표시용 문자열 버퍼에 복사
						strcpy_s(m_szSelectedPathBuffer, ISerializable::WideToUtf8(m_wstrSelectedTexturePath).c_str());
					}
				}
				if (isSelected)
				{
					ImGui::PopStyleColor();
					ImGui::PopStyleVar();
				}

				// 선택 표시 라벨 추가
				if (isSelected)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Selected");
				}
				else
				{
					ImGui::Text("Texture %d", (int)i + 1);
				}
				ImGui::EndGroup();
			}
			// 선택 버튼
			ImGui::Separator();
		}
		ImGui::End();
	}
}

_wstring CMyImGui::SelectFile()
{
	_wstring selectedPath;

	// COM 초기화
	HRESULT hr1 = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	IFileOpenDialog* pFileOpen;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

	if (SUCCEEDED(hr))
	{
		// 이미지 파일 필터 설정
		COMDLG_FILTERSPEC fileTypes[] =
		{
			{ L"이미지 파일", L"*.png;*.jpg;*.jpeg;*.bmp;*.dds;*.tga" },
			{ L"모든 파일", L"*.*" }
		};
		pFileOpen->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);

		// 대화상자 표시
		hr = pFileOpen->Show(NULL);

		if (SUCCEEDED(hr))
		{
			IShellItem* pItem;
			hr = pFileOpen->GetResult(&pItem);

			if (SUCCEEDED(hr))
			{
				PWSTR pszFilePath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

				if (SUCCEEDED(hr))
				{
					selectedPath = pszFilePath;
					CoTaskMemFree(pszFilePath);
				}

				pItem->Release();
			}
		}

		pFileOpen->Release();
	}

	CoUninitialize();

	return selectedPath;
}

_wstring CMyImGui::SelectFolder()
{
	_wstring selectedPath;

	// COM 초기화 (이미 초기화된 경우는 무시됨)
	HRESULT hr1 = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	IFileOpenDialog* pFileOpen;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

	if (SUCCEEDED(hr))
	{
		// 대화상자를 폴더 선택 모드로 설정
		_ulong dwOptions;
		hr = pFileOpen->GetOptions(&dwOptions);
		if (SUCCEEDED(hr))
		{
			hr = pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);
		}

		// 대화상자 표시
		hr = pFileOpen->Show(NULL);

		if (SUCCEEDED(hr))
		{
			IShellItem* pItem;
			hr = pFileOpen->GetResult(&pItem);

			if (SUCCEEDED(hr))
			{
				PWSTR pszFilePath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

				if (SUCCEEDED(hr))
				{
					selectedPath = pszFilePath;
					CoTaskMemFree(pszFilePath);
				}

				pItem->Release();
			}
		}

		pFileOpen->Release();
	}

	CoUninitialize();

	return selectedPath;
}


_wstring CMyImGui::GetRelativePath(const _wstring& absolutePath)
{
	if (absolutePath.empty()) return absolutePath;
	_wstring relativePath = L"../../" + absolutePath.substr(absolutePath.find(L"Resources"));

	return relativePath;
}

void CMyImGui::ShowItemCreationTab()
{
	static int s_iSelectedItemType = 0;
	static int s_iSelectedItemSubType = 0;
	static char s_szItemNameBuffer[256] = { 0 };
	static int s_iItemValue = 10;
	static bool s_bItemFloating = true;
	static bool s_bItemAnimation = false;
	static float s_fItemScale = 0.5f;
	static float s_fItemFloatMin = 0.3f;
	static float s_fItemFloatMax = 0.7f;

	// 아이템 타입 및 서브타입 배열도 static으로 선언
	static const char* s_ItemTypeNames[] = { "HP", "MP", "Ammo", "EXP", "Stat","Key" ,"Weapon"};
	static const char* s_HPSubTypeNames[] = { "Small HP", "Big HP" };
	static const char* s_MPSubTypeNames[] = { "Small MP", "Big MP" };
	static const char* s_AmmoSubTypeNames[] = { "ShotGun_Ammo_Small","ShotGun_Ammo_Big",  "Staff_Ammo_Small", "Staff_Ammo_Big",
		"Magnum_Ammo_Small",
		"Magnum_Ammo_Big" };
	static const char* s_ExpSubTypeNames[] = { "Small EXP", "Medium EXP", "Large EXP" };

	static const char* s_KeySubTypeNames[] = { "Blue", "Red", "Yellow" };
	static const char* s_WeaponSubTypeNames[] = { "ShotGun", "Staff", "Minigun" };

	// 현재 선택된 아이템 타입에 따른 서브타입 배열
	static const char** s_pCurrentSubTypeNames = nullptr;
	static int s_iCurrentSubTypeCount = 0;

	// 서브타입 UI 업데이트 함수 (함수 내에 람다로 정의)
	auto UpdateSubTypeUI = [&](int iItemType) {
		// 선택된 아이템 타입에 따라 서브타입 배열 설정
		switch (iItemType)
		{
		case 0: // HP
			s_pCurrentSubTypeNames = s_HPSubTypeNames;
			s_iCurrentSubTypeCount = IM_ARRAYSIZE(s_HPSubTypeNames);
			strcpy_s(s_szItemNameBuffer, "HP_Small");
			s_iItemValue = 10;
			s_bItemAnimation = false;
			break;
		case 1: // MP
			s_pCurrentSubTypeNames = s_MPSubTypeNames;
			s_iCurrentSubTypeCount = IM_ARRAYSIZE(s_MPSubTypeNames);
			strcpy_s(s_szItemNameBuffer, "MP_Small");
			s_iItemValue = 10;
			s_bItemAnimation = false;
			break;
		case 2: // Ammo
			s_pCurrentSubTypeNames = s_AmmoSubTypeNames;
			s_iCurrentSubTypeCount = IM_ARRAYSIZE(s_AmmoSubTypeNames);
			strcpy_s(s_szItemNameBuffer, "ShotGun_Ammo_Small");
			s_iItemValue = 20;
			s_bItemAnimation = false;
			break;
		case 3: // EXP
			s_pCurrentSubTypeNames = s_ExpSubTypeNames;
			s_iCurrentSubTypeCount = IM_ARRAYSIZE(s_ExpSubTypeNames);
			strcpy_s(s_szItemNameBuffer, "EXP");
			s_iItemValue = 100;
			s_bItemAnimation = false;
			break;
		case 4: // Stat
			s_pCurrentSubTypeNames = nullptr;
			s_iCurrentSubTypeCount = 0;
			strcpy_s(s_szItemNameBuffer, "Stat");
			s_iItemValue = 1;
			s_bItemAnimation = true;
			break;
		case 5: // Key
			s_pCurrentSubTypeNames = s_KeySubTypeNames;
			s_iCurrentSubTypeCount = IM_ARRAYSIZE(s_KeySubTypeNames);
			strcpy_s(s_szItemNameBuffer, "Blue");
			s_iItemValue = 1;
			s_bItemAnimation = false;
			break;
		case 6: // Weapon
			s_pCurrentSubTypeNames = s_WeaponSubTypeNames;
			s_iCurrentSubTypeCount = IM_ARRAYSIZE(s_WeaponSubTypeNames);
			strcpy_s(s_szItemNameBuffer, "ShotGun");
			s_iItemValue = 1;
			s_bItemAnimation = false;
			break;
		default:
			s_pCurrentSubTypeNames = nullptr;
			s_iCurrentSubTypeCount = 0;
			break;
		}
		};

	ImGui::Separator();
	ImGui::Text("Item Settings");

	// 아이템 타입 선택
	if (ImGui::Combo("Item Type", &s_iSelectedItemType, s_ItemTypeNames, IM_ARRAYSIZE(s_ItemTypeNames)))
	{
		// 타입 변경 시 서브타입 UI 업데이트
		UpdateSubTypeUI(s_iSelectedItemType);
		// 서브타입 선택 초기화
		s_iSelectedItemSubType = 0;
	}

	if (s_pCurrentSubTypeNames != nullptr && s_iCurrentSubTypeCount > 0)
	{
		// 서브타입이 변경되었을 때 아이템 이름도 업데이트
		if (ImGui::Combo("Item Sub Type", &s_iSelectedItemSubType, s_pCurrentSubTypeNames, s_iCurrentSubTypeCount))
		{
			// 서브타입에 따른 아이템 이름 업데이트
			switch (s_iSelectedItemType)
			{
			case 0: // HP
				strcpy_s(s_szItemNameBuffer, s_iSelectedItemSubType == 0 ? "HP_Small" : "HP_Big");
				break;
			case 1: // MP
				strcpy_s(s_szItemNameBuffer, s_iSelectedItemSubType == 0 ? "MP_Small" : "MP_Big");
				break;
			case 2: // Ammo
				switch (s_iSelectedItemSubType)
				{
				case 0: strcpy_s(s_szItemNameBuffer, "ShotGun_Ammo_Small"); break;
				case 1: strcpy_s(s_szItemNameBuffer, "ShotGun_Ammo_Big"); break;
				case 2: strcpy_s(s_szItemNameBuffer, "Staff_Ammo_Small"); break;
				case 3: strcpy_s(s_szItemNameBuffer, "Staff_Ammo_Big"); break;
				case 4: strcpy_s(s_szItemNameBuffer, "Magnum_Ammo_Small"); break;
				case 5: strcpy_s(s_szItemNameBuffer, "Magnum_Ammo_Big"); break;
				default: strcpy_s(s_szItemNameBuffer, "Pistol_Ammo_Small"); break;
				}
				break;
			case 3: // EXP
				switch (s_iSelectedItemSubType)
				{
				case 0: strcpy_s(s_szItemNameBuffer, "EXP_Small"); break;
				case 1: strcpy_s(s_szItemNameBuffer, "EXP_Medium"); break;
				case 2: strcpy_s(s_szItemNameBuffer, "EXP_Large"); break;
				default: strcpy_s(s_szItemNameBuffer, "EXP"); break;
				}
				break;
			case 4: //Stat
				strcpy_s(s_szItemNameBuffer, "Stat");
				break;
			case 5: //Key
				switch (s_iSelectedItemSubType)
				{
				case 0: strcpy_s(s_szItemNameBuffer, "Blue"); break;
				case 1: strcpy_s(s_szItemNameBuffer, "Red"); break;
				case 2: strcpy_s(s_szItemNameBuffer, "Yellow"); break;
				default: strcpy_s(s_szItemNameBuffer, "Blue"); break;
				}
				break;
			case 6: //Weapon
				switch (s_iSelectedItemSubType)
				{
				case 0: strcpy_s(s_szItemNameBuffer, "ShotGun"); break;
				case 1: strcpy_s(s_szItemNameBuffer, "Staff"); break;
				case 2: strcpy_s(s_szItemNameBuffer, "Minigun"); break;
				default: strcpy_s(s_szItemNameBuffer, "ShotGun"); break;
				}
				break;
			}
		}
	}

	// 아이템 이름 
	ImGui::InputText("Item Name", s_szItemNameBuffer, IM_ARRAYSIZE(s_szItemNameBuffer));

	//// 아이템 값 (회복량, 탄약 수 등)
	//ImGui::InputInt("Item Value", &s_iItemValue);
	//if (s_iItemValue < 1) s_iItemValue = 1;

	// 아이템 생성 버튼
	if (ImGui::Button("Create Item"))
	{
		// 아이템 타입 및 이름 결정
		CItem::ITEM_TYPE eItemType = static_cast<CItem::ITEM_TYPE>(s_iSelectedItemType);
		_wstring strItemName;

		switch (eItemType)
		{
		case CItem::ITEM_TYPE::HP:
			strItemName = (s_iSelectedItemSubType == 0) ? L"HP_Small" : L"HP_Big";
			break;
		case CItem::ITEM_TYPE::MP:
			strItemName = (s_iSelectedItemSubType == 0) ? L"MP_Small" : L"MP_Big";
			break;
		case CItem::ITEM_TYPE::AMMO:
			switch (s_iSelectedItemSubType)
			{
			case 0: strItemName = L"Shotgun_Ammo_Small"; break;
			case 1: strItemName = L"Shotgun_Ammo_Big"; break;
			case 2: strItemName = L"Staff_Ammo_Small"; break;
			case 3: strItemName = L"Staff_Ammo_Big"; break;
			case 4: strItemName = L"Pistol_Ammo_Small"; break;
			case 5: strItemName = L"Pistol_Ammo_Big"; break;
			default: strItemName = L"Pistol_Ammo_Small"; break;
			}
			break;
		case CItem::ITEM_TYPE::EXP:
			switch (s_iSelectedItemSubType)
			{
			case 0: strItemName = L"EXP_Small"; break;
			case 1: strItemName = L"EXP_Medium"; break;
			case 2: strItemName = L"EXP_Large"; break;
			default: strItemName = L"EXP"; break;
			}
			break;
		case CItem::ITEM_TYPE::STAT:
			strItemName = L"Stat";
			break;
		case CItem::ITEM_TYPE::KEY:
			switch (s_iSelectedItemSubType)
			{
			case 0: strItemName = L"Blue"; break;
			case 1: strItemName = L"Red"; break;
			case 2: strItemName = L"Yellow"; break;
			default: strItemName = L"Blue"; break;
			}
			break;

		case CItem::ITEM_TYPE::WEAPON:
			switch (s_iSelectedItemSubType)
			{
			case 0: strItemName = L"ShotGun"; break;
			case 1: strItemName = L"Staff"; break;
			case 2: strItemName = L"Minigun"; break;
			default: strItemName = L"ShotGun"; break;
			}
			break;
		default:
			strItemName = L""; // 기본값
			break;
		}

		// 아이템 이름에 사용자 입력 적용 (만약 사용자가 입력했다면)
		if (s_szItemNameBuffer[0] != '\0')
		{
			wchar_t wszItemName[256] = {};
			MultiByteToWideChar(CP_ACP, 0, s_szItemNameBuffer, -1, wszItemName, 256);
			strItemName = wszItemName;
		}

		// 아이템 생성을 위한 구조체 설정
		CItem::ITEM_DESC tItemDesc;
		tItemDesc.eType = eItemType;
		tItemDesc.strItemName = strItemName;
		tItemDesc.iLevel = 3;
		tItemDesc.iProtoLevel = 3;
		tItemDesc.stProtTextureTag = L"Prototype_Component_Texture_Items";
		tItemDesc.stBufferTag = L"Prototype_Component_VIBuffer_Rect";
		tItemDesc.stProtTag = L"Prototype_GameObject_Item_" + strItemName;
		string stName = s_szItemNameBuffer;
		wstring strProtoTag = L"Prototype_GameObject_Item_" + ISerializable::Utf8ToWide(stName);
		wstring strLayerTag = L"Layer_Item";
		// 아이템 게임 오브젝트 생성
		CJsonLoader jsonLoader;

		if (FAILED(m_pGameInstance->Find_Prototype(strProtoTag)))
		{
			CBase* pGameObject = jsonLoader.Create_Object_ByClassName("CItem", m_pGraphic_Device);
			if (FAILED(m_pGameInstance->Add_Prototype(tItemDesc.iProtoLevel, strProtoTag, pGameObject)))
			{
				Safe_Release(pGameObject);
				return;
			}
		}

		// 게임 오브젝트 추가
		if (FAILED(m_pGameInstance->Add_GameObject(3, strProtoTag, 3, strLayerTag, &tItemDesc)))
		{
			return;
		}

		CGameObject* pItemObject = m_pGameInstance->Find_Last_Object(3, strLayerTag);
		if (pItemObject)
		{
			CTransform* pTransform = (CTransform*)pItemObject->Get_Component(TEXT("Com_Transform"));
			if (pTransform)
			{
				// 카메라 정보 (뷰 행렬) 가져오기
				_float4x4 vViewMatrix;
				m_pGraphic_Device->GetTransform(D3DTS_VIEW, &vViewMatrix);

				// 뷰 행렬의 역행렬 계산 (카메라의 월드 변환 행렬)
				D3DXMATRIX vInvViewMatrix;
				D3DXMatrixInverse(&vInvViewMatrix, nullptr, &vViewMatrix);

				// 카메라 위치와 방향 
				_float3 vCameraPos = { vInvViewMatrix._41, vInvViewMatrix._42, vInvViewMatrix._43 };
				_float3 vCameraLook = { vInvViewMatrix._31, vInvViewMatrix._32, vInvViewMatrix._33 };
				vCameraLook.Normalize();

				// 카메라 앞 일정 거리에 아이템 배치
				_float3 vItemPos = vCameraPos + vCameraLook * 2.0f;

				// 아이템 위치 설정
				pTransform->Set_State(CTransform::STATE_POSITION, vItemPos);

				if (eItemType == CItem::ITEM_TYPE::WEAPON)
				{
					s_fItemScale = 1.f;
				}
				else
				{
					s_fItemScale = 0.5f;
				}
				// 아이템 크기 설정
				pTransform->Set_Scale(s_fItemScale, s_fItemScale, s_fItemScale);

				// 아이템 기본 방향 설정 (플레이어 없이도 작동하도록)
				_float3 vRight = { 1.0f, 0.0f, 0.0f };
				_float3 vUp = { 0.0f, 1.0f, 0.0f };
				_float3 vLook = { 0.0f, 0.0f, 1.0f };

				vRight.Normalize();
				vUp.Normalize();
				vLook.Normalize();

				vRight *= s_fItemScale;
				vUp *= s_fItemScale;
				vLook *= s_fItemScale;

				pTransform->Set_State(CTransform::STATE_RIGHT, vRight);
				pTransform->Set_State(CTransform::STATE_UP, vUp);
				pTransform->Set_State(CTransform::STATE_LOOK, vLook);
			}
		}
		SaveObjectToJson(
			"",
			tItemDesc.stProtTag,         // 오브젝트 태그
			tItemDesc.iLevel,        // 오브젝트 레벨
			L"CItem",        // 클래스 이름
			tItemDesc.stProtTextureTag,       // 텍스처 태그
			tItemDesc.iLevel,             // 텍스처 레벨
			L"",     // 텍스처 경로
			96,       // 텍스처 개수
			tItemDesc.stBufferTag,        // 버퍼 태그
			3,                  // 버퍼 레벨 (기본값)
			tItemDesc.stBufferTag // 버퍼 클래스 이름
		);

		//tHistoryItem historyItem;
		//historyItem.iLevel = 3;
		//historyItem.iProtoLevel = 3;
		//historyItem.eType = EHistoryActionType::OBJECT_CREATE;
		//historyItem.pGameObject = m_pGameInstance->Find_Last_Object(3, strLayerTag);
		//historyItem.wstrPrototypeTag = strProtoTag;
		//historyItem.wstrLayerTag = strLayerTag;
		//AddToHistory(historyItem);
	}
}

void CMyImGui::RenderImGuizmo(CTransform* pTransform)
{
	if (!pTransform)
		return;

	bool wasUsingGuizmo = ImGuizmo::IsUsing();
	if (!wasUsingGuizmo && ImGuizmo::IsOver())
	{
		BeginTransformAction();
	}

	// 그래픽 디바이스에서 직접 뷰 및 투영 행렬 가져오기
	D3DXMATRIX d3dViewMatrix, d3dProjMatrix;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &d3dViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &d3dProjMatrix);

	// ImGuizmo에서 사용할 수 있는 float 배열로 변환
	float viewMatrix[16], projMatrix[16];
	memcpy(viewMatrix, &d3dViewMatrix, sizeof(float) * 16);
	memcpy(projMatrix, &d3dProjMatrix, sizeof(float) * 16);

	// 오브젝트의 월드 행렬 가져오기
	_float4x4 worldMatrix = pTransform->Get_WorldMat();

	// ImGuizmo 조작 영역 설정
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	// 변환을 위한 임시 행렬
	float matrix[16];
	memcpy(matrix, &worldMatrix, sizeof(float) * 16);

	// 스냅 값 설정
	float snapValues[3] = { 0.0f, 0.0f, 0.0f };
	if (m_bUseSnap)
	{
		snapValues[0] = m_SnapValue[0];
		snapValues[1] = m_SnapValue[1];
		snapValues[2] = m_SnapValue[2];
	}

	// 기즈모 조작 렌더링
	bool manipulated = ImGuizmo::Manipulate(
		viewMatrix,
		projMatrix,
		m_CurrentGizmoOperation,
		m_CurrentGizmoMode,
		matrix,
		nullptr,
		m_bUseSnap ? snapValues : nullptr
	);
	// 조작이 있었다면 트랜스폼 업데이트
	if (manipulated)
	{
		// 행렬에서 위치, 회전, 크기 추출
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);

		// 트랜스폼 컴포넌트 업데이트
		if (m_CurrentGizmoOperation == ImGuizmo::TRANSLATE)
		{
			_float3 newPosition = _float3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
			pTransform->Set_State(CTransform::STATE_POSITION, newPosition);
		}
		else if (m_CurrentGizmoOperation == ImGuizmo::ROTATE)
		{
			_float3 newRotation = _float3(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
			pTransform->Rotate_EulerAngles(newRotation);
		}
		else if (m_CurrentGizmoOperation == ImGuizmo::SCALE)
		{
			_float3 newScale = _float3(matrixScale[0], matrixScale[1], matrixScale[2]);
			pTransform->Set_Scale(newScale.x, newScale.y, newScale.z);
		}
	}

	if (wasUsingGuizmo && !ImGuizmo::IsUsing())
	{
		EndTransformAction();
	}
}
void CMyImGui::InputKey()
{
	if (m_pCurrentGameObject && !ImGui::GetIO().WantTextInput)
	{
		// W = 이동 모드
		if (ImGui::IsKeyPressed(ImGuiKey_1))
			m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;

		// E = 회전 모드
		if (ImGui::IsKeyPressed(ImGuiKey_2))
			m_CurrentGizmoOperation = ImGuizmo::ROTATE;

		// R = 스케일 모드
		if (ImGui::IsKeyPressed(ImGuiKey_3))
			m_CurrentGizmoOperation = ImGuizmo::SCALE;
	}


	// Undo/Redo 키 추가
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
	{

		_float currentTime = static_cast<float>(ImGui::GetTime());
		if (currentTime - m_fLastInputTime < m_fInputCooldown)
			return;
		m_fLastInputTime = currentTime;

		if (ImGui::IsKeyPressed(ImGuiKey_Z))
			Undo();
		if (ImGui::IsKeyPressed(ImGuiKey_D))
			Duplicate_Object();
		if (ImGui::IsKeyPressed(ImGuiKey_G))
			Remove_Object();
	}
}

void CMyImGui::AddToHistory(const tHistoryItem& item)
{
	// 히스토리 스택 크기 제한
	if (m_vecUndoStack.size() >= 50)
	{
		// 맨 아래 항목 제거
		m_vecUndoStack.pop();
	}

	tHistoryItem newItem = item;

	if (item.pGameObject)
	{
		newItem.objectID = AssignObjectID(item.pGameObject);
	}

	m_vecUndoStack.push(newItem);
}

void CMyImGui::BeginTransformAction()
{
	if (!m_pCurrentGameObject || m_bTrackingTransform)
		return;

	CTransform* pTransform = (CTransform*)m_pCurrentGameObject->Get_Component(TEXT("Com_Transform"));
	if (!pTransform)
		return;

	m_bTrackingTransform = true;

	// 현재 상태 저장
	m_vPrevPosition = pTransform->Get_State(CTransform::STATE_POSITION);
	m_vPrevRotation = pTransform->Get_EulerAngles();
	m_vPrevScale = pTransform->Compute_Scaled();
}

void CMyImGui::EndTransformAction()
{
	if (!m_pCurrentGameObject || !m_bTrackingTransform)
		return;

	CTransform* pTransform = (CTransform*)m_pCurrentGameObject->Get_Component(TEXT("Com_Transform"));
	if (!pTransform)
		return;

	m_bTrackingTransform = false;

	// 현재 상태 가져오기
	_float3 vCurrPosition = pTransform->Get_State(CTransform::STATE_POSITION);
	_float3 vCurrRotation = pTransform->Get_EulerAngles();
	_float3 vCurrScale = pTransform->Compute_Scaled();

	if (memcmp(&m_vPrevPosition, &vCurrPosition, sizeof(_float3)) != 0)
	{
		tHistoryItem item;
		item.eType = EHistoryActionType::TRANSFORM_POSITION;
		item.pGameObject = m_pCurrentGameObject;
		item.vOldPosition = m_vPrevPosition;
		item.vNewPosition = vCurrPosition;
		item.pGameObject = m_pCurrentGameObject;
		AddToHistory(item);
	}
	else if (memcmp(&m_vPrevRotation, &vCurrRotation, sizeof(_float3)) != 0)
	{
		tHistoryItem item;
		item.eType = EHistoryActionType::TRANSFORM_ROTATION;
		item.pGameObject = m_pCurrentGameObject;
		item.vOldRotation = m_vPrevRotation;
		item.vNewRotation = vCurrRotation;
		item.pGameObject = m_pCurrentGameObject;
		AddToHistory(item);
	}
	else if (memcmp(&m_vPrevScale, &vCurrScale, sizeof(_float3)) != 0)
	{
		tHistoryItem item;
		item.eType = EHistoryActionType::TRANSFORM_SCALE;
		item.pGameObject = m_pCurrentGameObject;
		item.vOldScale = m_vPrevScale;
		item.vNewScale = vCurrScale;
		item.pGameObject = m_pCurrentGameObject;
		AddToHistory(item);
	}
}

void CMyImGui::Undo()
{
	//if (m_vecUndoStack.empty())
	//	return;

	//// 마지막 액션 가져오기
	//tHistoryItem item = m_vecUndoStack.top();
	//m_vecUndoStack.pop();


	//switch (item.eType)
	//{
	//case EHistoryActionType::TRANSFORM_POSITION:
	//{
	//	if (!item.pGameObject)
	//		break;

	//	// 메모리에 아직 존재하는지 확인
	//	CGameObject* pFoundObject = Find_Object_ByAddress(item.pGameObject);
	//	if (pFoundObject)
	//	{
	//		CTransform* pTransform = (CTransform*)pFoundObject->Get_Component(TEXT("Com_Transform"));
	//		if (pTransform)
	//			pTransform->Set_State(CTransform::STATE_POSITION, item.vOldPosition);
	//	}
	//}
	//break;

	//case EHistoryActionType::TRANSFORM_ROTATION:
	//{
	//	if (!item.pGameObject)
	//		break;

	//	// 메모리에 아직 존재하는지 확인
	//	CGameObject* pFoundObject = Find_Object_ByAddress(item.pGameObject);
	//	if (pFoundObject)
	//	{
	//		CTransform* pTransform = (CTransform*)pFoundObject->Get_Component(TEXT("Com_Transform"));
	//		if (pTransform)
	//			pTransform->Rotate_EulerAngles(item.vOldRotation);
	//	}
	//}
	//break;

	//case EHistoryActionType::TRANSFORM_SCALE:
	//{
	//	if (!item.pGameObject)
	//		break;

	//	// 메모리에 아직 존재하는지 확인
	//	CGameObject* pFoundObject = Find_Object_ByAddress(item.pGameObject);
	//	if (pFoundObject)
	//	{
	//		CTransform* pTransform = (CTransform*)pFoundObject->Get_Component(TEXT("Com_Transform"));
	//		if (pTransform)
	//			pTransform->Set_Scale(item.vOldScale.x, item.vOldScale.y, item.vOldScale.z);
	//	}
	//}
	//break;

	//case EHistoryActionType::OBJECT_CREATE:
	//{
	//	// 오브젝트 생성을 취소하므로 오브젝트 삭제
	//	if (item.pGameObject)
	//	{
	//		m_pGameInstance->Remove_Object(item.iLevel, item.wstrLayerTag, item.pGameObject);
	//		item.pGameObject = nullptr;
	//	}
	//}
	//break;

	//case EHistoryActionType::OBJECT_DELETE:
	//{
	//	// 오브젝트 삭제를 취소하므로 오브젝트 다시 생성
	//	if (item.tObjDesc != nullptr)
	//	{
	//		m_pGameInstance->Add_GameObject(item.iProtoLevel, item.wstrPrototypeTag,
	//			item.iLevel, item.wstrLayerTag, item.tObjDesc);

	//		CGameObject* pGameObject = m_pGameInstance->Find_Last_Object(item.iLevel, item.wstrLayerTag);
	//		if (pGameObject != nullptr)
	//		{
	//			CTransform* pTransform = (CTransform*)pGameObject->Get_Component(TEXT("Com_Transform"));
	//			if (pTransform)
	//			{
	//				pTransform->Set_Scale(item.vOldScale.x, item.vOldScale.y, item.vOldScale.z);
	//				pTransform->Set_State(CTransform::STATE_POSITION, item.vOldPosition);
	//				pTransform->Rotate_EulerAngles(item.vOldRotation);
	//			}

	//			// Redo를 위해 새로 생성된 오브젝트 참조 업데이트
	//			item.pGameObject = pGameObject;
	//		}
	//	}
	//	break;
	//}
	//}

	///*if (item.eType == EHistoryActionType::OBJECT_DELETE)
	//{
	//	CGameObject::OBJECT_DESC* pOldDesc = static_cast<CGameObject::OBJECT_DESC*>(item.tObjDesc);
	//	CGameObject::OBJECT_DESC* pNewDesc = new CGameObject::OBJECT_DESC;
	//	*pNewDesc = *pOldDesc;
	//	item.tObjDesc = pNewDesc;
	//	Safe_Delete(pOldDesc);
	//}*/

	if (m_vecUndoStack.empty())
	{
		return;
	}

	tHistoryItem item = m_vecUndoStack.top();
	m_vecUndoStack.pop();

	//객체 참조 ID 기반으로 처리
	CGameObject* pTargetObject = nullptr;

	if (item.eType == EHistoryActionType::TRANSFORM_POSITION ||
		item.eType == EHistoryActionType::TRANSFORM_ROTATION ||
		item.eType == EHistoryActionType::TRANSFORM_SCALE)
	{
		pTargetObject = GetObjectByID(item.objectID);

		if (!pTargetObject)
		{
			return;
		}
	}

	m_pCurrentGameObject = pTargetObject;

	switch (item.eType)
	{
	case EHistoryActionType::TRANSFORM_POSITION:
	{
		CTransform* pTransform = (CTransform*)pTargetObject->Get_Component(TEXT("Com_Transform"));
		if (pTransform)
		{
			_float3 currentPos = pTransform->Get_State(CTransform::STATE_POSITION);
			pTransform->Set_State(CTransform::STATE_POSITION, item.vOldPosition);

		}
	}
	break;

	case EHistoryActionType::TRANSFORM_ROTATION:
	{

		CTransform* pTransform = (CTransform*)pTargetObject->Get_Component(TEXT("Com_Transform"));
		if (pTransform)
		{
			_float3 currentRot = pTransform->Get_EulerAngles();
			pTransform->Rotate_EulerAngles(item.vOldRotation);
		}
	}
	break;

	case EHistoryActionType::TRANSFORM_SCALE:
	{
		CTransform* pTransform = (CTransform*)pTargetObject->Get_Component(TEXT("Com_Transform"));
		if (pTransform)
		{
			_float3 currentScale = pTransform->Compute_Scaled();
			pTransform->Set_Scale(item.vOldScale.x, item.vOldScale.y, item.vOldScale.z);
		}
	}
	break;

	case EHistoryActionType::OBJECT_CREATE:
	{
		if (item.pGameObject)
		{
			// 객체 ID 맵에서 찾기
			pTargetObject = GetObjectByID(item.objectID);
			if (pTargetObject)
			{
				m_pGameInstance->Remove_Object(item.iLevel, item.wstrLayerTag, pTargetObject);

				// 객체 ID 맵에서 제거
				RemoveObjectID(pTargetObject);
			}
		}
	}
	break;

	case EHistoryActionType::OBJECT_DELETE:
	{
		if (item.tObjDesc != nullptr)
		{
			// 객체 다시 생성
			m_pGameInstance->Add_GameObject(item.iProtoLevel, item.wstrPrototypeTag,
				item.iLevel, item.wstrLayerTag, item.tObjDesc);

			// 새로 생성된 객체 가져오기
			CGameObject* pGameObject = m_pGameInstance->Find_Last_Object(item.iLevel, item.wstrLayerTag);
			if (pGameObject != nullptr)
			{
				// 트랜스폼 속성 복원
				CTransform* pTransform = (CTransform*)pGameObject->Get_Component(TEXT("Com_Transform"));
				if (pTransform)
				{
					pTransform->Set_Scale(item.vOldScale.x, item.vOldScale.y, item.vOldScale.z);
					pTransform->Set_State(CTransform::STATE_POSITION, item.vOldPosition);
					pTransform->Rotate_EulerAngles(item.vOldRotation);
				}

				// 새 객체에 ID 할당 (이전 객체와 동일한 ID)
				AssignSpecificObjectID(pGameObject, item.objectID);
			}
		}
	}
	break;
	}

}

_uint CMyImGui::AssignObjectID(CGameObject* pObj)
{
	if (!pObj) return 0;

	// 이미 ID가 있는지 확인
	auto it = m_ObjectIDMap.find(pObj);
	if (it != m_ObjectIDMap.end())
		return it->second;

	// 새 ID 할당
	UINT newID = ++m_NextObjectID;
	m_ObjectIDMap[pObj] = newID;
	m_IDObjectMap[newID] = pObj;

	return newID;
}

void CMyImGui::AssignSpecificObjectID(CGameObject* pObj, _uint specificID)
{
	if (!pObj) return;

	// 이미 다른 ID가 할당되어 있다면 기존 매핑 제거
	auto it = m_ObjectIDMap.find(pObj);
	if (it != m_ObjectIDMap.end())
	{
		m_IDObjectMap.erase(it->second);
		m_ObjectIDMap.erase(it);
	}

	m_ObjectIDMap[pObj] = specificID;
	m_IDObjectMap[specificID] = pObj;
}

// ID로 객체 찾기
CGameObject* CMyImGui::GetObjectByID(UINT id)
{
	auto it = m_IDObjectMap.find(id);
	if (it != m_IDObjectMap.end())
		return it->second;
	return nullptr;
}

// 객체 삭제 시 ID 제거
void CMyImGui::RemoveObjectID(CGameObject* pObj)
{
	auto it = m_ObjectIDMap.find(pObj);
	if (it != m_ObjectIDMap.end()) {
		m_IDObjectMap.erase(it->second);
		m_ObjectIDMap.erase(it);
	}
}
_bool CMyImGui::IsMouseOverImGui()
{
	ImGuiIO& io = ImGui::GetIO();
	return io.WantCaptureMouse || io.WantCaptureKeyboard;
}
//
//void CMyImGui::ShowLayerInMap(map<const _wstring, class CLayer*>* pLayers)
//{
//	if (ImGui::BeginTable("MyMapTable", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders)) {
//		// 헤더 설정
//		ImGui::TableSetupColumn("Layer Key");
//		//ImGui::TableSetupColumn("Objects"); // 오브젝트 리스트를 표시할 열
//		ImGui::TableHeadersRow();
//
//		// 맵의 각 요소를 순회하며 테이블에 행 추가
//		for (_uint i = 0; i < m_iNumLevels; ++i) {
//			// std::map<std::wstring, CLayer*> iter = pLayers[i];
//			for (auto& pair : pLayers[i])
//			{
//				ImGui::TableNextRow(); // 새 행 시작
//				ImGui::TableNextColumn(); // 첫 번째 열 (Key)
//				if (ImGui::CollapsingHeader(WStringToString(pair.first).c_str())) // Key (wstring) 출력
//					ShowListInLayer(pair.second);
//			}
//		}
//		ImGui::EndTable();
//	}
//}
//
//void CMyImGui::ShowListInLayer(CLayer* pLayer)
//{
//	////내부 테이블
//	//if (ImGui::BeginTable("ObjectsTable", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
//	//{
//	//	ImGui::TableSetupColumn("ID");
//	//	//ImGui::TableSetupColumn("Components");
//	//	ImGui::TableHeadersRow();
//	//	int i = 0;
//	//	for (const auto& pGameObject : pLayer->m_GameObjects)
//	//	{
//	//		ImGui::TableNextRow();
//	//		ImGui::TableNextColumn();
//	//		if (ImGui::CollapsingHeader(to_string(i).c_str()))
//	//			ShowComponentsInGameObject(pGameObject);
//	//		//ImGui::TableNextColumn();
//	//		//ImGui::Text("%d", pGameObject->GetID());
//	//		++i;
//	//	}
//	//	ImGui::EndTable();
//	//}
//}

void CMyImGui::ShowComponentsInGameObject(CGameObject* pGameObject)
{
	//if (ImGui::BeginTable("ObjectsTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
	//{
	//	ImGui::TableSetupColumn("Components");
	//	ImGui::TableHeadersRow();
	//	for (const auto& pair : pGameObject->m_Components)
	//	{
	//		ImGui::TableNextRow();
	//		ImGui::TableNextColumn();
	//		if (ImGui::CollapsingHeader(WStringToString(pair.first).c_str()))
	//		{

	//		}
	//	}
	//	ImGui::EndTable();
	//}
}

void CMyImGui::ShowTriggerTab()
{
	static int s_iSelectedTriggerType = 0;
	static char s_szTargetTagBuffer[256] = { 0 };
	static float s_fActivationRange = 1.0f;
	static bool s_bStartsActive = true;
	static float s_vPosition[3] = { 0.0f, 0.0f, 0.0f };
	static char s_szTextureTagBuffer[256] = "Prototype_Component_Texture_Trigger";
	static const char* s_TriggerTypeNames[] = { "Button", "Interaction" };

	ImGui::Separator();
	ImGui::Text("Trigger Settings");

	// 트리거 타입 선택
	if (ImGui::Combo("Trigger Type", &s_iSelectedTriggerType, s_TriggerTypeNames, IM_ARRAYSIZE(s_TriggerTypeNames)))
	{
		// 타입 변경 시 기본값 설정
		if (s_iSelectedTriggerType == 0) // BUTTON
		{
			strcpy_s(s_szTextureTagBuffer, "Prototype_Component_Texture_Button");
		}
		else // INTERACTION
		{
			strcpy_s(s_szTextureTagBuffer, "Prototype_Component_Texture_Interaction");
		}
	}

	// 활성화 범위 설정
	ImGui::SliderFloat("Activation Range", &s_fActivationRange, 0.5f, 5.0f);

	// 시작시 활성화 여부
	ImGui::Checkbox("Starts Active", &s_bStartsActive);

	// 텍스처 태그 설정
	ImGui::InputText("Trigger Texture Tag", s_szTextureTagBuffer, IM_ARRAYSIZE(s_szTextureTagBuffer));

	// 파일 선택 버튼
	static _wstring selectedTexturePath;
	if (ImGui::Button("Select Trigger Texture"))
	{
		selectedTexturePath = SelectFile();
		if (!selectedTexturePath.empty())
		{
			selectedTexturePath = GetRelativePath(selectedTexturePath);
			LoadImagesFromFolder(selectedTexturePath.substr(0, selectedTexturePath.find_last_of(L"\\")));
			m_bShowImageWindow = true;
		}
	}

	ImGui::Separator();
	ImGui::Text("Target Object Selection");

	// 현재 선택된 대상 표시
	if (m_pSelectedTarget)
	{
		ImGui::Text("Selected Target: %s", ISerializable::WideToUtf8(m_pSelectedTarget->Get_Tag()).c_str());

		// 대상 해제 버튼
		if (ImGui::Button("Clear Target"))
		{
			m_pSelectedTarget = nullptr;
			memset(s_szTargetTagBuffer, 0, sizeof(s_szTargetTagBuffer));
		}
	}
	else
	{
		ImGui::Text("No Target Selected");
	}

	// 직접 태그로 대상 설정
	ImGui::InputText("Target Tag", s_szTargetTagBuffer, IM_ARRAYSIZE(s_szTargetTagBuffer));

	// 태그 목록 표시 (Level에 있는 객체들)
	if (ImGui::CollapsingHeader("Available Objects"))
	{
		// 레벨의 모든 객체 목록 표시
		for (_uint iLevel = 0; iLevel < m_iNumLevels; ++iLevel)
		{
			if (ImGui::TreeNode(("Level " + to_string(iLevel)).c_str()))
			{
				auto& layers = m_pGameInstance->m_pObject_Manager->m_pLayers[iLevel];

				for (auto& layerPair : layers)
				{
					string layerName = ISerializable::WideToUtf8(layerPair.first);
					if (ImGui::TreeNode(layerName.c_str()))
					{
						for (auto& pObj : layerPair.second->m_GameObjects)
						{
							if (!pObj) continue;
							string objTag = ISerializable::WideToUtf8(pObj->Get_Tag());
							bool isSelected = (m_pSelectedTarget == pObj);

							if (ImGui::Selectable(objTag.c_str(), isSelected))
							{
								m_pSelectedTarget = pObj;
								strcpy_s(s_szTargetTagBuffer, objTag.c_str());
							}
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
	}

	// 트리거 생성 버튼
	if (ImGui::Button("Create Trigger"))
	{
		_wstring targetTag;

		// 선택된 대상이 있으면 그 태그 사용, 아니면 입력된 태그 사용
		if (m_pSelectedTarget)
		{
			targetTag = m_pSelectedTarget->Get_Tag();
		}
		else if (s_szTargetTagBuffer[0] != '\0')
		{
			wchar_t wTargetTag[256] = {};
			MultiByteToWideChar(CP_ACP, 0, s_szTargetTagBuffer, -1, wTargetTag, 256);
			targetTag = wTargetTag;
		}

		// 트리거 타입 설정
		CTrigger::TRIGGER_TYPE eType =
			(s_iSelectedTriggerType == 0) ? CTrigger::TRIGGER_TYPE::BUTTON : CTrigger::TRIGGER_TYPE::INTERACTION;

		// 위치 설정
		_float3 vPosition = { s_vPosition[0], s_vPosition[1], s_vPosition[2] };

		// 텍스처 태그 설정
		wchar_t wTextureTag[256] = {};
		MultiByteToWideChar(CP_ACP, 0, s_szTextureTagBuffer, -1, wTextureTag, 256);
		_wstring textureTag = wTextureTag;

		// 트리거 인스턴스 생성
		CreateTriggerInstance(targetTag, static_cast<_uint>(eType), vPosition, s_fActivationRange, s_bStartsActive, textureTag);
	}
}

HRESULT CMyImGui::CreateTriggerInstance(const _wstring& strTargetTag, _uint iTriggerType,
	const _float3& vPosition, _float fActivationRange, _bool bStartsActive, const _wstring& textureTag)
{
	const _uint iLevel = 3; // 기본 레벨
	const _uint iProtoLevel = 3; // 프로토타입 레벨

	// 트리거 프로토타입 이름 설정
	_wstring stProtoTag = L"Prototype_GameObject_Trigger";
	if (static_cast<CTrigger::TRIGGER_TYPE>(iTriggerType) == CTrigger::TRIGGER_TYPE::BUTTON)
		stProtoTag += L"_Button";
	else
		stProtoTag += L"_Interaction";

	// 레이어 태그 설정
	_wstring stLayerTag = L"Layer_Trigger";

	// 트리거 설명 구조체 생성
	CTrigger::TRIGGER_DESC tTriggerDesc{};
	tTriggerDesc.eType = static_cast<CTrigger::TRIGGER_TYPE>(iTriggerType);
	tTriggerDesc.stTargetTag = strTargetTag;
	tTriggerDesc.bStartsActive = bStartsActive;

	// OBJECT_DESC 부분 설정
	tTriggerDesc.iLevel = iLevel;
	tTriggerDesc.iProtoLevel = iProtoLevel;
	tTriggerDesc.stProtTag = stProtoTag;
	tTriggerDesc.stBufferTag = L"Prototype_Component_VIBuffer_Cube";
	tTriggerDesc.stProtTextureTag = textureTag;

	// 프로토타입이 존재하는지 확인
	bool prototypeCreated = false;
	if (FAILED(m_pGameInstance->Find_Prototype(stProtoTag)))
	{
		// 프로토타입 생성
		CJsonLoader jsonLoader;
		CBase* pGameObject = jsonLoader.Create_Object_ByClassName("CDoor", m_pGraphic_Device);

		if (FAILED(m_pGameInstance->Add_Prototype(iProtoLevel, stProtoTag, pGameObject)))
		{
			Safe_Release(pGameObject);
			return E_FAIL;
		}
		prototypeCreated = true;
	}

	// 텍스처가 존재하는지 확인하고 텍스처 경로가 있다면 텍스처 추가
	bool textureCreated = false;
	if (FAILED(m_pGameInstance->Find_Prototype(textureTag)) && !m_wstrSelectedTexturePath.empty())
	{
		if (FAILED(m_pGameInstance->Add_Prototype(iLevel, textureTag,
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, ISerializable::WStringToWChar(m_wstrSelectedTexturePath), 1))))
		{
			return E_FAIL;
		}
		textureCreated = true;
	}

	// 게임 오브젝트 추가
	if (FAILED(m_pGameInstance->Add_GameObject(iProtoLevel, stProtoTag,
		iLevel, stLayerTag, &tTriggerDesc)))
	{
		return E_FAIL;
	}

	// 생성된 트리거 가져오기
	CGameObject* pTriggerObject = m_pGameInstance->Find_Last_Object(iLevel, stLayerTag);

	if (pTriggerObject)
	{
		// 타겟 설정
		CTrigger* pTrigger = dynamic_cast<CTrigger*>(pTriggerObject);
		if (pTrigger && !strTargetTag.empty())
		{
			CGameObject* pTargetObject = nullptr;

			// 레벨에서 타겟 태그로 객체 찾기
			for (_uint iSearchLevel = 0; iSearchLevel < m_iNumLevels; ++iSearchLevel)
			{
				auto& layers = m_pGameInstance->m_pObject_Manager->m_pLayers[iSearchLevel];
				for (auto& layerPair : layers)
				{
					for (auto& pObj : layerPair.second->m_GameObjects)
					{
						if (pObj&&pObj->Get_Tag() == strTargetTag)
						{
							pTargetObject = pObj;
							break;
						}
					}
					if (pTargetObject) break;
				}
				if (pTargetObject) break;
			}

			if (pTargetObject)
			{
				pTrigger->AddTargetObject(static_cast<CDoor*>(pTargetObject));
			}
		}
	}

	// JSON에 저장
	if (prototypeCreated || textureCreated)
	{
		SaveObjectToJson(
			"",
			stProtoTag,         // 오브젝트 태그
			iProtoLevel,        // 오브젝트 레벨
			L"CTrigger",        // 클래스 이름
			textureTag,         // 텍스처 태그
			iLevel,             // 텍스처 레벨
			m_wstrSelectedTexturePath, // 텍스처 경로
			1,                  // 텍스처 개수
			L"Prototype_Component_VIBuffer_Cube", // 버퍼 태그
			3,                  // 버퍼 레벨
			L"CVIBuffer_Cube",  // 버퍼 클래스 이름
			0, 0                // 버퍼 크기 (기본값)
		);
	}

	return S_OK;
}

void CMyImGui::ShowDoorTab()
{
	static int s_iSelectedDoorType = 0; // 0: NORMAL, 1: KEY
	static int s_iSelectedDoorColor = 0; // 0: BLUE, 1: RED, 2: YELLOW, 3: GREEN, 4: NORMAL
	static char s_szKeyItemTagBuffer[256] = { 0 };
	static float s_fSlideDistance = 2.0f;
	static bool s_bStartsActive = true;
	static float s_vPosition[3] = { 0.0f, 0.0f, 0.0f };
	static char s_szTextureTagBuffer[256] = "Prototype_Component_Texture_Door";
	static const char* s_DoorTypeNames[] = { "Normal", "Key" };
	static const char* s_DoorColorNames[] = { "Blue", "Red", "Yellow", "Normal" };

	ImGui::Separator();
	ImGui::Text("Door Settings");

	// 문 타입 선택
	ImGui::Combo("Door Type", &s_iSelectedDoorType, s_DoorTypeNames, IM_ARRAYSIZE(s_DoorTypeNames));

	// 문 색상 선택
	ImGui::Combo("Door Color", &s_iSelectedDoorColor, s_DoorColorNames, IM_ARRAYSIZE(s_DoorColorNames));

	// KEY 타입인 경우에만 커스텀 열쇠 태그 입력 필드 표시
	if (s_iSelectedDoorType == 1) // KEY
	{
		ImGui::InputText("Custom Key Item Tag", s_szKeyItemTagBuffer, IM_ARRAYSIZE(s_szKeyItemTagBuffer));
		ImGui::Text("(Optional: Leave empty to use default color-based key)");
	}

	// 슬라이드 거리 설정
	ImGui::SliderFloat("Slide Distance", &s_fSlideDistance, 0.5f, 5.0f);

	// 시작시 활성화 여부
	ImGui::Checkbox("Starts Active", &s_bStartsActive);

	// 위치 설정
	ImGui::Text("Position");
	ImGui::PushID("DoorPosition");
	ImGui::InputFloat3("##Position", s_vPosition);
	ImGui::PopID();

	// 현재 카메라 위치에 배치 버튼
	if (ImGui::Button("Place at Camera"))
	{
		// 카메라 정보 (뷰 행렬) 가져오기
		_float4x4 vViewMatrix;
		m_pGraphic_Device->GetTransform(D3DTS_VIEW, &vViewMatrix);

		// 뷰 행렬의 역행렬 계산 (카메라의 월드 변환 행렬)
		D3DXMATRIX vInvViewMatrix;
		D3DXMatrixInverse(&vInvViewMatrix, nullptr, &vViewMatrix);

		// 카메라 위치와 방향 
		_float3 vCameraPos = { vInvViewMatrix._41, vInvViewMatrix._42, vInvViewMatrix._43 };
		_float3 vCameraLook = { vInvViewMatrix._31, vInvViewMatrix._32, vInvViewMatrix._33 };
		vCameraLook.Normalize();

		// 카메라 앞 일정 거리에 배치
		_float3 vDoorPos = vCameraPos + vCameraLook * 5.0f;

		s_vPosition[0] = vDoorPos.x;
		s_vPosition[1] = vDoorPos.y;
		s_vPosition[2] = vDoorPos.z;
	}

	// 텍스처 태그 설정
	ImGui::InputText("Door Texture Tag", s_szTextureTagBuffer, IM_ARRAYSIZE(s_szTextureTagBuffer));

	// 파일 선택 버튼
	static _wstring selectedTexturePath;
	if (ImGui::Button("Select Door Texture"))
	{
		selectedTexturePath = SelectFile();
		if (!selectedTexturePath.empty())
		{
			selectedTexturePath = GetRelativePath(selectedTexturePath);
			LoadImagesFromFolder(selectedTexturePath.substr(0, selectedTexturePath.find_last_of(L"\\")));
			m_bShowImageWindow = true;
		}
	}

	// 문 생성 버튼
	if (ImGui::Button("Create Door"))
	{
		_wstring keyItemTag;

		// KEY 타입인 경우 키 아이템 태그 설정
		if (s_iSelectedDoorType == 1 && s_szKeyItemTagBuffer[0] != '\0')
		{
			wchar_t wKeyItemTag[256] = {};
			MultiByteToWideChar(CP_ACP, 0, s_szKeyItemTagBuffer, -1, wKeyItemTag, 256);
			keyItemTag = wKeyItemTag;
		}

		// 위치 설정
		_float3 vPosition = { s_vPosition[0], s_vPosition[1], s_vPosition[2] };

		// 텍스처 태그 설정
		wchar_t wTextureTag[256] = {};
		MultiByteToWideChar(CP_ACP, 0, s_szTextureTagBuffer, -1, wTextureTag, 256);
		_wstring textureTag = wTextureTag;

		// 문 인스턴스 생성
		CreateDoorInstance(
			keyItemTag,
			s_iSelectedDoorType,
			s_iSelectedDoorColor,
			vPosition,
			s_fSlideDistance,
			s_bStartsActive,
			textureTag
		);
	}
}

HRESULT CMyImGui::CreateDoorInstance(const _wstring& keyItemTag, _uint iDoorType, _uint iDoorColor,
	const _float3& vPosition, _float fSlideDistance, _bool bStartsActive, const _wstring& textureTag)
{
	const _uint iLevel = 3; // 기본 레벨
	const _uint iProtoLevel = 3; // 프로토타입 레벨

	// 문 프로토타입 이름 설정
	_wstring stProtoTag = L"Prototype_GameObject_Door";
	if (static_cast<CDoor::DOOR_TYPE>(iDoorType) == CDoor::DOOR_TYPE::KEY)
	{
		stProtoTag += L"_Key";

		// 색상에 따른 태그 추가
		switch (iDoorColor)
		{
		case 0: stProtoTag += L"_Blue"; break;
		case 1: stProtoTag += L"_Red"; break;
		case 2: stProtoTag += L"_Yellow"; break;
		case 3: stProtoTag += L"_Green"; break;
		default: break;
		}
	}

	// 레이어 태그 설정
	_wstring stLayerTag = L"Layer_Door";

	CDoor::DOOR_DESC tDoorDesc{};
	tDoorDesc.eType = static_cast<CDoor::DOOR_TYPE>(iDoorType);
	tDoorDesc.eColor = static_cast<CDoor::DOOR_COLOR>(iDoorColor);
	tDoorDesc.fSlideDistance = fSlideDistance;
	tDoorDesc.stKeyItemTag = keyItemTag;
	tDoorDesc.bStartsActive = bStartsActive;

	// OBJECT_DESC 부분 설정
	tDoorDesc.iLevel = iLevel;
	tDoorDesc.iProtoLevel = iProtoLevel;
	tDoorDesc.stProtTag = stProtoTag;
	tDoorDesc.stBufferTag = L"Prototype_Component_VIBuffer_TexturedCube";
	tDoorDesc.stProtTextureTag = textureTag;

	// 프로토타입이 존재하는지 확인
	bool prototypeCreated = false;
	if (FAILED(m_pGameInstance->Find_Prototype(stProtoTag)))
	{
		// 프로토타입 생성
		CJsonLoader jsonLoader;
		CBase* pGameObject = jsonLoader.Create_Object_ByClassName("CDoor", m_pGraphic_Device);

		if (FAILED(m_pGameInstance->Add_Prototype(iProtoLevel, stProtoTag, pGameObject)))
		{
			Safe_Release(pGameObject);
			return E_FAIL;
		}
		prototypeCreated = true;
	}

	// 텍스처가 존재하는지 확인하고 텍스처 경로가 있다면 텍스처 추가
	bool textureCreated = false;
	if (FAILED(m_pGameInstance->Find_Prototype(textureTag)) && !m_wstrSelectedTexturePath.empty())
	{
		if (FAILED(m_pGameInstance->Add_Prototype(iLevel, textureTag,
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, ISerializable::WStringToWChar(m_wstrSelectedTexturePath), 1))))
		{
			return E_FAIL;
		}
		textureCreated = true;
	}

	// 게임 오브젝트 추가
	if (FAILED(m_pGameInstance->Add_GameObject(iProtoLevel, stProtoTag,
		iLevel, stLayerTag, &tDoorDesc)))
	{
		return E_FAIL;
	}

	// 생성된 문 오브젝트의 위치 설정
	CGameObject* pDoorObject = m_pGameInstance->Find_Last_Object(iLevel, stLayerTag);
	if (pDoorObject)
	{
		CTransform* pTransform = (CTransform*)pDoorObject->Get_Component(TEXT("Com_Transform"));
		if (pTransform)
		{
			pTransform->Set_State(CTransform::STATE_POSITION, vPosition);
		}
	}

	// JSON에 저장
	if (prototypeCreated || textureCreated)
	{
		SaveObjectToJson(
			"",
			stProtoTag,         // 오브젝트 태그
			iProtoLevel,        // 오브젝트 레벨
			L"CDoor",          // 클래스 이름
			textureTag,         // 텍스처 태그
			iLevel,             // 텍스처 레벨
			m_wstrSelectedTexturePath, // 텍스처 경로
			1,                  // 텍스처 개수
			L"Prototype_Component_VIBuffer_TexturedCube", // 버퍼 태그
			3,                  // 버퍼 레벨
			L"CVIBuffer_TexturedCube",  // 버퍼 클래스 이름
			0, 0                // 버퍼 크기 (기본값)
		);
	}

	return S_OK;
}

CMyImGui* CMyImGui::Create(_uint iNumLevels, LPDIRECT3DDEVICE9 pGraphic_Device)

{
	CMyImGui* pInstance = new CMyImGui(pGraphic_Device);

	if (FAILED(pInstance->Initialize(iNumLevels, pGraphic_Device)))
	{
		MSG_BOX("Failed to Created : CMyImGui");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMyImGui::Free()
{
	__super::Free();
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	for (auto& texture : m_Textures)
	{
		if (texture)
			texture->Release();
	}
	while (!m_vecUndoStack.empty())
	{
		tHistoryItem& item = m_vecUndoStack.top();
		Safe_Delete(item.tObjDesc);
		m_vecUndoStack.pop();
	}

	m_Textures.clear();
	Safe_Release(m_Editor);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pGraphic_Device);
}
