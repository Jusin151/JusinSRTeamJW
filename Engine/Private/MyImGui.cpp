#include "MyImGui.h"
#include "Transform.h"
#include "VIBuffer_Cube.h"
#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Object_Manager.h"
#include "Layer.h"

#include "GameObject.h"


inline string WStringToString(const wstring& wstr) {
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

CMyImGui::CMyImGui(CGraphic_Device* pGraphic_Device)
	: m_pGraphic_Device{ pGraphic_Device }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CMyImGui::Initialize(_uint iNumLevels, HWND hWnd, CGraphic_Device* pGraphic_Device)
{
	m_iNumLevels = iNumLevels;
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
	ImGui_ImplWin32_Init(hWnd);

	ImGui_ImplDX9_Init(pGraphic_Device->m_pDevice);


	m_pVIBuffer_CubeVec.reserve(50);
	m_pTransformVec.reserve(50);
	// 큐브 버퍼 생성
	m_pVIBuffer_CubeVec.emplace_back(CVIBuffer_Cube::Create(pGraphic_Device->m_pDevice));
	if (nullptr == m_pVIBuffer_CubeVec.front())
		return E_FAIL;

	// 트랜스폼 컴포넌트 생성 (큐브의 위치, 회전, 크기 조절용)
	m_pTransformVec.emplace_back(CTransform::Create(pGraphic_Device->m_pDevice));
	if (nullptr == m_pTransformVec.front())
		return E_FAIL;

	// 큐브 초기 위치 및 크기 설정
	m_pTransformVec.front()->Set_State(CTransform::STATE_POSITION, _float3(0.f, 0.f, 0.f));
	m_pTransformVec.front()->Set_Scale(1.f, 1.f, 1.f);


	return S_OK;
}

void CMyImGui::Update(_float fTimeDelta)
{
	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuiIO& io = ImGui::GetIO();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	//if (show_demo_window)
	//    ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{

		ImGui::Begin("Editor");                          // Create a window called "Hello, world!" and append into it.

		if (ImGui::Button("Select Folder and Load Images"))
		{
			_wstring selectedFolder = SelectFolder();
			if (!selectedFolder.empty())
			{
				LoadImagesFromFolder(selectedFolder);
				m_bShowImageWindow = true;
			}
		}
		ImGui::End();

		Show_Texture_Image();
		CreateObject();
		Show_Objects();
	}


	//{
	//    ImGui::Begin("Camera");
	//    m_pGameInstance->m_pObject_Manager->m_pLayers
	//    //여기에 쓰고싶은거로 채우기
	//    ImGui::End();
	//}

	ShowLayerInMap(m_pGameInstance->m_pObject_Manager->m_pLayers);

	// 3. Show another simple window.
	//if (show_another_window)
	//{
	//    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	//    ImGui::Text("Hello from another window!");
	//    if (ImGui::Button("Close Me"))
	//        show_another_window = false;
	//    ImGui::End();
	//}

	ImGui::EndFrame();
}

