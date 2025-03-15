#include "MyImGui.h"
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
	: m_pGraphic_Device{pGraphic_Device}
	, m_pGameInstance{CGameInstance::Get_Instance()}
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
	ImGui_ImplDX9_Init(m_pGraphic_Device->m_pDevice);
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
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            m_pGraphic_Device->ChangeClearColor(clear_color);
            //counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    //{
    //    ImGui::Begin("Camera");
    //    m_pGameInstance->m_pObject_Manager->m_pLayers
    //    //여기에 쓰고싶은거로 채우기
    //    ImGui::End();
    //}

    ShowLayerInMap(m_pGameInstance->m_pObject_Manager->m_pLayers);

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
    ImGui::EndFrame();
}

HRESULT CMyImGui::Render()
{
    ImGuiIO& io = ImGui::GetIO();
    m_pGraphic_Device->m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pGraphic_Device->m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphic_Device->m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
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
                if(ImGui::CollapsingHeader(WStringToString(pair.first).c_str())) // Key (wstring) 출력
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
    Safe_Release(m_pGraphic_Device);
    Safe_Release(m_pGameInstance);
}
