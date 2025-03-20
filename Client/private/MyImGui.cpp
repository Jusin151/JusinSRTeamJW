#include "MyImGui.h"
#include "Transform.h"
#include "Structure.h"
#include "GameInstance.h"
#include "Layer.h"
#include "JsonLoader.h"
#include "Editor.h"

inline string WStringToString(const wstring& wstr)
{
	if (wstr.empty()) {
		return "";
	}

	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (bufferSize == 0) {
		return ""; // 또는 예외 처리
	}

	string str(bufferSize, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], bufferSize, nullptr, nullptr);
	str.resize(strlen(str.c_str())); // null 문자 제거
	return str;
}

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

	m_Editor = new CEditor;


	return S_OK;
}

void CMyImGui::Update(_float fTimeDelta)
{

}

HRESULT CMyImGui::Render()
{	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuiIO& io = ImGui::GetIO();
	CreateObject();
	Show_Objects();
	//ShowLayerInMap(m_pGameInstance->m_pObject_Manager->m_pLayers);
	m_Editor->RenderUI();
	ImGui::EndFrame();
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	//D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
	//m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
	//ImGuiIO& io = ImGui::GetIO();

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);


	return S_OK;
}

void CMyImGui::Show_Objects()
{

	ImGui::Begin("Objects");

	if (ImGui::Button("Close Window"))
	{
		m_bShowObjectsWindow = false;
		ImGui::End();
		return;
	}

	// 선택된 오브젝트가 있고 인덱스가 유효한 경우
	if (m_pCurrentGameObject)
	{
		ImGui::Separator();


		// 트랜스폼 컴포넌트 가져오기
		CTransform* pTransform = (CTransform*)m_pCurrentGameObject->Get_Component(TEXT("Com_Transform"));
		string stLayerTag = ISerializable::WideToUtf8(m_pCurrentGameObject->Get_Tag());
		if (pTransform != nullptr)
		{
			ImGui::Text(stLayerTag.c_str());
			ImGui::Separator();
			ImGui::Text("Object Position");

			// 현재 위치 가져오기
			_float3 position = pTransform->Get_State(CTransform::STATE_POSITION);
			_float3 scale = pTransform->Compute_Scaled();
			_float3 euler = pTransform->Get_EulerAngles();
			_float3 rotation;
			bool positionChanged = false;
			bool scaleChanged = false;
			bool rotationChanged = false;

			// X 위치 조절
			ImGui::Text("X"); ImGui::SameLine();
			positionChanged |= ImGui::DragFloat("PositionX", &position.x, 0.1f,-FLT_MAX, FLT_MAX);

			// Y 위치 조절
			ImGui::Text("Y"); ImGui::SameLine();
			positionChanged |= ImGui::DragFloat("PositionY", &position.y, 0.1f, -FLT_MAX, FLT_MAX);

			// Z 위치 조절
			ImGui::Text("Z"); ImGui::SameLine();
			positionChanged |= ImGui::DragFloat("PositionZ", &position.z, 0.1f, -FLT_MAX, FLT_MAX);

			ImGui::Separator();
			ImGui::Text("Object Scale");
			// X 위치 조절
			ImGui::Text("X"); ImGui::SameLine();
			scaleChanged |= ImGui::DragFloat("ScaleX", &scale.x, 0.1f, 1.f, FLT_MAX);

			// Y 위치 조절
			ImGui::Text("Y"); ImGui::SameLine();
			scaleChanged |= ImGui::DragFloat("ScaleY", &scale.y, 0.1f, 1.f, FLT_MAX);

			// Z 위치 조절
			ImGui::Text("Z"); ImGui::SameLine();
			scaleChanged |= ImGui::DragFloat("ScaleZ", &scale.z, 0.1f, 1.f, FLT_MAX);

			ImGui::Separator();
			ImGui::Text("Object Rotation");
			// X 위치 조절
			ImGui::Text("X"); ImGui::SameLine();
			rotationChanged |= ImGui::DragFloat("RotationX", &euler.x, 1.f, 0.f, 360.f);

			// Y 위치 조절
			ImGui::Text("Y"); ImGui::SameLine();
			rotationChanged |= ImGui::DragFloat("RotationY", &euler.y, 1.f, 0.f, 360.f);

			// Z 위치 조절
			ImGui::Text("Z"); ImGui::SameLine();
			rotationChanged |= ImGui::DragFloat("RotationZ", &euler.z, 1.f, 0.f, 360.f);


			// 위치가 변경되면 트랜스폼 업데이트
			if (positionChanged)
			{
				pTransform->Set_State(CTransform::STATE_POSITION, position);
			}

			if (scaleChanged)
			{
				pTransform->Set_Scale(scale.x, scale.y, scale.z);
			}

			if (rotationChanged)
			{
				pTransform->Rotate_EulerAngles(euler);
			}
		}
	}

	ImGui::End();
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
					m_pGraphic_Device,  // Direct3D 디바이스
					filePath.c_str(),
					&texture);

				if (SUCCEEDED(hr))
				{
					m_Textures.push_back(texture);

				}
			}

		} while (FindNextFileW(hFind, &findData) != 0);

		FindClose(hFind);
	}
}