HRESULT CMyImGui::Render()
{
	// 선택된 텍스처로 큐브 렌더링
	if (m_bRenderSelectedCube && m_SelectedTextureIndex != -1 &&
		m_SelectedTextureIndex < m_Textures.size() && m_Textures[m_SelectedTextureIndex])
	{

		for (int i = 0; i < (int)m_pVIBuffer_CubeVec.size(); i++)
		{

			// 텍스처 설정
			m_pGraphic_Device->m_pDevice->SetTexture(0, m_Textures[m_SelectedTextureIndex]);

			m_pTransformVec[i]->Bind_Resource();
			m_pVIBuffer_CubeVec[i]->Bind_Buffers();
			m_pVIBuffer_CubeVec[i]->Render();
		}
	}


	m_pGraphic_Device->m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphic_Device->m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
	//m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}


void CMyImGui::Show_Texture_Image()
{
	if (m_Textures.empty() || !m_bShowImageWindow)
		return;

	ImGui::Begin("Textures");

	if (ImGui::Button("Close Window"))
	{
		m_bShowImageWindow = false;
		ImGui::End();
		return;
	}

	// 디버그용 정보 - 로드된 이미지 수 표시
	ImGui::Text("Load Image count: %d", (int)m_Textures.size());

	const int imagesPerRow = 4; // 한 줄에 표시할 이미지 수 

	for (_ulonglong i = 0; i < m_Textures.size(); ++i)
	{
		// 텍스처가 유효한지 확인
		if (m_Textures[i])
		{
			// 새 줄의 시작인 경우 SameLine을 호출하지 않음
			if (i % imagesPerRow != 0)
				ImGui::SameLine();

			// 각 항목을 세로로 정렬하기 위한 그룹 시작
			ImGui::BeginGroup();

			// 이미지 표시
			ImGui::Image((ImTextureID)m_Textures[i], ImVec2(100, 100));

			// 선택된 텍스처에 표시 추가
			if (m_SelectedTextureIndex == i)
			{
				ImGui::SameLine();
				ImGui::Text("*"); // 별표로 선택 상태 표시
			}

			// 버튼 표시
			if (ImGui::Button(("Apply to Cube " + to_string(i)).c_str()))
			{
				m_SelectedTextureIndex = i;
				m_bRenderSelectedCube = true; // 큐브 렌더링 활성화
			}


			ImGui::EndGroup();
		}
		else
		{
			if (i % imagesPerRow != 0)
				ImGui::SameLine();

			ImGui::Text("Invalid texture at index %d", (int)i);
		}
	}

	if (m_SelectedTextureIndex != -1 && m_SelectedTextureIndex < m_Textures.size())
	{
		ImGui::Separator();
		ImGui::Text("Selected Texture: %d", (int)m_SelectedTextureIndex);



		//// 큐브 크기 조절
		//_float3 scale = m_pTransform->Compute_Scaled();
		//bool scaleChanged = false;
		//ImGui::Text("Scale");

		//// X 스케일 조절
		//ImGui::Text("X"); ImGui::SameLine();
		//scaleChanged |= ImGui::DragFloat("##ScaleX", &scale.x, 0.01f, -FLT_MAX, FLT_MAX);  // 최소값 -FLT_MAX, 최대값 FLT_MAX

		//// Y 스케일 조절 
		//ImGui::Text("Y"); ImGui::SameLine();
		//scaleChanged |= ImGui::DragFloat("##ScaleY", &scale.y, 0.01f, -FLT_MAX, FLT_MAX);  // 최소값 -FLT_MAX, 최대값 FLT_MAX

		//// Z 스케일 조절
		//ImGui::Text("Z"); ImGui::SameLine();
		//scaleChanged |= ImGui::DragFloat("##ScaleZ", &scale.z, 0.01f, -FLT_MAX, FLT_MAX);  // 최소값 -FLT_MAX, 최대값 FLT_MAX

		//if (scaleChanged)
		//{
		//    m_pTransform->Set_Scale(scale.x, scale.y, scale.z);
		//}

	}

	ImGui::End();
}

void CMyImGui::Show_Objects()
{
	if (m_pVIBuffer_CubeVec.empty() || m_pTransformVec.empty()) return;
	if (m_iCurrentObject < 0 || m_pVIBuffer_CubeVec.size() <= m_iCurrentObject) return;

	ImGui::Begin("Objects");

	if (ImGui::Button("Close Window"))
	{
		m_bShowObjectsWindow = false;
		ImGui::End();
		return;
	}

	//if (ImGui::Button("Add New Cube"))
	//{
	//	
	//}

	// 디버그용 정보 - 로드된 이미지 수 표시
	ImGui::Text("Current Object count: %d", (int)m_pVIBuffer_CubeVec.size());


	for (_ulonglong i = 0; i < m_pVIBuffer_CubeVec.size(); ++i)
	{
		// 텍스처가 유효한지 확인
		if (m_pVIBuffer_CubeVec[i])
		{

			// 각 항목을 세로로 정렬하기 위한 그룹 시작
			ImGui::BeginGroup();

			//// 이미지 표시
			//ImGui::Image((ImTextureID)m_Textures[i], ImVec2(100, 100));

			//// 선택된 텍스처에 표시 추가
			//if (m_SelectedTextureIndex == i)
			//{
			//    ImGui::SameLine();
			//    ImGui::Text("*"); // 별표로 선택 상태 표시
			//}

			// 버튼 표시
			if (ImGui::Button(("Select" + to_string(i)).c_str()))
			{
				m_iCurrentObject = i;
				m_bRenderSelectedCube = true; // 큐브 렌더링 활성화
			}

			ImGui::EndGroup();
		}
	}

	if (m_iCurrentObject != -1 && m_iCurrentObject < m_pVIBuffer_CubeVec.size())
	{
		ImGui::Separator();

		// 큐브 렌더링 활성화/비활성화
		ImGui::Checkbox("Show Cube", &m_bRenderSelectedCube);


		ImGui::Text("Cube Position");

		// 위치 조절 (드래그 슬라이더 + 직접 입력)
		_float3 position = m_pTransformVec[m_iCurrentObject]->Get_State(CTransform::STATE_POSITION);
		bool positionChanged = false;

		// X 위치 조절
		ImGui::Text("X"); ImGui::SameLine();
		positionChanged |= ImGui::DragFloat("PositionX", &position.x, 0.1f);

		// Y 위치 조절
		ImGui::Text("Y"); ImGui::SameLine();
		positionChanged |= ImGui::DragFloat("PositionY", &position.y, 0.1f);

		// Z 위치 조절
		ImGui::Text("Z"); ImGui::SameLine();
		positionChanged |= ImGui::DragFloat("PositionZ", &position.z, 0.1f);

		if (positionChanged)
		{
			m_pTransformVec[m_iCurrentObject]->Set_State(CTransform::STATE_POSITION, position);
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
					m_pGraphic_Device->m_pDevice,  // Direct3D 디바이스
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
	static char colliderNameBuffer[256] = "Prototype_Component_Collider_Cube"; // 기본값
	static char bufferNameBuffer[256] = "Prototype_Component_VIBuffer_Cube"; // 기본값
	static char layerTagBuffer[128] = "Com_Collider"; // 기본값
	static char textureTagBuffer[256] = "Prototype_Component_Texture"; // 기본값

	// 충돌체 타입 배열
	static const char* colliderTypes[] = { "Cube", "Sphere" };
	static const char* colliderProtoNames[] = {
		"Prototype_Component_Collider_Cube",
		"Prototype_Component_Collider_Sphere"
	};

	// 버퍼 타입 배열
	static const char* bufferTypes[] = { "Cube", "Rect" };
	static const char* bufferProtoNames[] = {
		"Prototype_Component_VIBuffer_Cube",
		"Prototype_Component_VIBuffer_Rect"
	};

	// ImGui 컴포넌트 선택 인터페이스 표시
	ImGui::Begin("Structure Attricbute");

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

	// 충돌체 컴포넌트 이름 입력 필드
	ImGui::InputText("Collider Tag", colliderNameBuffer, IM_ARRAYSIZE(colliderNameBuffer));

	// 레이어 태그 입력
	ImGui::InputText("Layer Tag", layerTagBuffer, IM_ARRAYSIZE(layerTagBuffer));
	
	// 충돌체 컴포넌트 이름 입력 필드
	ImGui::InputText("Texture Tag", textureTagBuffer, IM_ARRAYSIZE(textureTagBuffer));

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


	//// 폴더 선택 버튼과 "생성" 버튼 사이에 추가
	//if (m_bShowImageWindow && !m_Textures.empty())
	//{
	//	ImGui::Text("Selected Texture: %s",
	//		(selectedFolder.empty()) ?
	//		WStringToString(GetRelativePath(selectedFolder)).c_str() :
	//		"None");
	//}

	// 생성 버튼
	if (ImGui::Button("Create"))
	{
		// char에서 wchar_t로 문자열 변환을 위한 변수 및 버퍼
		wchar_t wBufferName[256] = {};
		wchar_t wColliderName[256] = {};
		wchar_t wLayerTag[128] = {};
		wchar_t wtextureTag[128] = {};

		// MultiByteToWideChar 함수를 사용하여 변환
		MultiByteToWideChar(CP_ACP, 0, bufferNameBuffer, -1, wBufferName, 256);
		MultiByteToWideChar(CP_ACP, 0, colliderNameBuffer, -1, wColliderName, 256);
		MultiByteToWideChar(CP_ACP, 0, layerTagBuffer, -1, wLayerTag, 128);
		MultiByteToWideChar(CP_ACP, 0, textureTagBuffer, -1, wtextureTag, 256);

		// 구조체에 변환된 문자열 포인터 할당
		CStructure::STRUCTURE_DESC stDesc;
		stDesc.vPos = _float3(0.f, 0.f, 0.f);
		stDesc.stVIBuffer = wBufferName;
		stDesc.stCollProtoTag = wColliderName;
		stDesc.stTextureTag = wtextureTag;
		stDesc.stTexturePath = selectedFolder;

		// 게임 오브젝트 추가
		if (FAILED(m_pGameInstance->Add_GameObject(0, TEXT("Prototype_GameObject_Structure"),
			3, TEXT("Layer_Structure"), &stDesc)))
			return E_FAIL;
	}

	ImGui::End();

	return S_OK;
}

_wstring CMyImGui::GetRelativePath(const _wstring& absolutePath)
{
	if (absolutePath.empty()) return absolutePath;
	_wstring relativePath = L"../../" + absolutePath.substr(absolutePath.find(L"Resources"));
	
	return relativePath;
}

void CMyImGui::ShowLayerInMap(map<const _wstring, class CLayer*>* pLayers)
{
	if (ImGui::BeginTable("MyMapTable", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders)) {
		// 헤더 설정
		ImGui::TableSetupColumn("Layer Key");
		//ImGui::TableSetupColumn("Objects"); // 오브젝트 리스트를 표시할 열
		ImGui::TableHeadersRow();

		// 맵의 각 요소를 순회하며 테이블에 행 추가
		for (_uint i = 0; i < m_iNumLevels; ++i) {
			// std::map<std::wstring, CLayer*> iter = pLayers[i];
			for (auto& pair : pLayers[i])
			{
				ImGui::TableNextRow(); // 새 행 시작
				ImGui::TableNextColumn(); // 첫 번째 열 (Key)
				if (ImGui::CollapsingHeader(WStringToString(pair.first).c_str())) // Key (wstring) 출력
					ShowListInLayer(pair.second);
			}
		}
		ImGui::EndTable();
	}
}

void CMyImGui::ShowListInLayer(CLayer* pLayer)
{
	//내부 테이블
	if (ImGui::BeginTable("ObjectsTable", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
	{
		ImGui::TableSetupColumn("ID");
		//ImGui::TableSetupColumn("Components");
		ImGui::TableHeadersRow();
		int i = 0;
		for (const auto& pGameObject : pLayer->m_GameObjects)
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			if (ImGui::CollapsingHeader(to_string(i).c_str()))
				ShowComponentsInGameObject(pGameObject);
			//ImGui::TableNextColumn();
			//ImGui::Text("%d", pGameObject->GetID());
			++i;
		}
		ImGui::EndTable();
	}
}

void CMyImGui::ShowComponentsInGameObject(CGameObject* pGameObject)
{
	if (ImGui::BeginTable("ObjectsTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
	{
		ImGui::TableSetupColumn("Components");
		ImGui::TableHeadersRow();
		for (const auto& pair : pGameObject->m_Components)
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			if (ImGui::CollapsingHeader(WStringToString(pair.first).c_str()))
			{

			}
		}
		ImGui::EndTable();
	}
}

CMyImGui* CMyImGui::Create(_uint iNumLevels, HWND hWnd, CGraphic_Device* pGraphic_Device)

{
	CMyImGui* pInstance = new CMyImGui(pGraphic_Device);

	if (FAILED(pInstance->Initialize(iNumLevels, hWnd, pGraphic_Device)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
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
	m_Textures.clear();
	for (auto& cube : m_pVIBuffer_CubeVec)
	{
		Safe_Release(cube);
	}
	m_pVIBuffer_CubeVec.clear();
	for (auto& transform : m_pTransformVec)
	{
		Safe_Release(transform);
	}
	m_pTransformVec.clear();

	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pGameInstance);
}
