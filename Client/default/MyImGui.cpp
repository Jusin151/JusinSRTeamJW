#include "MyImGui.h"
#include "Client_Defines.h"
#include "GameInstance.h"
CMyImGui::CMyImGui(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device{pGraphic_Device}
	, m_pGameInstance{CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CMyImGui::Initialize(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX9_Init(m_pGraphic_Device);
}

void CMyImGui::Update(_float fTimeDelta)
{
}

HRESULT CMyImGui::Render()
{
}

CMyImGui* CMyImGui::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
}

void CMyImGui::Free()
{
	CBase::Free();
}