_wstring CMyImGui::SelectFile()
{
	_wstring selectedPath;

	// COM 초기화
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

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
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

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

HRESULT CMyImGui::CreateObject()
{
	static int selectedColliderType = 0;
	static int selectedBufferType = 0;
	static int selectedClassType = 0;
	static char colliderNameBuffer[256] = "Prototype_Component_Collider_Cube"; // 콜라이더 프로토타입
	static char bufferNameBuffer[256] = "Prototype_Component_VIBuffer_Cube"; // 버퍼 프로토타입
	static char layerTagBuffer[128] = "Com_Collider"; // 콜라이더 컴포넌트 태그
	static char objectLayerTagBuffer[256] = "Layer_"; // 오브젝트 태그
	static char textureTagBuffer[256] = "Prototype_Component_Texture"; // 텍스쳐 태그
	static char objectClassTagBuffer[256] = "C"; // 클래스 이름
	static char objectProtoTagBuffer[256] = "Prototype_GameObject_"; // 프토로타입 태그
	static _int iPoolingCount = 0; // 풀링 할 개수
	static _int iLevel = 4; // 어느 레벨에 둘지 (테스트용 에디터로)
	static _int iProtoLevel = 4; // 어느 프로토 레벨에 둘지

	// JSON에서 로드한 클래스 이름 목록
	static _bool bClassNamesLoaded = false;
	static vector<string> classNames;
	static vector<const char*> classNamesCStr;

	// JSON 파일에서 클래스 이름 로드 (최초 한 번만 실행)
	if (!bClassNamesLoaded)
	{
		CJsonLoader jsonLoader;
		// JSON 파일 로드 (Data 폴더의 ClassNames.json 파일에서 로드)
		if (SUCCEEDED(jsonLoader.LoadClassNamesFromJson("../Save/ClassNames.json", classNames)))
		{
			// std::string을 const char*로 변환하여 저장
			classNamesCStr.clear();
			for (const auto& name : classNames)
			{
				classNamesCStr.push_back(name.c_str());
			}

			bClassNamesLoaded = true;
		}
		else
		{
			// 로드 실패 시 기본값 설정 (Create_Object_ByClassName 함수에 있는 클래스들로 초기화)
			classNames = {
				"CPlayer", "CTestMonster", "CTerrain", "CStructure",
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
	// 선택된 클래스 이름으로 업데이트
	strcpy_s(objectClassTagBuffer, classNames[selectedClassType].c_str());
	// 충돌체 타입 배열
	static const char* colliderTypes[] = { "Cube", "Sphere" };
	static const char* colliderProtoNames[] = {
		"Prototype_Component_Collider_Cube",
		"Prototype_Component_Collider_Sphere"
	};

	// 버퍼 타입 배열
	static const char* bufferTypes[] = { "Cube", "Rect","Terrain"};
	static const char* bufferProtoNames[] = {
		"Prototype_Component_VIBuffer_Cube",
		"Prototype_Component_VIBuffer_Rect",
		"Prototype_Component_VIBuffer_Terrain"
	};

	// ImGui 컴포넌트 선택 인터페이스 표시
	ImGui::Begin("Object Attribute");

	// 충돌체 타입 드롭다운
	if (ImGui::Combo("Collider Type", &selectedColliderType, colliderTypes, IM_ARRAYSIZE(colliderTypes)))
	{
		// 충돌체 타입이 변경되면 컴포넌트 이름 업데이트
		strcpy_s(colliderNameBuffer, colliderProtoNames[selectedColliderType]);
	}

	// 버퍼 타입 드롭다운
	if (ImGui::Combo("Buffer Type", &selectedBufferType, bufferTypes, IM_ARRAYSIZE(bufferTypes)))
	{
		strcpy_s(bufferNameBuffer, bufferProtoNames[selectedBufferType]);
	}

	// 클래스 타입 드롭다운 (JSON에서 로드한 클래스 이름 사용)
	if (ImGui::Combo("Class Type", &selectedClassType, classNamesCStr.data(), static_cast<int>(classNamesCStr.size())))
	{
		// 선택된 클래스 이름으로 업데이트
		strcpy_s(objectClassTagBuffer, classNames[selectedClassType].c_str());
	}

	// 충돌체 컴포넌트 이름 입력 필드
	ImGui::InputText("Collider Tag", colliderNameBuffer, IM_ARRAYSIZE(colliderNameBuffer));

	// 레이어 태그 입력
	ImGui::InputText("Component Layer Tag", layerTagBuffer, IM_ARRAYSIZE(layerTagBuffer));

	ImGui::InputText("Object Proto Layer Tag", objectProtoTagBuffer, IM_ARRAYSIZE(objectProtoTagBuffer));
	ImGui::InputText("Object Layer Tag", objectLayerTagBuffer, IM_ARRAYSIZE(objectLayerTagBuffer));

	// 충돌체 컴포넌트 이름 입력 필드
	ImGui::InputText("Texture Tag", textureTagBuffer, IM_ARRAYSIZE(textureTagBuffer));

	ImGui::InputInt("Pool Count", &iPoolingCount);
	ImGui::InputInt("Level", &iLevel);
	ImGui::InputInt("ProtoLevel", &iProtoLevel);

	static _wstring selectedFolder;
	if (ImGui::Button("Select Texture"))
	{
		selectedFolder = SelectFile();
		if (!selectedFolder.empty())
		{
			selectedFolder = GetRelativePath(selectedFolder);
			m_bShowImageWindow = true;
		}
	}

	// 생성 버튼
	if (ImGui::Button("Create"))
	{
		// char에서 wchar_t로 문자열 변환을 위한 변수 및 버퍼
		wchar_t wBufferName[256] = {};
		wchar_t wColliderName[256] = {};
		wchar_t wLayerTag[128] = {};
		wchar_t wtextureTag[256] = {};
		wchar_t wobjectClassTag[256] = {};
		wchar_t wobjectProtoTagBuffer[256] = {};
		wchar_t wobjectLayerTagBuffer[256] = {};

		// MultiByteToWideChar 함수를 사용하여 변환
		MultiByteToWideChar(CP_ACP, 0, bufferNameBuffer, -1, wBufferName, 256);
		MultiByteToWideChar(CP_ACP, 0, colliderNameBuffer, -1, wColliderName, 256);
		MultiByteToWideChar(CP_ACP, 0, layerTagBuffer, -1, wLayerTag, 128);
		MultiByteToWideChar(CP_ACP, 0, textureTagBuffer, -1, wtextureTag, 256);
		MultiByteToWideChar(CP_ACP, 0, objectClassTagBuffer, -1, wobjectClassTag, 256);
		MultiByteToWideChar(CP_ACP, 0, objectProtoTagBuffer, -1, wobjectProtoTagBuffer, 256);
		MultiByteToWideChar(CP_ACP, 0, objectLayerTagBuffer, -1, wobjectLayerTagBuffer, 256);

		CJsonLoader jsonLoader;

		CBase* pGameObject = jsonLoader.Create_Object_ByClassName(ISerializable::WideToUtf8(wobjectClassTag), m_pGraphic_Device);

		_wstring stProtoTag = ISerializable::Utf8ToWide(ISerializable::WideToUtf8(wobjectProtoTagBuffer));
		_wstring stLayerTag = ISerializable::Utf8ToWide(ISerializable::WideToUtf8(wobjectLayerTagBuffer));
		_wstring stTextureTag = ISerializable::Utf8ToWide(ISerializable::WideToUtf8(wtextureTag));

		CGameObject::OBJECT_DESC tObjDesc{};
		tObjDesc.iLevel = iLevel;
		tObjDesc.iProtoLevel = iProtoLevel;
		tObjDesc.stProtTag = stProtoTag;
		tObjDesc.stBufferTag = wBufferName;
		tObjDesc.stProtTextureTag = stTextureTag;
		tObjDesc.iPoolCount = iPoolingCount;

		if (FAILED(m_pGameInstance->Find_Prototype(stTextureTag)))
		{
			if (FAILED(m_pGameInstance->Add_Prototype(iLevel, stTextureTag,
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D, ISerializable::WStringToWChar(selectedFolder), 1))))
			{
				ImGui::End();
				return E_FAIL;
			}
		}

		if (FAILED(m_pGameInstance->Find_Prototype(stProtoTag)))
		{
			if (FAILED(m_pGameInstance->Add_Prototype(iProtoLevel, stProtoTag, pGameObject)))
			{
				Safe_Release(pGameObject);
				ImGui::End();
				return E_FAIL;
			}
		}

		// 게임 오브젝트 추가
		if (iPoolingCount != 0)
		{
			if (FAILED(m_pGameInstance->Reserve_Pool(iProtoLevel, stProtoTag,
				stLayerTag, iPoolingCount, &tObjDesc)))
			{
				ImGui::End();
				return E_FAIL;
			}
		}
		else
		{
			if (FAILED(m_pGameInstance->Add_GameObject(iProtoLevel, stProtoTag,
				iLevel, stLayerTag, &tObjDesc)))
			{
				ImGui::End();
				return E_FAIL;
			}
		}
	}

	//// JSON 저장 버튼 추가
	//if (ImGui::Button("Save Classes to JSON"))
	//{
	//	// classNames 벡터를 JSON 파일로 저장
	//	SaveClassNamesToJson("./Data/ClassNames.json", classNames);
	//}

	ImGui::End();
	return S_OK;
}

_wstring CMyImGui::GetRelativePath(const _wstring& absolutePath)
{
	if (absolutePath.empty()) return absolutePath;
	_wstring relativePath = L"../../" + absolutePath.substr(absolutePath.find(L"Resources"));

	return relativePath;
}
_bool CMyImGui::IsMouseOverImGui()
{
	ImGuiIO& io = ImGui::GetIO();
	return io.WantCaptureMouse;
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

	Safe_Release(m_Editor);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pGraphic_Device);
}
